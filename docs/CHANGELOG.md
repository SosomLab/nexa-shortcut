# 변경기록 (CHANGELOG)

모든 변경은 이 문서에 시간 역순(최신이 위)으로 기록한다.
각 항목은 **요청 / 분석 내용 / 설계 방향 / 개발 내용 및 소스 위치**를 남긴다.

---

## 2026-07-04 00:43:18 — Chocolatey 패키지 등록 파이프라인 구성

### 요청
- Chocolatey에 제품 등록을 진행할 것.

### 분석 내용
- chocolatey.org 게시는 계정 API 키가 필요 — 자동화 가능 범위는 패키지 제작과
  태그 배포 시 자동 게시 잡 구성까지. 커뮤니티 저장소 특성상 최초 등록은 모더레이션 심사를 거침.
- 원격 다운로드형 패키지는 릴리스 zip의 SHA256 체크섬을 설치 스크립트에 명시해야 함
  → 체크섬은 릴리스가 만들어진 뒤에만 계산 가능하므로 CI에서 주입하는 구조 필요.

### 설계 방향
- 패키지 id `nshiftspace`, GitHub Release zip을 내려받아 설치하는 원격형 패키지.
- 템플릿(`__VERSION__`, `__CHECKSUM32/64__`) 방식: CI의 chocolatey 잡(windows-latest)이
  릴리스 zip을 내려받아 체크섬 계산 → 주입 → `choco pack` → `choco push`.
- `CHOCO_API_KEY` 시크릿이 없으면 pack까지만 수행하고 nupkg를 아티팩트로 업로드 (안전 기본값).
- 트레이 GUI 프로그램이므로 shim이 콘솔을 붙잡지 않도록 `.gui` 마커 생성,
  업그레이드/제거 전 실행 중 프로세스를 종료하는 `chocolateybeforemodify.ps1` 포함.

### 개발 내용 및 소스 위치
- `packaging/chocolatey/nshiftspace.nuspec` (신규) — 패키지 메타데이터
- `packaging/chocolatey/tools/chocolateyinstall.ps1` (신규) — zip 설치 + .gui 마커
- `packaging/chocolatey/tools/chocolateybeforemodify.ps1` (신규) — 프로세스 정리
- `.github/workflows/build.yml` — `chocolatey` 잡 추가 (태그 트리거, build 잡 이후 실행)
- `README.md` — "설치 (Chocolatey)" 섹션 추가

---

## 2026-07-04 00:36:10 — 제품 소개 페이지·홈페이지 링크 등록

