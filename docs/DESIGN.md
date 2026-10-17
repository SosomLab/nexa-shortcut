# nexa-shortcut 설계

## 목표

1. **nShiftSpace** (Nexa ShiftSpace) — Shift+Space 한/영 전환 단일 기능. 초경량(파일 크기·메모리). ✅ 완료
2. **nexa-mapper** — 사용자가 간단한 키 매핑을 직접 설정해 쓰는 범용 매퍼.
   자주 안 쓰는 키를 다른 용도로 재매핑하는 수준의 단순한 기능. 초경량 원칙 유지.
3. **exe 내보내기** — 설정한 매핑만 담긴 독립 exe를 생성하는 기능.

## 공통 초경량 원칙

- C + Win32 API만 사용. GUI 프레임워크·CRT 미사용 (`-nostdlib`, 진입점 `start`).
- 링크 라이브러리는 kernel32 / user32 / shell32 로 제한. 예외: advapi32 — "로그인 시 자동 실행"의
  HKCU `Run` 값 읽기/쓰기 전용 3개 함수, imm32 — `ImmDisableIME` 하나. 둘 다 user32가 이미 로드하는 DLL이라
  메모리 추가 부담은 없다.
- 큰 구조체는 전역(.bss)에 두어 컴파일러의 `memset` 호출 생성을 회피.
- 메모리 표시 최소화 두 가지: `ImmDisableIME(-1)`로 이 스레드의 IME/TSF 스택이 붙지 않게 하고, 마지막 메시지 2초 뒤
  일회성 타이머로 `SetProcessWorkingSetSize(-1, -1)` (작업 관리자 "메모리" ≈1MB → 0.07~0.12MB. 아래 "메모리" 절).
- 결과물 기준: 목표 1은 v0.1.0에서 4,608B 달성 (원본 jwShiftSpaceKey.exe 12KB 대비 1/3 수준).
  "로그인 시 자동 실행" 메뉴 추가 후 x64 5,120B / x86 5,632B (2026-10-17).
- 아이콘도 초경량: 16×16 1bpp 단일 이미지 ICO(198바이트)를 스크립트(`tools/make_icon.py`)로
  생성해 리소스로 내장. 다중 해상도·트루컬러 ICO(수십 KB)는 사용하지 않는다.
- **부가 기능은 컴파일 플래그로 분리한 별도 빌드**로 제공한다 (예: `NSS_LED`). 기본 빌드는 변형 코드와
  리소스를 전혀 포함하지 않아야 한다. 변형 빌드 안에서의 런타임 토글은 허용하되 설정은 저장하지 않는다
  (무설정 원칙). 유일한 예외는 사용자가 메뉴에서 명시적으로 켠 "로그인 시 자동 실행"의 `Run` 값 하나다.

## 목표 1 아키텍처 (구현됨: `src/nShiftSpace.c`)

```
start() ─ 뮤텍스(중복실행 방지) ─ ImmDisableIME(-1)
        ─ 숨은 윈도우 생성 + RegisterHotKey(Shift+Space)
        ─ [LED판] Raw Input 등록 (RIDEV_INPUTSINK)
        ─ 트레이 아이콘 (Shell_NotifyIcon)
        └ 메시지 루프
             WM_HOTKEY   → keybd_event(VK_HANGUL)  # 한/영 키 합성
             WM_TRAYICON → 메뉴: 로그인 시 자동 실행(체크) / [LED판] 키보드 LED 표시(체크) / 종료
             (모든 메시지) → 2초 일회성 타이머 (재)설정
             WM_TIMER    → KillTimer + 작업 집합 비우기
             WM_COMMAND  → Run 값 등록·삭제 / [LED판] Raw Input 등록·해제 / 종료
             [LED판] WM_INPUT → 키 눌림/뗌 → 트레이 아이콘 초록/파랑
             TaskbarCreated → 트레이 아이콘 복구 (explorer 재시작 대응)
```

