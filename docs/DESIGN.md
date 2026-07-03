# nexa-shortcut 설계

## 목표

1. **nShiftSpace** (Nexa ShiftSpace) — Shift+Space 한/영 전환 단일 기능. 초경량(파일 크기·메모리). ✅ 완료
2. **nexa-mapper** — 사용자가 간단한 키 매핑을 직접 설정해 쓰는 범용 매퍼.
   자주 안 쓰는 키를 다른 용도로 재매핑하는 수준의 단순한 기능. 초경량 원칙 유지.
3. **exe 내보내기** — 설정한 매핑만 담긴 독립 exe를 생성하는 기능.

## 공통 초경량 원칙

- C + Win32 API만 사용. GUI 프레임워크·CRT 미사용 (`-nostdlib`, 진입점 `start`).
- 링크 라이브러리는 kernel32 / user32 / shell32 로 제한.
- 큰 구조체는 전역(.bss)에 두어 컴파일러의 `memset` 호출 생성을 회피.
- 결과물 기준: 목표 1은 4.5KB 달성 (원본 jwShiftSpaceKey.exe 12KB 대비 1/3 수준).
- 아이콘도 초경량: 16×16 1bpp 단일 이미지 ICO(198바이트)를 스크립트(`tools/make_icon.py`)로
  생성해 리소스로 내장. 다중 해상도·트루컬러 ICO(수십 KB)는 사용하지 않는다.

## 목표 1 아키텍처 (구현됨: `src/nShiftSpace.c`)

```
start() ─ 뮤텍스(중복실행 방지)
        ─ 숨은 윈도우 생성 + RegisterHotKey(Shift+Space)
        ─ 트레이 아이콘 (Shell_NotifyIcon)
        └ 메시지 루프
             WM_HOTKEY   → keybd_event(VK_HANGUL)  # 한/영 키 합성
             WM_TRAYICON → 종료 메뉴
             TaskbarCreated → 트레이 아이콘 복구 (explorer 재시작 대응)
```

- `RegisterHotKey`는 OS가 키를 가로채므로 훅보다 단순하고 CPU 비용이 0에 가깝다.
- 한계: 관리자 권한 창에 포커스가 있으면 일반 권한 프로세스의 단축키는 동작하지 않음(원본도 동일).

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