### 요청
- 제품 소개 페이지(https://sosomlab.com/apps/nexa-shortcut/)를 문서에 등록할 것.
- SosomLab 홈페이지(https://sosomlab.com)도 함께 등록.

### 개발 내용 및 소스 위치
- `README.md` — 상단에 SosomLab 홈페이지, 제품 소개 페이지, GitHub Releases 다운로드 링크 추가

---

## 2026-07-04 00:15:14 — 릴리스 자산을 zip 압축 형태로 변경

### 요청
- nShiftSpace-x64.exe 가 nShiftSpace-x64.zip 으로 압축된 형태로 배포되도록 할 것.

### 설계 방향
- 릴리스 직전 단계에서 exe별로 개별 zip 생성 (`zip -9` 최대 압축) →
  릴리스 첨부를 `dist/*.zip`으로 변경. x86도 동일하게 nShiftSpace-x86.zip 으로 배포.
- 브라우저가 exe 직접 다운로드를 경고하는 문제도 함께 완화됨.
- CI 아티팩트(Actions 탭)는 기존대로 exe 유지 — GitHub이 아티팩트를 자동 zip 포장하므로
  이중 압축을 피함.

### 개발 내용 및 소스 위치
- `.github/workflows/build.yml` — "Package (exe별 zip 압축)" 단계 추가, Release 첨부를 zip으로 변경
- 태그 `v0.1.0` 재발행으로 검증

---

## 2026-07-04 00:11:32 — 릴리스 권한 수정 (403 해결)

### 요청
- (v0.1.0 첫 배포 검증 중 발견) 릴리스 단계가 403 "Resource not accessible by integration"으로 실패.

### 분석 내용
- 저장소 기본 GITHUB_TOKEN이 읽기 전용이라 Release 생성 API 호출이 거부됨.
  워크플로에 `permissions: contents: write` 선언이 필요.

### 개발 내용 및 소스 위치
- `.github/workflows/build.yml` — 워크플로 레벨 `permissions: contents: write` 추가
- 태그 `v0.1.0`을 수정 커밋으로 이동 후 재푸시하여 재검증

---

## 2026-07-04 00:09:17 — 자동 릴리스 검증 및 배포 방법 문서화 (v0.1.0)

### 요청
- 배포 시 실행 파일 2개(x64/x86)가 자동으로 Release 되도록 할 것.

### 분석 내용
- 릴리스 자동화는 00:03:21에 등록한 워크플로의 `softprops/action-gh-release` 단계에
  이미 포함되어 있으나(`v*` 태그 트리거) 실제 실행으로 검증된 적은 없음.

### 개발 내용 및 소스 위치
- `README.md` — "배포 (자동 릴리스)" 섹션 추가: 태그 푸시 한 줄로 배포하는 방법 안내
- 첫 배포 태그 `v0.1.0` 푸시로 전체 파이프라인 검증
  (빌드 → 크기 검사 → Release 생성 → exe 2개 첨부)

---

## 2026-07-04 00:03:21 — GitHub Actions 자동 빌드 등록

### 요청
- 프로그램 빌드를 GitHub Actions에 등록해서 진행할 수 있게 할 것.

### 분석 내용
- 기존 Makefile은 mingw-w64 크로스 컴파일 기반이라 Linux 러너(ubuntu-latest)에서
  `apt-get install gcc-mingw-w64`만으로 그대로 재사용 가능 — Windows 러너 불필요.

### 설계 방향
- 트리거: main 푸시 / PR / 수동 실행(workflow_dispatch) / `v*` 태그.
- 빌드 후 exe를 아티팩트로 업로드, `v*` 태그 푸시 시에는 GitHub Release에 exe 자동 첨부.
- 초경량 회귀 검사 단계 추가: exe가 8KB를 초과하면 빌드 실패 처리
  (의존성·CRT가 슬그머니 늘어나는 것을 CI에서 차단).

### 개발 내용 및 소스 위치
- `.github/workflows/build.yml` (신규) — 설치 → make → 크기 검사 → 아티팩트 업로드 → 릴리스

---

## 2026-07-03 23:49:12 — 아이콘 미세 조정: ㄱ 가로획 2px 확대

### 요청
- '가'의 ㄱ 가로획을 2px 키울 것.

### 설계 방향
- 가로획 4px → 6px. 외곽 1px 여백 규칙 유지를 위해 왼쪽 1px(col 2→1) +
  오른쪽 1px(세로획 col 5→6) 양방향 확장. ㄱ–ㅏ 간격은 2col→1col로 줄어듦.

### 개발 내용 및 소스 위치
- `tools/make_icon.py` — `PIXELS` 상단 8행 조정 (198바이트 동일)
- `res/nShiftSpace.ico` 재생성, exe 재빌드 (4,608바이트 동일, 2026-07-03 23:49)

---

## 2026-07-03 23:47:51 — 아이콘 미세 조정: ㄱ 가로획 축소, 외곽 1px 여백 확보

### 요청
- '가'의 ㄱ 가로 길이를 2px 줄일 것.
- 캔버스 외곽 1px에는 글자가 그려지지 않도록 할 것.

### 설계 방향
- ㄱ 가로획 6px → 4px (시작점을 오른쪽으로 이동, 세로획 위치는 유지).
- 전체 글리프를 rows/cols 1–14 범위 안으로 재배치 — 위 가장자리에 닿던 ㄱ,
  오른쪽 가장자리(col 15)에 닿던 A 다리를 안쪽으로 이동. 글자 크기는 유지.

### 개발 내용 및 소스 위치
- `tools/make_icon.py` — `PIXELS` 조정 및 여백 규칙 주석 추가 (198바이트 동일)
- `res/nShiftSpace.ico` 재생성, exe 재빌드 (4,608바이트 동일, 2026-07-03 23:47)

---

## 2026-07-03 23:45:58 — 아이콘 글자 크기 확대 ('가'·'A')

### 요청
- 아이콘의 '가'와 'A' 글자를 조금 더 크게 만들 것 (렌더링 확인 결과 A가 작아 뭉개져 보임).

### 설계 방향
- 16×16 캔버스의 여백을 줄여 두 글자를 확대. 대각 배치(가: 좌상, A: 우하)는 유지.
- '가': 세로 6px → 9px (ㄱ 가로획을 좌측 가장자리까지 6px로 연장, ㅏ 세로획 8px).
- 'A': 5×6px → 7×7px (꼭짓점에서 다리가 두 번 벌어지는 형태로 획 구분이 뚜렷해짐).

### 개발 내용 및 소스 위치
- `tools/make_icon.py` — `PIXELS` 픽셀 맵 교체 (파일 크기 198바이트 동일)
- `res/nShiftSpace.ico` 재생성, exe 재빌드 (4,608바이트 동일, 2026-07-03 23:45)

---

## 2026-07-03 23:42:19 — README에서 Windows 도구 설치 단계 제거

### 요청
- 윈도우 (도구) 설치 부분을 제거할 것. (확인 결과: 설치 단계만 제거, 빌드 명령은 유지)

### 설계 방향
- 빌드 방법 2·3에서 도구 설치 안내(MSYS2 설치, pacman 툴체인 설치, Visual Studio
  워크로드 설치)를 걷어내고, 셸 실행 → 빌드 명령만 남겨 문서를 간결하게 유지.

### 개발 내용 및 소스 위치
- `README.md` — 방법 2: MSYS2 UCRT64 셸에서 `make x64 CC64=gcc RES64=windres` 한 줄로 축약,
  방법 3: Native Tools Command Prompt에서 `build.bat` 실행으로 축약. `build.bat`은 유지.

---

## 2026-07-03 23:39:51 — winget 설치 안내 되돌림 (revert)

### 요청
- 윈도우 (빌드 도구) 설치 방법 부분을 revert 처리할 것.

### 개발 내용 및 소스 위치
- 커밋 b0c4491("docs: Windows 빌드 도구 설치 안내를 winget 기준으로 보강")을 `git revert`로 되돌림
  (이미 원격에 푸시된 커밋이므로 히스토리 재작성 대신 revert 커밋 생성).
- `README.md` — 방법 2/3의 winget 설치 명령 제거, 기존 안내(MSYS2 공식 사이트,
  Visual Studio 워크로드 설치)로 복원. 본 문서의 해당 항목(23:36:26)도 함께 제거됨.

---

## 2026-07-03 23:29:14 — Windows 네이티브 빌드 방법 추가

### 요청
- Windows에서 컴파일하는 방법을 (README 빌드) 메뉴에 추가.

### 분석 내용
- 기존 빌드 문서는 macOS/Linux 크로스 컴파일만 안내.
- Windows 네이티브 빌드 경로 2가지 검토: MSYS2/MinGW-w64(GNU 툴체인, 기존 Makefile 재사용 가능)와
  Visual Studio/MSVC(cl.exe, 별도 스크립트 필요).
- 리소스 스크립트 호환성 문제 발견: MS `rc.exe`는 .rc 안의 상대 경로를 .rc 파일 위치 기준으로,
  GNU `windres`는 실행 위치(cwd) 기준으로 해석 → 경로 표기를 통일해야 양쪽에서 빌드 가능.

### 설계 방향
- Makefile의 도구 변수(CC64/RES64 등)를 커맨드라인에서 덮어쓸 수 있는 점을 활용,
  MSYS2에서는 `make x64 CC64=gcc RES64=windres`로 동일 Makefile 재사용 (중복 빌드 스크립트 방지).
- MSVC용은 `build.bat` 신설. 초경량 원칙 동일 적용: `/NODEFAULTLIB` + `/ENTRY:start`(CRT 미링크),
  `/O1`(크기 최적화), `/GS-`(스택 쿠키 제거 — CRT 없이는 쿠키 초기화 불가), `/utf-8`(소스 인코딩).
- .rc 내부 경로를 파일명만("nShiftSpace.ico")으로 바꾸고 windres에 `--include-dir res` 부여
  → rc.exe(파일 기준)와 windres(include dir) 모두에서 해석 가능.

### 개발 내용 및 소스 위치
- `build.bat` (신규) — MSVC 네이티브 빌드 스크립트 (cl.exe 존재 검사 포함)
- `res/nShiftSpace.rc` — 아이콘 경로를 파일명만으로 변경 (rc.exe/windres 호환)
- `Makefile` — windres 호출에 `--include-dir res` 추가
- `README.md` — 빌드 섹션을 3가지 방법(크로스 컴파일 / MSYS2 / Visual Studio)으로 확장
- 크로스 빌드 회귀 확인: x64/x86 각 4,608바이트 동일 (2026-07-03 23:29)

---

## 2026-07-03 23:27:07 — 초경량 자체 아이콘 제작 및 리소스 내장

### 요청
- 파일 크기와 메모리 용량을 최소로 사용하는 아이콘 제작.
- 아이콘 디자인은 한/영 글자로 만들어진 것을 추천할 것.

### 분석 내용
- 기존에는 시스템 기본 아이콘(`IDI_APPLICATION`)을 빌려 써서 리소스 비용이 0이었으나,
  배포 시 탐색기/트레이에서 프로그램 식별이 어려움.
- 일반 아이콘 편집기가 만드는 다중 해상도·트루컬러 ICO는 수십 KB로 프로그램(4KB)보다 커짐.
- 최소 구성 분석: 16×16 단일 이미지 + 1bpp(2색 팔레트) BMP 형식이면
  ICONDIR(6) + ENTRY(16) + BITMAPINFOHEADER(40) + 팔레트(8) + XOR(64) + AND(64) = **198바이트**.

### 설계 방향
- 아이콘은 Python 스크립트가 픽셀 맵 문자열에서 바이너리를 직접 생성 (재현 가능, 편집기 불필요).
- 트레이 표시 크기(16×16)만 내장 — 큰 크기는 Windows가 확대 표시. 초경량 우선.
- AND 마스크 전부 불투명 처리로 합성 비용 제거. 팔레트는 파랑(#2563EB) + 흰색 2색.
- 디자인 시안 3종(가·A 대각 / ㅎ·A 나란히 / 한 단독)을 제시, 사용자가 **가·A 대각 배치** 선택
  — Windows IME 표시(가/A) 관례와 동일해 직관적.

### 개발 내용 및 소스 위치
- `tools/make_icon.py` (신규) — 픽셀 맵 → 198바이트 ICO 생성기
- `res/nShiftSpace.ico` (신규, 198바이트), `res/nShiftSpace.rc` (신규) — 아이콘 리소스 ID 1
- `Makefile` — windres(x64/x86) 단계 추가, ICO 자동 재생성 규칙, build/ 중간 산출물 도입
- `src/nShiftSpace.c` — 트레이 아이콘을 내장 리소스(`IDI_TRAY`=1)에서 로드하도록 변경
- `.gitignore` — build/ 추가
- 결과: exe **4,608바이트** (아이콘 리소스 섹션 +512바이트, 파일 정렬 단위)

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