- `RegisterHotKey`는 OS가 키를 가로채므로 훅보다 단순하고 CPU 비용이 0에 가깝다.
- 한계: 관리자 권한 창에 포커스가 있으면 일반 권한 프로세스의 단축키는 동작하지 않음(원본도 동일).

## 목표 1 변형 — 키보드 LED 표시판 (구현됨: `-DNSS_LED`, `nShiftSpace-led-*.exe`)

아무 키나 눌리는 동안 트레이 아이콘을 **켜짐(초록)** 으로, 떼면 **꺼짐(파랑)** 으로 바꾸는 시각 표시.
픽셀 맵은 같고 바탕색만 다른 두 번째 ICO(`res/nShiftSpace-on.ico`, 198바이트)를 리소스 ID 2로 넣는다.

```
트레이 메뉴 "키보드 LED 표시" (체크, 기본 켜짐)
   켜기 → RegisterRawInputDevices(키보드, RIDEV_INPUTSINK)
   끄기 → RegisterRawInputDevices(키보드, RIDEV_REMOVE)  # WM_INPUT 이 전혀 오지 않음 = 일반판과 동일 비용
WM_INPUT
   → GetRawInputData → RI_KEY_BREAK 비트로 눌림/뗌 판별
   → 상태가 바뀔 때만 Shell_NotifyIcon(NIM_MODIFY)  # 키 반복 입력은 셸 호출 없음
   → DefWindowProc 로 넘겨 user32 가 버퍼 정리
```

### 감지 방식: 훅이 아니라 Raw Input

| 후보 | 판단 |
|---|---|
| `SetWindowsHookEx(WH_KEYBOARD_LL)` | ❌ 모든 키 입력이 우리 콜백을 **동기적으로** 거쳐 시스템 전체 입력이 지연된다. 콜백 안에서 `Shell_NotifyIcon`(explorer로의 동기 호출)을 부르면 훅 타임아웃 시 훅이 조용히 제거될 위험 |
| `GetAsyncKeyState` 폴링(타이머) | ❌ 유휴 CPU 0 원칙 위반 |
| **`RegisterRawInputDevices(RIDEV_INPUTSINK)`** | ✅ 키 make/break가 `WM_INPUT` 메시지로 **비동기** 전달. 다른 프로세스의 입력 경로에 개입하지 않고, 훅이 아니므로 백신 오탐 소지도 낮다 |

- 여러 키 동시 입력은 추적하지 않는다: 어느 키든 떼면 꺼진다 (요구가 "눌리면 켜고 떼면 끈다"이고, 눌린 키 개수를 세면 포커스 전환 등으로 상태가 꼬일 수 있다).
- 탐색기 재시작(`TaskbarCreated`)으로 아이콘을 다시 추가할 때 꺼짐 상태로 초기화한다. Raw Input 등록은 우리 창에 묶여 있어 그대로 유지된다.
- 아이콘 핸들 2개는 리소스 공유 핸들(`LoadIcon`)이라 해제하지 않는다.
- 두 판은 같은 뮤텍스·같은 단축키를 쓰므로 동시에 실행되지 않는다.

### 켜고 끄기(런타임 토글)의 성능 의미

LED를 끄면 Raw Input 등록을 **해제**(`RIDEV_REMOVE`, 이때 `hwndTarget`은 NULL이어야 함)하므로
키 입력 시 우리 프로세스가 깨어나는 일 자체가 없어진다. 즉 꺼진 LED판의 런타임 비용은 일반판과 같다.
남는 차이는 파일 크기(+1KB)와 켜짐 아이콘 리소스뿐이다. 토글 상태는 저장하지 않으며 시작 시 항상 켜짐이다
(레지스트리·설정 파일을 쓰지 않는 무설정 원칙 — LED를 상시 원하지 않으면 일반판을 쓰면 된다).

