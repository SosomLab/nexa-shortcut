# 목표와 할 일 (ROADMAP)

nexa-shortcut 프로젝트의 **목표**와 그 목표를 이루기 위한 **할 일**을 한곳에 모은 문서.
설계 근거는 [DESIGN.md](DESIGN.md), 완료된 작업의 상세 이력은 [CHANGELOG.md](CHANGELOG.md),
패키지 배포 현황은 [PACKAGING.md](PACKAGING.md)에 있다. 이 문서는 **앞으로 할 일**에 집중한다.

- 기준일: 2026-10-17
- 표기: `[ ]` 미착수 · `[~]` 진행 중 · `[x]` 완료

---

## 1. 목표 한눈에 보기

| # | 목표 | 산출물 | 상태 |
|---|---|---|---|
| 1 | **nShiftSpace** — Shift+Space 한/영 전환 단일 기능 초경량 상주 프로그램 | `nShiftSpace-x64/x86.exe` (v0.1.0 4.5KB → 현재 5.0/5.5KB) | ✅ 완료 (v0.1.0 배포) |
| 1-a | **키보드 LED 표시판** — 아무 키나 누르는 동안 트레이 아이콘 켜짐 (Raw Input, 메뉴로 켜고 끔) | `nShiftSpace-led-x64/x86.exe` (6.0/6.5KB) | ✅ 구현 (2026-10-17, 미배포) |
| 1-b | **로그인 시 자동 실행** — 트레이 메뉴 체크로 HKCU `Run` 값 등록/삭제 (양쪽 빌드 공통) | 트레이 메뉴 항목 | ✅ 구현 (2026-10-17, 미배포) |
| 1-c | **메모리 표시 개선** — 초기화 후 작업 집합 비우기 (작업 관리자 ≈1MB → 0.3MB) | 양쪽 빌드 공통 | ✅ 구현 (2026-10-17, 미배포) |
| 2 | **nexa-mapper** — `mappings.ini`로 사용자가 정의하는 단순 키 재매핑 엔진 | `nMapper-x64/x86.exe` + `mappings.ini` | ⬜ 미착수 |
| 3 | **exe 내보내기** — 현재 매핑만 내장한 독립 exe 생성 (스텁 복사 + payload 덧붙이기) | 엔진 내 "실행파일 만들기" 기능 | ⬜ 미착수 (목표 2 선행) |
| — | **배포 채널 확보** — winget / Chocolatey 정식 등록 | 두 채널 설치 명령 동작 | ✅ 완료 (2026-07-30) |
| — | **사용자 문서 창구** — 설치·자동 실행·문제 해결 안내 | [GitHub Wiki](https://github.com/SosomLab/nexa-shortcut/wiki) 8개 페이지 | ✅ 완료 (2026-08-02) |

### 모든 목표에 공통으로 적용되는 불변 원칙

DESIGN.md의 "공통 초경량 원칙"이 곧 완료 기준의 일부다. 새 기능을 넣을 때마다 아래를 만족해야 한다.

- C + Win32 API만 사용, GUI 프레임워크·CRT 미링크 (`-nostdlib`, 진입점 `start`)
- 링크 라이브러리는 kernel32 / user32 / shell32 로 제한 (+ advapi32: 자동 실행 `Run` 값 전용)
- **exe 1개당 8KB 이내** (CI가 초과 시 빌드 실패 — [.github/workflows/build.yml](../.github/workflows/build.yml) 크기 회귀 검사)
- 설정 GUI를 만들지 않는다 (설정은 텍스트 파일 편집). 레지스트리는 사용자가 메뉴에서 켠 자동 실행 `Run` 값 외에 쓰지 않는다
- 부가 기능은 컴파일 플래그로 분리한 별도 빌드로 제공한다 (기본 빌드에는 변형 코드·리소스·import가 들어가지 않는다)
- 모든 변경은 CHANGELOG.md에 요청/분석/설계/개발 4개 절로 기록

---

## 2. 배포 채널 — 등록 완료, 남은 과제 2건

두 채널 모두 최초 등록 심사를 통과해 설치 명령이 동작한다. 상세는 [PACKAGING.md](PACKAGING.md) 참고.
**2026-08-02 재확인 결과 두 채널 모두 변동 없음** (게시 버전은 0.1.0 단일).

- [x] Chocolatey `nshiftspace` 0.1.0 모더레이션 승인 (2026-07-30 21:01)
- [x] winget `SosomLab.nShiftSpace` 0.1.0 PR 병합 —
  [microsoft/winget-pkgs#397365](https://github.com/microsoft/winget-pkgs/pull/397365) (2026-07-17 05:23)
- [x] README·PACKAGING.md의 "심사 중" 안내 제거, 상태 표 갱신
- [x] Chocolatey 자동 게시 경로 검증 — v0.1.0 태그 런에서 CI `chocolatey` 잡이 `choco push`까지 성공
- [x] 설치 후 자동 실행(시작 프로그램) 설정 절차 문서화 — winget `Links` 별칭 경로 사용,
  작업 스케줄러 최고 권한 방식 포함 ([PACKAGING.md](PACKAGING.md) "설치 후 자동 실행 설정")
- [ ] **다음 버전(v0.1.1) 태그 시 winget 자동 제출 검증** — `wingetcreate` 업데이트 PR 자동 제출은
  아직 한 번도 실행된 적이 없다 (0.1.0 최초 등록은 수동 PR이었고, CI 잡은 병합 이후 버전부터 동작)
- [ ] 자동 실행 절차를 실제 Windows PC에서 실측 검증 (특히 winget 별칭 생성에 개발자 모드가
  필요한지 여부, 방법 C 예약 작업의 승격 실행 동작)

> ⚠️ 운영 규칙: **이미 게시한 태그는 재발행 금지** (릴리스 zip의 SHA256이 매니페스트에 고정됨).
> 수정이 필요하면 새 버전으로 배포한다.

### 후보 과제 — Chocolatey 시작 프로그램 바로가기 자동 생성 (v0.1.1 검토 → 우선순위 하향)

> 2026-10-17: 프로그램 자체에 "로그인 시 자동 실행" 메뉴가 들어가 설치 스크립트 쪽 바로가기 생성의 필요성이 줄었다.

`Install-ChocolateyShortcut`으로 설치 시 `shell:startup`에 바로가기를 만들 수 있음을 확인했다
(winget에는 불가능한 Chocolatey 고유 기능). 적용 시 필요한 일:

- [ ] `chocolateyinstall.ps1`에 `--params "/Startup"` **opt-in** 분기 추가
- [ ] `chocolateyuninstall.ps1` 신설 — 제거 시 바로가기 삭제
- [ ] SYSTEM/타 사용자 컨텍스트 설치 시 프로필이 어긋나는 문제 대응 방침 결정
- [ ] 새 버전 배포 + 모더레이션 재심사 감수 (0.1.0 실적 약 27일)

판단 근거와 예시 코드는 [PACKAGING.md](PACKAGING.md) "시작 프로그램 바로가기 자동 생성" 절.

---

## 3. 다음 릴리스 v0.2.0 — LED판 · 자동 실행 메뉴 · 메모리 (구현 완료, 배포 전 과제)

2026-10-17 `feat/led-indicator` 브랜치에서 구현해 main에 병합했다. 상세는 [CHANGELOG.md](CHANGELOG.md) 10-17 항목 3건,
설계 근거는 [DESIGN.md](DESIGN.md) "목표 1 변형" · "공통 기능" · "공통 — 메모리" 절.

| 산출물 | 크기 (로컬 winlibs GCC 16.2 실측) | 사설 작업 집합 (기동 2초 후) |
|---|---|---|
| `nShiftSpace-x64.exe` / `-x86.exe` | 5,120 / 5,632 B | 284K / 160K |
| `nShiftSpace-led-x64.exe` / `-x86.exe` | 6,144 / 6,656 B | 296K / 160K |

- [x] 구현·빌드 4종·`-Wall -Wextra` 무경고·WM_COMMAND 주입 테스트(자동 실행 값 생성/삭제, LED 토글 상주 유지)
- [x] 문서 반영 — README, DESIGN, PACKAGING, DEV-ENV-WINDOWS, wiki 8개 페이지 원고
- [ ] CI(Ubuntu gcc-mingw-w64) 빌드 크기 재확인 — 로컬 툴체인과 바이트 차이가 있을 수 있음. 8KB 검사 통과 확인
- [ ] **Wiki 재발행** `./tools/publish-wiki.sh` + 제품 소개 페이지 갱신 (문서 3면 동기화)
- [ ] **LED판 포함 첫 배포(v0.2.0 후보)** — 릴리스 자산이 zip 2종 → 4종으로 늘어난다.
  winget·Chocolatey는 일반판 zip만 참조하므로 매니페스트·설치 스크립트 변경은 없다
- [ ] LED판 화면 실측 — 키를 누른 동안 아이콘이 실제로 초록으로 바뀌는지, 메뉴 토글로 꺼지는지,
  관리자 권한 창에 포커스가 있을 때도 Raw Input이 수신되는지(UIPI) 확인 후 Wiki 사용법에 반영
- [ ] 자동 실행 메뉴 실측 — winget `Links` 별칭(`nshiftspace`)으로 실행한 상태에서 켰을 때 `Run` 값에
  별칭 경로가 기록되는지(별칭이 심볼릭 링크인지 복사본인지에 따라 다름), 재로그인 후 실제로 뜨는지
- [ ] `chocolateyuninstall.ps1`에서 `Run` 값 정리 여부 결정 (메뉴로 켠 자동 실행이 제거 후 남는 문제)
- [ ] `v0.2.0` 태그 — zip 4종 첨부, winget 자동 제출(`wingetcreate`) 첫 검증(2절 과제)과 Chocolatey 재심사

---

## 4. 사용자 문서 — GitHub Wiki ✅ 발행 완료

저장소 `docs/`는 개발·배포 이력 중심이라 일반 사용자가 읽기엔 무겁다.
[GitHub Wiki](https://github.com/SosomLab/nexa-shortcut/wiki)를 **사용자용 안내 창구**로 둔다.

| 구분 | 대상 | 위치 |
|---|---|---|
| Wiki | **사용자** — 설치·자동 실행·사용법·문제 해결 | [wiki](https://github.com/SosomLab/nexa-shortcut/wiki) (정본: [docs/wiki/](wiki/)) |
| `docs/` | **메인테이너** — 설계 근거, 배포 파이프라인, 심사 이력, 변경 기록 | DESIGN·PACKAGING·ROADMAP·CHANGELOG·DEV-ENV-WINDOWS |

- [x] Wiki 배포 가능 여부 확인 — Wiki는 활성화(`has_wiki=true`)되어 있었으나 **첫 페이지가
  생성된 적이 없어** `*.wiki.git` 저장소가 존재하지 않았다. GitHub은 첫 페이지 생성 API를
  제공하지 않으므로 **웹 UI에서 최초 1회 생성이 필요**했다 (사용자가 Home 페이지 수동 생성)
- [x] Wiki 페이지 원고 작성 — [docs/wiki/](wiki/)에 8개 페이지 + 사이드바·푸터
- [x] `tools/publish-wiki.sh`로 발행 — 8개 페이지 모두 HTTP 200 확인,
  페이지 간 링크·앵커(`#방법-c--…` 등) 실제 렌더링 결과와 대조 검증
- [x] README에 Wiki 링크 추가

### 운영 방침

- Wiki의 **정본은 저장소의 [docs/wiki/](wiki/)** 다. 수정은 여기서 하고
  `./tools/publish-wiki.sh`로 발행한다.
- **Wiki에서 직접 편집하면 다음 발행 때 덮어써진다** (`_Footer.md`에 경고 표시).
- 재발행은 스크립트가 기존 `.md`를 지우고 다시 복사하는 방식이라 **삭제된 페이지도 반영**된다.

---

## 5. 목표 2 — nexa-mapper (다음 개발 대상)

설계는 [DESIGN.md "목표 2 아키텍처"](DESIGN.md)에 이미 확정되어 있다. 남은 것은 구현.
아래 순서대로 진행하면 각 단계마다 동작하는 결과물이 나온다.

### 2-1. 기반 (엔진 골격)

- [ ] `src/nMapper.c` 신설 — nShiftSpace의 골격(뮤텍스·숨은 윈도우·트레이·메시지 루프) 재사용
- [ ] 공통 코드 정리 방침 결정: 헤더 분리(`src/win32_min.h`) vs 파일 복제
      — 초경량·단순성을 해치지 않는 쪽 선택
- [ ] `Makefile` / `build.bat`에 nMapper 타깃 추가, CI 크기 검사 대상에 포함

### 2-2. 설정 파일 파서

- [ ] exe 옆 `mappings.ini` 읽기 (`CreateFile`+`ReadFile`, 최대 크기 제한 — CRT 미사용)
- [ ] 줄 단위 파서: `원본키 = 대상키`, `;` 주석, 공백 무시
- [ ] 키 이름 → VK 코드 정적 테이블 (Hangul, Hanja, Esc, Tab, F1–F24, 볼륨/미디어 키, A–Z, 0–9)
      — 비교는 `lstrcmpiW`
- [ ] 매핑 테이블 정적 배열(최대 32개), 파싱 실패 줄은 무시하되 트레이 툴팁/메시지로 알림

### 2-3. 키 가로채기 이원화

- [ ] 수식키+일반키 조합 → `RegisterHotKey` 경로 (nShiftSpace 로직 일반화)
- [ ] 단일 키 재매핑 → `SetWindowsHookEx(WH_KEYBOARD_LL)` 경로
- [ ] **훅은 단일 키 매핑이 하나 이상 있을 때만 설치** (조합 전용 설정이면 훅 없음)
- [ ] `SendInput` 합성 시 `dwExtraInfo` 시그니처로 훅 재진입(무한 루프) 차단
- [ ] 원본 키 삼키기(훅 콜백 1 반환) 동작 확인

### 2-4. 트레이 UX

- [ ] 트레이 메뉴: 설정 열기(메모장) / 다시 읽기 / 종료
- [ ] 설정 파일이 없으면 주석 달린 기본 `mappings.ini` 생성
- [ ] nMapper 전용 아이콘 생성 (`tools/make_icon.py` 확장, 1bpp 16×16 유지)

### 2-5. 검증·문서

- [ ] 수동 테스트 시나리오 문서화 (조합 매핑 / 단일 키 매핑 / 잘못된 설정 / 다시 읽기 / explorer 재시작)
- [ ] 크기 실측 후 8KB 예산 내 확인, 초과 시 감량
- [ ] README에 nMapper 사용법 절 추가, DESIGN.md의 "설계" → "구현됨"으로 갱신
- [ ] 릴리스: 태그 배포에 nMapper exe 포함 여부 결정 (별도 패키지 vs 동일 zip)

---

## 6. 목표 3 — exe 내보내기 (목표 2 완료 후)

방식은 확정: `[스텁 exe] + ["NEXA1" 마커] + [설정 텍스트] + [길이 4바이트]`.

- [ ] 시작 시 자기 exe 끝에서 **역방향 스캔**으로 마커 탐색 (코드서명 뒤따름 대비)
- [ ] 마커 있음 → 내장 설정으로 동작(잠금 모드, 트레이 메뉴는 "종료"만) / 없음 → `mappings.ini` 모드
- [ ] 트레이 메뉴 "실행파일 만들기": 자기 exe 복사 + 현재 설정 덧붙이기 (`GetModuleFileName`, `CopyFile`)
- [ ] 저장 위치 선택 방법 결정 — 파일 대화상자(comdlg32 추가 링크) vs 고정 경로 생성 (초경량 원칙과 저울질)
- [ ] 생성된 exe 실행 검증: 설정 없이 단독 동작, 크기 = 엔진 + 수백 바이트
- [ ] 백신 오탐 여부 확인 (payload append 방식 특성상 점검 필요)
- [ ] README/DESIGN 문서화

---

## 7. 상시 운영 과제 (기한 없음)

- [ ] Windows 11 / 관리자 권한 창 포커스 시 동작 한계 재확인 및 README 명시 유지
- [ ] 신규 릴리스마다 크기 회귀 검사 통과 확인 (CI 자동)
- [ ] **문서 3면 동기화** — 설치 명령·경로·버전이 바뀌면 세 곳을 함께 갱신
  1. `README.md` 2. `docs/wiki/` → `./tools/publish-wiki.sh` **재발행 필수**
  3. 제품 소개 페이지 https://sosomlab.com/apps/nexa-shortcut/
- [ ] 사용자 이슈·요청 대응 (GitHub Issues)

---

## 8. 완료된 일 (요약)

상세 이력은 [CHANGELOG.md](CHANGELOG.md).

- [x] 목표 1 구현 — `src/nShiftSpace.c`, RegisterHotKey 기반, v0.1.0 x64/x86 각 4.5KB
- [x] 키보드 LED 표시판 (2026-10-17) — 같은 소스에 `NSS_LED` 플래그, Raw Input 기반, 트레이 메뉴로
  켜고 끔(끄면 등록 해제로 비용 0), 별도 exe 6.0/6.5KB.
  빌드 경로 3종(Makefile/MSYS2/build.bat)과 CI 크기 검사에 포함
- [x] 로그인 시 자동 실행 메뉴 (2026-10-17) — HKCU `Run` 값 토글, advapi32 import 3개, 양쪽 빌드 공통.
  WM_COMMAND 주입 테스트로 4개 exe 모두 값 생성·삭제 확인
- [x] 메모리 분석·작업 집합 비우기 (2026-10-17) — 작업 관리자 표시 ≈1MB → 0.16~0.3MB. 1MB의 정체는
  user32/shell32/IME DLL의 복사-후-쓰기 데이터 페이지였고, 형제 앱(nexa-coffee 등)도 같은 트리밍으로 작게 보이는 것임을 확인
- [x] 자체 아이콘 내장 (198바이트 1bpp ICO, `tools/make_icon.py`)
- [x] 중복 실행 방지, explorer 재시작 시 트레이 아이콘 복구
- [x] 크로스 빌드(macOS mingw-w64) + Windows 빌드(MSYS2, MSVC) 경로 확보
- [x] GitHub Actions: 빌드 + 8KB 크기 회귀 검사 + `v*` 태그 시 Release 자동 첨부
- [x] v0.1.0 릴리스 및 Chocolatey / winget 패키지 명세·자동 게시 파이프라인 구성
- [x] winget(2026-07-17) · Chocolatey(2026-07-30) 최초 등록 심사 통과 — 두 채널 설치 명령 동작
- [x] 설치 후 자동 실행 설정 절차 정립 (2026-08-02) — winget `Links` 별칭 경로 사용,
  작업 스케줄러 최고 권한 방식으로 관리자 창 제약 우회
- [x] GitHub Wiki 발행 (2026-08-02) — 사용자용 8개 페이지, `tools/publish-wiki.sh`로 재발행 가능
- [x] 문서화 — README, DESIGN, CHANGELOG, PACKAGING, ROADMAP, DEV-ENV-WINDOWS, wiki/
