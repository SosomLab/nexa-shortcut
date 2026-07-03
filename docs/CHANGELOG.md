# 변경기록 (CHANGELOG)

모든 변경은 이 문서에 시간 역순(최신이 위)으로 기록한다.
각 항목은 **요청 / 분석 내용 / 설계 방향 / 개발 내용 및 소스 위치**를 남긴다.

---
## 2026-07-03 23:17:08 — 프로그램 명칭 변경: nexa-hangul → nShiftSpace

### 요청
- 소스 파일명을 `nShiftSpace.c`로 변경. "Nexa ShiftSpace"의 약칭(nShiftSpace)을 공식 명칭으로 사용.
- 빌드되는 실행 파일 이름도 동일하게 변경.
- 변경기록 문서를 만들어 년월일시분초와 함께 상세 기록을 남길 것 (본 문서 신설).

### 분석 내용
- 명칭이 박혀 있는 위치 조사: 소스 파일명, 소스 내부 문자열 5곳(주석 헤더, 트레이 툴팁,
  중복 실행 방지 뮤텍스명, 윈도우 클래스명/창 제목, 오류 메시지박스 제목),
  Makefile의 SRC·타깃명, README/DESIGN 문서의 프로그램명·경로 참조.

### 설계 방향
- 파일명만 바꾸지 않고 **내부 식별자(뮤텍스명, 윈도우 클래스명)까지 일괄 변경**하여
  구버전과 신버전이 동시에 실행되는 혼란을 방지 (뮤텍스명이 다르면 중복 실행 방지가 서로 안 걸림).
- git 이력 보존을 위해 `git mv`로 이름 변경.

### 개발 내용 및 소스 위치
- `src/hangul_toggle.c` → `src/nShiftSpace.c` (git mv)
  - 주석 헤더, 트레이 툴팁(`add_tray_icon`), 뮤텍스명(`start`), 윈도우 클래스명·창 제목,
    메시지박스 제목의 "nexa-hangul"을 모두 "nShiftSpace"로 변경
- `Makefile` — SRC 경로, 빌드 타깃을 `dist/nShiftSpace-x64.exe` / `dist/nShiftSpace-x86.exe`로 변경
- `README.md`, `docs/DESIGN.md` — 프로그램명·파일 경로 참조 갱신
- 클린 재빌드 검증: 두 exe 모두 4,096바이트로 기존 크기 유지 (2026-07-03 23:18)

---

## 2026-07-03 23:00:04 — 프로젝트 문서화 (README, 설계 문서)

### 요청
- 진행 내용 정리 후 설계/개발 기능 단위로 커밋하고 푸시. (커밋 c8a27ee)

### 개발 내용 및 소스 위치
- `README.md` — 프로젝트 개요, 목표 1 결과(4KB), 빌드 방법(mingw-w64 크로스 컴파일),
  시작 프로그램 등록 안내, 로드맵, MIT 라이선스 안내(무료 사용·배포 가능 명시)
- `docs/DESIGN.md` — 3대 목표 정의, 공통 초경량 원칙, 목표 1 아키텍처,
  목표 2(nexa-mapper: 설정 파일 + RegisterHotKey/저수준 훅 이원 구조),
  목표 3(스텁 복사 + 설정 덧붙이기 방식 exe 내보내기) 설계

---

## 2026-07-03 22:59:55 — 목표 1 구현: Shift+Space 한/영 전환 초경량 유틸

### 요청
- 목표 1: Shift+Space 한영전환 단일 기능의 초경량(파일 용량·메모리) 프로그램 제작.
- 목표 2: 사용자가 간단한 키 매핑을 직접 설정하는 기능 (초경량 유지).
- 목표 3: 설정한 매핑만 담긴 별도 exe를 만들어주는 기능.

### 분석 내용
- 참고 프로그램 `jwShiftSpaceKey.exe`(12,288바이트, PE32) 리버스 분석:
  - Borland Delphi 제작 (`SOFTWARE\Borland\Delphi\RTL` 문자열, MZP 헤더)
  - 임포트: `RegisterHotKey`/`UnregisterHotKey`(전역 단축키), `keybd_event`+`MapVirtualKeyA`
    (한/영 키 합성), `Shell_NotifyIconA`(트레이), `CreatePopupMenu`/`TrackPopupMenu`(메뉴),
    `RegisterClassA`/`CreateWindowExA`/`GetMessageA`(숨은 윈도우 + 메시지 루프)
  - 소형화 비결: GUI 프레임워크(VCL) 없이 Win32 API 직접 호출

### 설계 방향
- C + Win32 API로 동일 구조 재현. **CRT 미링크**(`-nostdlib` + 커스텀 진입점 `start`)로
  원본보다 작은 크기 달성. 큰 구조체는 전역(.bss)에 배치해 컴파일러의 memset 생성 회피.
- `RegisterHotKey` 방식 채택 (훅 대비 단순, 대기 CPU 0). `MOD_NOREPEAT`로 키 반복 방지.
- 부가 요소: 중복 실행 방지 뮤텍스, TaskbarCreated 메시지로 explorer 재시작 시 아이콘 복구.
- macOS에서 mingw-w64로 크로스 빌드 (x64 PE32+ / x86 PE32 동시 생성).

### 개발 내용 및 소스 위치
- `src/hangul_toggle.c` (현 `src/nShiftSpace.c`) — 프로그램 전체 (커밋 5da6cb1)
- `Makefile` — 크기 최소화 플래그(-Os -s -mwindows -nostdlib -fno-ident
  -fno-asynchronous-unwind-tables -fno-stack-protector), UNICODE 정의, 진입점 지정
- `.gitignore` — 빌드 산출물 dist/ 제외
- 결과: x64/x86 각 **4,096바이트** (원본 12KB의 1/3), 임포트 테이블 검증 완료
  (kernel32/user32/shell32만 사용)