### 비용 실측과 별도 빌드로 분리한 근거 (2026-10-17, GCC 16.2 mingw-w64)

| 항목 | 일반판 | LED판 |
|---|---|---|
| 파일 크기 x64 / x86 | 5,120 / 5,632 B | **6,144 / 6,656 B** (+1,024) |
| 섹션 증가 (x64, 토글 포함) | — | `.text` +432 (Raw Input 처리·메뉴 토글), `.idata` +84 (import 2개), `.rsrc` +296 (켜짐 아이콘), `.bss` +96 |
| 사설 메모리, 기동 직후 | ≈1.4 MB | ≈1.4 MB (차이 없음 — 로드되는 DLL이 지배) |
| 키 입력 시 비용 (LED 켜짐) | 0 (Shift+Space 외 무관심) | 눌림/뗌마다 `WM_INPUT` 1회 + 상태 변화 시 `NIM_MODIFY` 1회 |
| 키 입력 시 비용 (LED 꺼짐) | 0 | 0 (등록 해제) |

- 런타임 토글만으로 한 exe에 합치면 LED를 쓰지 않는 사용자도 +1KB와 켜짐 아이콘을 떠안는다
  → **컴파일 플래그 `NSS_LED`로 분리**한다. LED 분리 단계에서는 일반판이 v0.1.0 소스 빌드와 코드 섹션까지 동일함을
  확인했다 (PE 타임스탬프·체크섬 4바이트만 상이). 이후 "로그인 시 자동 실행" 메뉴가 양쪽 공통으로 들어가 일반판도 커졌다.
- +1,024B의 내역: 실제 증가분은 1KB 미만이지만 PE 파일 정렬(512B) 경계를 `.text`와 `.rsrc`가 각각 넘어 2블록이 늘었다.
  `.rdata`를 `.text`에 합치는 링커 스크립트로 512B를 더 줄일 수는 있으나 빌드 경로 3종(mingw/MSYS2/MSVC)에 공통 적용하기 어려워 보류.
- 8KB 예산 대비 여유: 가장 큰 x86 LED판(6,656B) 기준 1.3KB. CI 크기 검사는 `dist/*.exe` 전체에 적용되므로 LED판도 같은 예산을 받는다.

## 공통 기능 — 로그인 시 자동 실행 (구현됨, 양쪽 빌드)

트레이 메뉴 **"로그인 시 자동 실행"** 체크 항목. 체크 표시는 메뉴를 열 때마다 `Run` 값 존재 여부로 결정한다
(별도 상태 변수 없음 → 사용자가 레지스트리에서 직접 지워도 메뉴가 어긋나지 않는다).

```
켜기 → RegSetKeyValue(HKCU, Software\Microsoft\Windows\CurrentVersion\Run, "nShiftSpace", REG_SZ, "\"<exe 경로>\"")
끄기 → RegDeleteKeyValue(같은 값)
표시 → RegGetValue == ERROR_SUCCESS
```

### 방식 선택

| 후보 | 판단 |
|---|---|
| 시작 프로그램 폴더에 `.lnk` | ❌ `IShellLink`(COM) 필요 → ole32 링크·`CoInitialize`. 코드·import 가장 큼 |
| 시작 프로그램 폴더에 exe 복사 | ❌ 사본이 생겨 업그레이드 후 옛 버전이 뜬다 |
| 작업 스케줄러 | ❌ COM/`schtasks` 외부 프로세스. 관리자 창 대응이 필요한 사용자만 수동으로 (Wiki 방법 C) |
| **HKCU `Run` 값** | ✅ 표준 방식. Vista+ `RegGetValue`/`RegSetKeyValue`/`RegDeleteKeyValue` 3개면 키 열기·닫기 없이 끝난다 |

- 경로는 `GetModuleFileName`으로 얻어 **큰따옴표로 감싼다** (공백 경로 대비). 반환 길이를 그대로 써서 `lstrcat`/`lstrlen` import 를 피한다.
- 비용: import 3개(advapi32) + 메뉴 문자열. x64 일반판 4,608 → 5,120B (`.text` 1,024 → 1,344, `.idata` 1,172 → 1,388),
  x86 일반판 4,608 → 5,632B (`.text`·`.idata`가 각각 512B 경계를 넘음).
- 한계: `Run` 값은 일반 권한으로 실행되므로 관리자 권한 창에서의 단축키 문제는 그대로다 (작업 스케줄러 방식 안내 유지).
  winget 사용자는 `Links` 별칭으로 실행한 상태에서 켜야 업그레이드에 안전한 경로가 기록된다 (별칭이 심볼릭 링크일 때
  `GetModuleFileName`이 별칭 경로를 돌려주는지는 실측 과제). 제거 전에 끄지 않으면 `Run` 값이 남는다 (Windows는 무시하지만
  작업 관리자 시작 프로그램 탭에 흔적이 남음).

## 공통 — 메모리: 1MB는 어디서 오고, 어떻게 0.3MB(1차)·0.1MB(2차)로 줄였는가 (2026-10-17 실측)

작업 관리자의 "메모리" 열은 **사설 작업 집합**(private working set)이다. nShiftSpace가 1.0~1.7MB로 보이는 반면
같은 작성자의 nexa-coffee·NexaMemKeeper는 0.3MB로 보이는 이유를 페이지 단위로 분류해 확인했다
(`VirtualQueryEx` + `QueryWorkingSetEx`, 사설 페이지를 매핑된 모듈별로 합산).

| 항목 | nShiftSpace (트리밍 전) | nexa-coffee |
|---|---|---|
| 로드된 DLL | 47개 | 52개 (거의 같은 집합) |
| 커밋된 사설 메모리 (실제 할당) | 1,464K | 2,436K |
| 사설 작업 집합 (작업 관리자 표시) | 988~1,712K | 292~348K |
| 그중 힙·스택 등 순수 사설 영역 | 716K | 220K |
| 그중 DLL 쓰기 가능 데이터 페이지(복사-후-쓰기) | ≈1,000K (45개 DLL × 8~56K) | ≈130K |

- **우리 코드가 쓰는 메모리는 거의 없다** (`.bss` 1.6KB, 스택 수 KB). 1MB의 정체는 user32·shell32 초기화와
  한국어 IME(msctf, imkrtip, TextInputFramework 등 — 창을 만든 스레드에 자동으로 붙는다)가 건드린 각 DLL의
  `.data` 페이지와 기본 힙이다. shell32 하나가 windows.storage, combase, propsys 등 30여 개를 끌고 들어온다.
- nexa-coffee는 `SetProcessWorkingSetSize`, NexaMemKeeper는 `K32EmptyWorkingSet`을 import 한다 — 즉
  **초기화 후 작업 집합을 비워 표시 수치를 낮추는 것**이지 실제 할당(커밋)이 적은 것이 아니다.
  (커밋은 nexa-coffee가 오히려 1MB 더 크다.)

### 적용: 같은 방식으로 작업 집합 비우기

`trim_working_set()` = `SetProcessWorkingSetSize((HANDLE)-1, (SIZE_T)-1, (SIZE_T)-1)`. `GetCurrentProcess()`의
의사 핸들 값은 -1로 고정이므로 상수를 써서 import 하나를 아낀다 (LED x64 판이 `.idata` 512B 경계를 넘지 않게 하는 데 필요했다).
호출 시점은 두 곳: 초기화 직후(트레이 아이콘 추가 후)와 트레이 메뉴를 닫은 직후(메뉴 표시가 수백 KB를 끌어들인다).

| 빌드 (기동 2초 후) | 트리밍 전 사설 WS | 트리밍 후 사설 WS | 커밋 |
|---|---|---|---|
| nShiftSpace-x64 | 1,020K | **284K** | 1,428K (불변) |
| nShiftSpace-x86 | 988K | **160K** | 1,460K (불변) |
| nShiftSpace-led-x64 | 1,020K | **296K** | 1,416K (불변) |
| nShiftSpace-led-x86 | 988K | **160K** | 1,452K (불변) |

- 솔직한 평가: 이것은 **표시 수치와 상주 페이지를 줄이는 것**이지 메모리 할당을 줄이는 것이 아니다. 비워진 페이지는
  대기 목록으로 가고, 다음 Shift+Space 처리 때 필요한 몇 페이지만 소프트 폴트(마이크로초 단위)로 돌아온다.
  실제 메모리 압박 상황에서는 OS가 어차피 같은 일을 한다. 비용은 kernel32 import 1개 + 코드 수십 바이트.
- 진짜로 커밋을 줄이려면 user32/shell32를 쓰지 않아야 하는데, 트레이 아이콘·단축키·메뉴가 전부 그 둘에 있어 불가능하다.
  (키 입력 없는 서비스형 프로그램이 아닌 한 1.4MB 커밋은 Win32 트레이 앱의 바닥값이다.)

### 2차 개선 (v0.3.0): 트리밍이 풀리는 이유와 IME 비활성화

사용자 실측에서 일반판 800KB, LED판 1,000KB가 다시 보였다. 기동 직후(284K)와 다른 이유를 추적한 결과:

- 기동 직후 프로세스에는 DLL 27개만 있다. 그런데 **트레이 메뉴를 열면 `SetForegroundWindow`로 우리 창이 활성화되고,
  그 순간 한국어 IME/TSF 스택**(msctf, imkrtip, imjkapi, imetip, TextInputFramework, CoreMessaging, DUI70, OLEACC … 20여 개)이
  프로세스에 붙는다 (앞서 본 47개 모듈이 이것). 이 DLL들의 데이터 페이지와 스레드는 메뉴를 닫은 뒤 한 번 비워도,
  이후 포그라운드 창이 바뀔 때마다 TSF 쪽에서 다시 깨어나 페이지를 끌어들인다 — 메뉴 직후 한 번만 비우는 1차 방식의 한계.
- 우리 프로세스는 텍스트 입력을 받을 일이 없다 (키 합성은 포그라운드 앱의 IME가 처리). 따라서
  **`ImmDisableIME((DWORD)-1)`** 을 창 생성 전에 호출해 이 프로세스의 IME/TSF 부착을 막는다.
  실측: 기동 후 모듈 25개(msctf 미로드), IME 계열 DLL 0개.
- 트리밍 시점은 "활동 뒤 조용해지면"으로 바꿨다: `wnd_proc`가 어떤 메시지든 받으면 2초 일회성 타이머를 (재)설정하고,
  `WM_TIMER`에서 `KillTimer` + 비우기. 주기 타이머가 아니라 활동 뒤 한 번만 깨어나므로 유휴 CPU 0은 유지된다.
- import 수지: `SetTimer`/`KillTimer`/`ImmDisableIME` 3개가 늘고, 종료 경로를 `Shell_NotifyIcon(NIM_DELETE)` + `ExitProcess`로
  단순화해 `DestroyWindow`/`PostQuitMessage`/`UnregisterHotKey`를, 문자 입력이 없으므로 `TranslateMessage`를 제거 (4개 감소).
  exe 크기는 v0.2.0과 동일 (5,120 / 5,632 / 6,144 / 6,656 B).

| 빌드 | 기동 0.8초 (타이머 전) | 기동 3.8초 (트리밍 후) | 메뉴 명령 4회 직후 | 그 3초 뒤 |
|---|---|---|---|---|
| nShiftSpace-x64 | 964K | **68K** | 212K | 68K |
| nShiftSpace-led-x64 | 948K | **76K** | 200K | 76K |
| nShiftSpace-x86 | 920K | **120K** | 264K | 116K |
| nShiftSpace-led-x86 | 908K | **116K** | 260K | 120K |

(사설 작업 집합, 커밋은 1.4MB대로 불변.) 트레이 메뉴를 실제로 열었을 때의 재증가는 화면 조작 없이 재현할 수 없어
미실측이며, IME 부착이 사라진 만큼 1차보다 낮게 유지될 것으로 본다 — 릴리스 후 작업 관리자로 확인할 항목.

## 목표 2 아키텍처 (nexa-mapper)

**엔진 하나 + 텍스트 설정 파일** 구조. 설정 GUI를 만들지 않는 것이 초경량의 핵심 —
설정은 exe 옆의 `mappings.ini`를 메모장으로 편집하고, 트레이 메뉴에 "설정 열기 / 다시 읽기"만 둔다.

### 설정 파일 형식 (단순 줄 단위, 파서 최소화)

```ini
; 원본키 = 대상키   (수식키는 + 로 조합)
Shift+Space = Hangul
CapsLock    = Esc
RCtrl       = Hanja
F1          = VolumeMute
```

### 키 가로채기 방식: 매핑 종류에 따라 이원화

| 매핑 형태 | 방식 | 이유 |
|---|---|---|
| 수식키+일반키 조합 (Shift+Space 등) | `RegisterHotKey` | 코드 단순, CPU 0 |
| 단일 키 재매핑 (CapsLock→Esc 등) | `SetWindowsHookEx(WH_KEYBOARD_LL)` | RegisterHotKey는 단일 무수식 키에 부적합 |

훅은 단일 키 매핑이 하나라도 있을 때만 설치한다 (조합 매핑만 있으면 훅 없이 동작 → 메모리·지연 최소).
훅 콜백에서는 매핑 테이블(정적 배열, 최대 32개) 조회 후 `SendInput`으로 대상 키를 합성하고 원본 키는 1 반환으로 삼킨다.
합성 입력에는 `KEYEVENTF_*` + 자체 시그니처(`dwExtraInfo`)를 넣어 훅 재진입(무한루프)을 방지한다.

### 대상 키 이름 테이블

정적 `{이름, VK코드}` 배열로 해결 (Hangul, Hanja, Esc, Tab, F1–F24, VolumeUp/Down/Mute,
MediaPlayPause, A–Z, 0–9 등). 문자열 비교는 `lstrcmpiW` 사용 → CRT 불필요.

## 목표 3 아키텍처 (exe 내보내기)

**컴파일러 없이, 스텁 복사 + 설정 덧붙이기(payload append)** 방식.

```
[nexa-mapper.exe 스텁 부분] + [매직마커 "NEXA1"] + [설정 텍스트] + [설정 길이 4바이트]
```

- 엔진은 시작 시 **자기 자신의 exe 파일 끝**을 검사한다:
  - 마커가 있으면 → 내장 설정으로 동작 (트레이 메뉴는 "종료"만; 잠금 모드)
  - 마커가 없으면 → 옆의 `mappings.ini` 로드 (일반 모드)
- "실행파일 만들기" = 자기 exe를 복사하고 현재 설정을 뒤에 덧붙이는 것뿐.
  → 생성기·컴파일러·별도 스텁 파일이 전혀 필요 없고, 결과물 크기 = 엔진 크기 + 설정 몇백 바이트.
- PE 뒤에 덧붙은 데이터는 Windows 로더가 무시하므로 실행에 영향 없음
  (자기압축해제 아카이브와 같은 원리). 단, 코드서명을 하면 서명이 파일 끝에 붙으므로
  서명과 병용할 경우 마커 탐색은 "끝에서 역방향 스캔"으로 구현한다.

## 빌드

macOS에서 `brew install mingw-w64` 후 `make`. x64(PE32+)와 x86(PE32) 동시 생성.
