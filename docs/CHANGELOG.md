# 변경기록 (CHANGELOG)

모든 변경은 이 문서에 시간 역순(최신이 위)으로 기록한다.
각 항목은 **요청 / 분석 내용 / 설계 방향 / 개발 내용 및 소스 위치**를 남긴다.

---

## 2026-10-17 — 내용 정리 및 진행사항 최신화 (v0.3.0 이후)

### 요청
- 내용 정리 후 진행사항 최신화를 수행하고 commit 및 main 병합 후 push할 것.

### 분석 내용
- v0.3.0 작업은 `main`에서 직접 진행해 병합할 브랜치가 없다 (`git branch --no-merged main` 비어 있음). 작업 트리는 origin/main과 동기.
- 같은 날 두 번 릴리스하며 남은 불일치: ROADMAP 1절 1-b 행 "미배포", 2절 제목의 건수(2건 → 실제 4건)와 "2026-08-02 변동 없음"
  문구, 3절 제목이 v0.2.0만 가리킴, PACKAGING의 "구현·미배포"/"다음 릴리스부터", wiki 로드맵의 메모리 수치(0.3MB → 0.1MB),
  두 nuspec의 "0.3MB"(다음 Chocolatey 버전에 반영될 설명), DESIGN 메모리 절 제목.

### 설계 방향
- 수치는 2차 개선 실측(약 0.1MB)으로 통일하고, 릴리스 시점 표기는 "v0.2.0부터/v0.3.0" 같은 버전 기준으로 바꿔 날짜가 지나도 틀리지 않게 한다.

### 개발 내용 및 소스 위치
- `docs/ROADMAP.md` 1·2·3절 문구, `docs/PACKAGING.md` 2곳, `docs/wiki/Roadmap.md`, `docs/DESIGN.md` 절 제목,
  `packaging/chocolatey/nshiftspace.nuspec`·`packaging/chocolatey-led/nshiftspace-led.nuspec` 설명. Wiki 재발행.
- 함께 커밋: 작업 트리에 준비돼 있던 **CI `guard` 잡** — 태그 배포 시 패키지별로 같은 버전이 이미 등록(Chocolatey 피드 /
  winget-pkgs 병합·열린 PR)돼 있으면 그 패키지 게시를 건너뛰고, 직전 버전이 검수 대기 중이면 보류한다. 수동 실행 `force=true`로
  무시 가능. `chocolatey`·`chocolatey-led`·`winget` 잡이 `guard` 출력에 따라 push/submit 여부를 결정. PACKAGING.md에 조건 표 추가.
  (pyyaml 문법 검사 통과. 실제 동작은 다음 태그에서 확인)

---

## 2026-10-17 — v0.3.0 릴리스 · LED판 패키지 최초 게시 · winget CI 방식 교체

### 요청
- (이어서) 개발 완료 후 새 버전 릴리스와 패키지 게시.

### 분석 내용
- main CI(런 39039…) 통과, 크기 4종 모두 v0.2.0과 동일. `v0.3.0` 태그 런 39039940540: `build`·`chocolatey`·`chocolatey-led` 성공,
  `winget` 실패. 로그는 v0.2.0과 글자까지 같은 `Nested installer not found in zip archive: nShiftSpace-x86.exe` — URL 뒤
  `|x64`/`|x86` 지정이 zip+중첩 포터블 조합에서는 효과가 없다. `wingetcreate update`의 짝짓기 로직 한계로 판단.
- Chocolatey 피드: `nshiftspace` 0.3.0과 신규 `nshiftspace-led` 0.3.0 모두 `Submitted`. 0.2.0 winget PR #454746은 검증 통과
  (`Validation-Completed`) 상태.

### 설계 방향
- winget 제출은 **완성된 매니페스트 폴더를 올리는 `wingetcreate submit`** 으로 바꾼다. 매니페스트는 저장소의
  `TEMPLATE.*.yaml`(일반판·LED판)에 CI가 버전·SHA256·날짜를 주입해 만들고 아티팩트로도 남긴다. 사람이 수동 PR을 만들 때도
  같은 템플릿을 쓰므로 CI와 수동 경로가 동일한 산출물을 낸다.
- 이번 버전은 수동 PR: 일반판 0.3.0 "New version" PR과 LED판 "New package" PR. LED PR 본문에 같은 저장소의 별도 실행 파일이며
  두 판을 함께 설치하지 않는다는 점을 설명.

### 개발 내용 및 소스 위치
- 태그 `v0.3.0` → [Release](https://github.com/SosomLab/nexa-shortcut/releases/tag/v0.3.0) zip 4종,
  `nshiftspace.0.3.0.nupkg`·`nshiftspace-led.0.3.0.nupkg` push 성공.
- `packaging/winget/manifests/s/SosomLab/nShiftSpace/{TEMPLATE.*.yaml, 0.3.0/}`, `nShiftSpace.LED/0.3.0/` —
  SHA256: 일반 x64 `612C4DF9…2A1F` / x86 `43FF71F8…B402`, LED x64 `D5C68597…352C` / x86 `2454B772…BDCB`. 둘 다 `winget validate` 통과.
  PR [#454771](https://github.com/microsoft/winget-pkgs/pull/454771)(일반판 0.3.0), [#454772](https://github.com/microsoft/winget-pkgs/pull/454772)(LED 신규).
- `.github/workflows/build.yml` — `winget` 잡 교체: Render manifests → Upload artifact → `wingetcreate submit` ×2.
- 문서: PACKAGING(상태 표·구성 요소·이력), ROADMAP(2절·3절), wiki Home/Releasing. Wiki 재발행.

---

## 2026-10-17 — 메모리 2차 개선(IME 비활성화·유휴 타이머) · LED판 별도 패키지 · v0.3.0 준비

### 요청
- 먼저 사용 메모리 최적화: 사용자 PC에서 nShiftSpace 800KB, LED판 1,000KB로 보여 앞선 분석(284K)과 다름.
- LED판을 Chocolatey·winget에 **별도 이름으로 신규 게시**하고, 기존 nShiftSpace는 기존 이름을 유지할 것.
- 끝나면 개발 및 새 버전 릴리스.

### 분석 내용
- 재증가 원인: 기동 직후 프로세스의 DLL은 27개뿐인데 사용자 인스턴스는 47개였다. 차이는 IME/TSF 스택
  (msctf, imkrtip, imjkapi, imetip, TextInputFramework, CoreMessaging, DUI70, OLEACC …). **트레이 메뉴를 열 때
  `SetForegroundWindow`로 우리 창이 활성화되면서 붙는다.** 메뉴 직후 한 번 비워도 그 스레드들이 포그라운드 변화마다
  페이지를 다시 끌어들여 800K~1MB가 된다. (A/B 실험: `ImmDisableIME(-1)` 변형은 기동 시 msctf조차 로드하지 않음.
  4분 관찰은 실험 프로세스가 중간에 종료되어 미완.)
- 우리 프로세스는 텍스트 입력을 받지 않으므로 IME를 꺼도 기능 손실이 없다. 키 합성은 포그라운드 앱의 IME가 처리.
- 사용자 PC의 `Run\nShiftSpace` 값이 Chocolatey shim 경로로 이미 설정되어 있었고, 토글 테스트가 이를 덮어써서
  원래 값으로 복원했다 (테스트 전 상태 저장·복원 규칙으로 기록).

### 설계 방향
- `ImmDisableIME((DWORD)-1)`을 창 생성 전에 호출 (imm32 import 1개, user32가 이미 로드하는 DLL).
- 트리밍 시점을 "어떤 메시지든 받은 뒤 2초 조용하면" 일회성 타이머(`SetTimer`/`KillTimer`)로 변경. 주기 타이머가 아니므로
  유휴 CPU 0 유지. 초기화 직후·메뉴 직후의 명시적 호출은 제거 (타이머가 대신).
- import 수지 맞추기: 종료를 `NIM_DELETE` + `ExitProcess`로 단순화해 `DestroyWindow`/`PostQuitMessage`/`UnregisterHotKey` 제거,
  문자 입력이 없으므로 `TranslateMessage` 제거 → LED x64의 `.idata`가 512B 경계를 넘지 않아 크기 동일.
- LED판 패키지: Chocolatey id `nshiftspace-led`(`packaging/chocolatey-led/`, beforemodify는 `nShiftSpace-led*`만 종료),
  winget `SosomLab.nShiftSpace.LED`(별칭 `nshiftspace-led`, 템플릿 3종 + 릴리스 후 SHA256 주입). CI에 `chocolatey-led` 잡과
  winget LED update 스텝(`continue-on-error`, 최초 등록 PR 병합 전까지 실패 허용) 추가. 두 패키지 설명에 "하나만 설치" 명시.

### 개발 내용 및 소스 위치
- `src/nShiftSpace.c` — `ImmDisableIME`, `IDT_TRIM`/`TRIM_DELAY_MS`, `wnd_proc` 머리의 타이머 처리·재설정, 종료 경로 단순화,
  `TranslateMessage` 제거. `Makefile`/`build.bat`에 imm32 링크.
- 실측 (사설 WS): 기동 0.8초 ≈950K → 3.8초 68K(x64) / 76K(LED x64) / 120K(x86) / 116K(LED x86), 메뉴 명령 4회 뒤 200~264K →
  3초 뒤 다시 68~120K. 모듈 25개, IME 계열 DLL 0개. 종료 메뉴로 exit code 0. 크기 5,120 / 5,632 / 6,144 / 6,656 B (불변).
- `packaging/chocolatey-led/{nshiftspace-led.nuspec,tools/chocolateyinstall.ps1,tools/chocolateybeforemodify.ps1}` — 로컬 `choco pack` 성공.
- `packaging/winget/manifests/s/SosomLab/nShiftSpace.LED/TEMPLATE.*.yaml` — 더미 값 치환 후 `winget validate` 통과.
- `.github/workflows/build.yml` — `chocolatey-led` 잡, winget LED 스텝. pyyaml로 문법 확인.
- 문서: README(메모리·LED 패키지), DESIGN(2차 개선 절·원칙·도식), ROADMAP(v0.3.0 절·완료), PACKAGING(LED 행·설명),
  wiki Installation(패키지 이름 표)/Usage/Home/Build/Releasing/Roadmap.

---

## 2026-10-17 — v0.2.0 릴리스 및 패키지 게시

### 요청
- 새 버전 릴리스와 패키지 게시를 진행할 것.

### 분석 내용
- main 푸시 CI(런 39036302171) 통과 — 4개 exe 크기가 로컬 실측과 바이트 단위로 동일(5,120 / 5,632 / 6,144 / 6,656).
- Chocolatey nuspec 설명이 0.1.0 시절(4.5KB, "shell:startup에 바로가기") 그대로라 태그 전에 갱신했다
  (새 버전은 어차피 모더레이션을 다시 거친다).
- `v0.2.0` 태그 런(39036391096): `build`·`chocolatey` 성공, **`winget` 실패**. 로그: `wingetcreate update`가
  x64 zip을 내려받아 `Nested installer not found in zip archive: nShiftSpace-x86.exe` — URL 2개를 기존 매니페스트의
  설치 항목에 짝지을 때 아키텍처를 URL에서 읽지 못해 x64 zip을 x86 항목에 맞춘 것. 첫 자동 제출이었으므로 0.1.0 때는 드러나지 않았다.
- Chocolatey 피드 API: `nshiftspace 0.2.0` `PackageStatus=Submitted`·`PackageSubmittedStatus=Pending` — 큐 진입 확인.

### 설계 방향
- winget 0.2.0은 저장소의 매니페스트 사본(0.1.0)을 복제·갱신해 **수동 PR**로 제출 (사용자가 다른 Nexa 패키지에서도 쓰는 경로).
  로컬 `winget validate` 통과 후 포크 브랜치에 contents API로 3개 파일을 올리고 PR 생성.
- CI 재발 방지: `--urls "…x64.zip|x64" "…x86.zip|x86"` (wingetcreate의 URL 뒤 `|아키텍처` 지정). v0.3.0에서 재검증.
- Wiki는 `tools/publish-wiki.sh`로 재발행 (문서 3면 동기화 중 저장소·Wiki 두 면. 제품 소개 페이지는 별도 과제).

### 개발 내용 및 소스 위치
- 커밋 `6b468e2` — `packaging/chocolatey/nshiftspace.nuspec` 설명 갱신, README 태그 예시 v0.2.0.
- 태그 `v0.2.0` → [Release](https://github.com/SosomLab/nexa-shortcut/releases/tag/v0.2.0) zip 4종
  (x64 2,149 B · x86 2,239 B · led-x64 2,530 B · led-x86 2,645 B), `nshiftspace.0.2.0.nupkg` push 성공.
- `packaging/winget/manifests/s/SosomLab/nShiftSpace/0.2.0/` — 3종 신규 (SHA256: x64 `8A99E9D0…9623`, x86 `3D249144…64B6`,
  ReleaseDate 2026-10-17, 설명에 자동 실행 메뉴·LED판 안내). PR [microsoft/winget-pkgs#454746](https://github.com/microsoft/winget-pkgs/pull/454746).
- `.github/workflows/build.yml` — winget 잡 URL 아키텍처 지정 + 주석.
- 문서: PACKAGING(상태 표·구성 요소·진행 이력 양쪽), ROADMAP(2절 winget 검증 결과·3절 릴리스 완료), wiki Home(현재 버전 0.2.0)/Roadmap/Releasing.
- Wiki 재발행 — `tools/publish-wiki.sh`의 HTTPS 푸시가 이 PC의 Git 자격 증명(다른 GitHub 계정)으로 403. 스크립트에
  `WIKI_URL` 환경변수 덮어쓰기를 추가하고 저장소 origin과 같은 SSH 별칭(`git@kiros33.github.com:…wiki.git`)으로 발행 완료 (7개 페이지 갱신).

---

## 2026-10-17 — 내용 정리 및 진행사항 최신화 (feat/led-indicator → main 병합)

### 요청
- 내용 정리 후 진행사항 최신화를 수행하고 commit 및 main 병합 후 push할 것.

### 분석 내용
- 같은 날 세 차례(LED판 → 자동 실행 → 메모리) 문서를 덧붙이며 생긴 정합성 문제:
  - ROADMAP 2절 "배포 채널 — 남은 과제 2건" 아래에 LED 실측·자동 실행 실측·v0.2.0 배포 등 **배포 채널과 무관한
    과제 5건**이 섞여 들어갔고 제목의 건수도 맞지 않았다.
  - ROADMAP 불변 원칙의 "기본 빌드는 바이트 단위로 동일하게 유지"가 DESIGN의 완화된 표현("변형 코드·리소스를
    포함하지 않는다")과 어긋났다 — 자동 실행 메뉴가 양쪽에 공통으로 들어가 일반판도 커졌기 때문.
  - ROADMAP 1절 목표 1 행의 크기(5.0/5.5KB)가 상태(v0.1.0 배포)와 맞지 않았다 (v0.1.0은 4.5KB).
  - DESIGN 목표 1 아키텍처 도식이 "WM_TRAYICON → 종료 메뉴"로 남아 새 메뉴·WM_INPUT·작업 집합 비우기를 반영하지 못했다.
  - 메모리 개선이 1절 목표 표에 없었고, Wiki 로드맵에 "다음 릴리스에 포함"이라는 시점 안내가 없었다.

### 설계 방향
- ROADMAP에 **"3. 다음 릴리스 v0.2.0"** 절을 신설해 구현 완료 항목의 실측 수치와 배포 전 과제를 한곳에 모으고,
  이후 절을 4~8로 재배열 (목표 2 하위 단계 번호 `2-1`~`2-5`는 그대로).
- 원칙 문구는 DESIGN을 정본으로 ROADMAP을 맞춘다. 레지스트리 예외(사용자가 켠 `Run` 값)도 원칙에 명시.

### 개발 내용 및 소스 위치
- `docs/ROADMAP.md` — 3절 신설·재번호, 1절에 1-c(메모리) 행과 목표 1 크기 이력, 원칙 2줄 정정, 8절 목표 1 항목에 v0.1.0 표기
- `docs/DESIGN.md` — 목표 1 아키텍처 도식에 메뉴 항목·WM_COMMAND·[LED판] WM_INPUT·작업 집합 비우기 반영
- `docs/wiki/Roadmap.md` — v0.2.0 포함 예정 안내 추가
- 저장소 `.md` 상대 링크 전수 검사 — 모두 실재 파일

---

## 2026-10-17 — 메모리 분석 및 작업 집합 비우기

### 요청
- nexa-memkeeper, nexa-coffee 등은 사용 메모리가 훨씬 적어 보이는데 이 프로그램이 1.7MB를 차지하는 이유를
  분석하고 줄일 수 있는지 확인할 것 (작업 관리자 캡처: nShiftSpace-x64 1,720K, nexa-coffee 292K, NexaMemKeeper 268K).

### 분석 내용
- 작업 관리자 "메모리" = 사설 작업 집합. 세 프로세스의 로드 모듈·커밋·작업 집합을 비교한 결과:
  nexa-coffee는 DLL 52개(우리 47개와 거의 같은 집합)를 로드하고 **커밋은 2,436K로 우리(1,464K)보다 크다.**
  작업 집합만 작다 (WS 3.4MB vs 13.8MB).
- 두 형제 앱의 import 테이블 확인: nexa-coffee → `SetProcessWorkingSetSize`, NexaMemKeeper → `K32EmptyWorkingSet`.
  즉 **초기화 후 작업 집합을 비워 표시 수치를 낮추는 것**이며 실제 할당이 적은 것이 아니다.
- 우리 1.7MB의 정체 (`QueryWorkingSetEx`로 사설 페이지를 모듈별 분류): 힙·스택 등 순수 사설 716K +
  45개 DLL의 쓰기 가능 데이터 페이지(복사-후-쓰기) 약 1MB (ntdll/shell32 56K, windows.storage 52K, … IME 계열
  msctf/imkrtip/imjkapi/imetip/TextInputFramework 등 포함). 우리 코드 자체는 .bss 1.6KB.
- 결론: 커밋을 진짜로 줄일 여지는 없다 (트레이·단축키·메뉴가 모두 user32/shell32에 있음). 표시 수치와 상주
  페이지는 형제 앱과 같은 방법으로 줄일 수 있다.

### 설계 방향
- `trim_working_set()` — `SetProcessWorkingSetSize((HANDLE)-1, -1, -1)`. `GetCurrentProcess()` 대신 의사 핸들 상수를
  써서 import 1개 절약 (LED x64 판의 `.idata`가 512B 경계를 넘어 +512B 되는 것을 막음).
- 호출 시점: 초기화 직후(트레이 아이콘 추가 후) + 트레이 메뉴를 닫은 직후. 주기적 호출은 하지 않는다 (타이머 = CPU).
- 문서에 솔직하게 명시: 표시 수치·상주 페이지 감소이지 할당 감소가 아니며, 다음 단축키 처리 때 몇 페이지가
  소프트 폴트로 돌아온다.

### 개발 내용 및 소스 위치
- `src/nShiftSpace.c` — `trim_working_set()`, `start()` 말미와 `WM_TRAYICON` 메뉴 종료 후 호출, 파일 머리 주석.
- 실측 (기동 2초 후, 사설 WS): x64 1,020K → **284K**, x86 988K → **160K**, LED x64 → 296K, LED x86 → 160K.
  커밋은 1.4MB대로 불변. exe 크기 변동 없음 (x64 5,120 / x86 5,632 / LED 6,144 / 6,656).
- 문서: README(메모리 항목), DESIGN("메모리" 절·원칙 추가), ROADMAP(완료), wiki Usage/Build/Home.

---

## 2026-10-17 — 로그인 시 자동 실행 메뉴 (Start on login)

### 요청
- "Start on login" 기능을 추가할 것 (LED 작업과 같은 브랜치 `feat/led-indicator`에서 이어서).

### 분석 내용
- 기존에는 Wiki의 수동 절차(바로가기 / Run 키 / 작업 스케줄러) 3종을 사용자가 직접 수행해야 했고, winget의
  `Links` 별칭 경로 함정 때문에 설명이 길었다. 프로그램 안에서 한 번 체크하는 쪽이 훨씬 쉽다.
- 구현 후보: (1) 시작 프로그램 폴더 `.lnk` — COM `IShellLink`·ole32 필요, 가장 무거움. (2) 시작 폴더에 exe 복사 —
  업그레이드 후 옛 사본이 뜸. (3) 작업 스케줄러 — 외부 프로세스/COM. (4) **HKCU `Run` 값** — 표준이며 가장 가볍다.
- `Run` 값은 레지스트리 API(advapi32)가 필요해 "kernel32/user32/shell32만" 원칙에 예외가 생긴다. advapi32는
  모든 프로세스에 이미 로드되는 DLL이라 메모리 부담은 없고, import는 Vista+ 단축 API 3개
  (`RegGetValueW`/`RegSetKeyValueW`/`RegDeleteKeyValueW`)로 제한해 키 열기·닫기 코드를 없앴다
  (5개 API 버전 대비 .text 112B·.idata 132B 절약 실측).
- 크기 실측: x64 일반판 4,608 → **5,120B**, x86 일반판 4,608 → **5,632B** (x86은 `.text`·`.idata` 둘 다 512B 경계를 넘음),
  LED판 x64 **6,144B** / x86 **6,656B**. 모두 8KB 예산 안.
- "레지스트리에 아무것도 쓰지 않는다" 원칙은 **기본 상태**에 한해 유지된다. 사용자가 메뉴로 켤 때만 값 하나를 쓰고,
  끄면 지운다. 제거 전에 끄지 않으면 값이 남는 한계는 문서에 명시.

### 설계 방향
- 트레이 메뉴 "로그인 시 자동 실행(&S)" 체크 항목. **체크 표시는 메뉴를 열 때마다 `Run` 값 존재 여부로 결정**
  (상태 변수 없음 → 사용자가 레지스트리에서 직접 지워도 어긋나지 않음).
- 값은 `GetModuleFileNameW` 경로를 큰따옴표로 감싼 REG_SZ. 반환 길이를 그대로 써서 `lstrcat`/`lstrlen` import 회피.
- 양쪽 빌드(일반·LED) 공통. 메뉴 순서: 자동 실행 / (LED 표시) / 구분선 / 종료.
- winget 사용자는 `nshiftspace` 별칭으로 실행한 상태에서 켜야 `Links` 경로가 기록된다 — Wiki에 채널별 안내 표 추가.
  관리자 권한 창 대응은 여전히 작업 스케줄러(방법 C) 안내 유지.

### 개발 내용 및 소스 위치
- `src/nShiftSpace.c` — `IDM_AUTOSTART`, `g_run_cmd[MAX_PATH+3]`(.bss), `RUN_KEY`/`RUN_VAL`, `autostart_enabled()`,
  `set_autostart()`, 트레이 메뉴 항목·구분선, `WM_COMMAND` 분기.
- `Makefile` `LIBS += -ladvapi32`, `build.bat` 두 링크 모두 `advapi32.lib` 추가.
- 문서: README(기능·자동 실행 절), DESIGN(공통 기능 절·원칙 예외·크기 갱신), ROADMAP(1-b 행·실측 과제·Chocolatey 바로가기
  과제 하향), PACKAGING, DEV-ENV-WINDOWS, wiki Autostart(최상단 "가장 쉬운 방법" 절·해제·선택표)/Usage(트레이 메뉴 표·무설정
  문구 수정)/Home/Installation/Build/Roadmap. **Wiki 재발행 필요** (`tools/publish-wiki.sh`, 미실행).
- 검증 — 화면 조작 없이 `WM_COMMAND`(IDM_AUTOSTART=12, IDM_LED=11)를 창에 PostMessage로 주입:
  4개 exe 모두 `Run\nShiftSpace` 값이 `"<exe 경로>"`로 생성되고 다시 삭제됨, LED 토글 후에도 상주 유지, 테스트 후 값 없음 확인.
  실제 재로그인 후 기동과 winget 별칭 경로 기록 여부는 미실측 (ROADMAP 과제).

---

## 2026-10-17 — 키보드 LED 표시판 (별도 빌드, 메뉴로 켜고 끔)

### 요청
- 브랜치를 추가하고 작업할 것 (`feat/led-indicator`).
- 트레이 아이콘 영역에 **어떤 키든 눌리면 LED를 켜고 떼면 끄는** 단순한 기능을 넣을 것.
- 최종 exe 크기·메모리 최소화 규칙을 점검하고, 필요하면 LED 버전과 일반 버전을 따로 빌드할 것.
- (추가) LED를 끄면 성능 영향이 더 줄어드는지 — 그렇다면 **켜고 끄는 기능**을 넣을 것.

### 분석 내용
- "어떤 키든"을 감지하려면 `RegisterHotKey`로는 불가능하고 키 입력 전체를 받아야 한다. 후보 3가지:
  - `WH_KEYBOARD_LL` 훅: 모든 키 입력이 우리 콜백을 **동기적으로** 거쳐 시스템 전체 입력 지연이 생기고,
    콜백 안에서 `Shell_NotifyIcon`(explorer 동기 호출)을 부르면 훅 타임아웃으로 훅이 조용히 제거될 위험.
    Wiki가 내세우던 "훅이 아니다"라는 장점도 잃는다.
  - 타이머 폴링: 유휴 CPU 0 원칙 위반.
  - **Raw Input(`RIDEV_INPUTSINK`)**: 키 make/break가 `WM_INPUT` 메시지로 비동기 전달. 다른 프로세스 입력 경로에
    개입하지 않고 훅도 아니다. 채택.
- 크기 규칙 점검 — 기능 추가분을 실측한 결과 **+1,024바이트** (4,608 → 5,632). 실제 증가분은 1KB 미만이지만
  PE 파일 정렬(512B) 경계를 `.text`와 `.rsrc`가 각각 넘었다. 8KB 예산 안이나, 한 exe에 런타임 토글로 넣으면
  LED를 안 쓰는 사용자도 이 증가분과 켜짐 아이콘을 떠안는다.
- 메모리 — 네 빌드 모두 기동 직후 사설 메모리 ≈1.4MB로 차이 없음 (로드되는 DLL이 지배).
- LED를 끄면: Raw Input 등록을 `RIDEV_REMOVE`로 해제할 수 있어 `WM_INPUT`이 전혀 오지 않는다.
  즉 **꺼진 LED판의 키 입력당 비용은 일반판과 동일하게 0**이다. 켜고 끄는 기능을 넣을 가치가 있다.
- 로컬 PC에 C 툴체인이 없어 포터블 winlibs GCC 16.2(mingw-w64, CI와 같은 GCC+binutils 계열)를 스크래치
  폴더에 받아 빌드·측정했다. 기존 소스로 빌드한 기준 exe와 새 일반판 exe는 PE 타임스탬프·체크섬 4바이트만
  다르고 나머지는 동일 — **일반판 바이트 동일 유지 확인**.

### 설계 방향
- 같은 소스(`src/nShiftSpace.c`)에서 **컴파일 플래그 `NSS_LED`로 분리**한 별도 빌드. 일반판에는 LED 코드·import·
  아이콘이 전혀 들어가지 않는다. 이 방침을 DESIGN "공통 초경량 원칙"에 추가.
- LED판 안의 런타임 토글: 트레이 메뉴 **"키보드 LED 표시"** 체크 항목. 끄면 Raw Input 등록 해제 + 아이콘 꺼짐.
  상태는 저장하지 않는다 (레지스트리·설정 파일 미사용 원칙) — 시작 시 항상 켜짐. 상시 꺼둘 사용자는 일반판.
- 셸 호출 최소화: 상태가 실제로 바뀔 때만 `NIM_MODIFY` (키 반복 입력은 호출 없음). 여러 키 동시 입력은 세지 않는다
  (어느 키든 떼면 꺼짐 — 요구 그대로이며 상태 꼬임이 없다).
- 켜짐 아이콘은 픽셀 맵 동일·바탕색만 초록(#22C55E)인 198바이트 ICO를 `tools/make_icon.py`의 변형(`on`)으로 생성.
  `.rc`의 `#ifdef NSS_LED`로 LED판에만 포함.
- 릴리스 자산은 exe별 zip이므로 2종 → 4종이 된다. winget·Chocolatey는 일반판 zip만 참조해 패키징 변경 없음.

### 개발 내용 및 소스 위치
- `src/nShiftSpace.c` — `#ifdef NSS_LED` 블록: `g_icon_off/on`, `g_lit`, `g_led_enabled`, `g_rid`, `g_raw`(.bss),
  `set_led()`(상태 변화 시만 `NIM_MODIFY`), `set_led_enabled()`(`RIDEV_INPUTSINK`/`RIDEV_REMOVE` 전환),
  `WM_INPUT` 처리(`GetRawInputData` → `RI_KEY_BREAK`), 트레이 메뉴 `IDM_LED` 체크 항목 + 구분선,
  `TaskbarCreated` 재추가 시 꺼짐으로 초기화. 비-LED 경로의 코드는 텍스트 그대로 유지.
- `res/nShiftSpace.rc` — `#ifdef NSS_LED` 아래 `2 ICON "nShiftSpace-on.ico"`.
- `res/nShiftSpace-on.ico` (신규, 198바이트), `tools/make_icon.py` — `VARIANTS` {off: 파랑, on: 초록}, 두 번째 인자로
  변형 선택. 기본 아이콘 출력은 바이트 동일 (재생성 후 git diff 없음 확인).
- `Makefile` — `led`, `led-x64`, `led-x86` 타깃, `LEDDEF := -DNSS_LED`를 gcc·windres 양쪽에 전달,
  `build/rsrc-led-*.o`, `dist/nShiftSpace-led-*.exe`. `all`은 4개 모두.
- `build.bat` — MSVC: `rc /d NSS_LED` + `cl /DNSS_LED`로 `dist\nShiftSpace-led-x64.exe` 추가 생성.
- `.github/workflows/build.yml` — 변경 없음: `make`가 4개를 만들고, 8KB 검사·아티팩트·zip이 `dist/*.exe` 전체에 적용됨.
- 문서: README, DESIGN(변형 아키텍처·비용 실측·토글의 성능 의미), ROADMAP(1-a 행, 배포·실측 과제), DEV-ENV-WINDOWS,
  wiki Build/Usage/Installation/Home/Releasing/Roadmap. **Wiki는 `tools/publish-wiki.sh`로 재발행 필요** (미실행).
- 검증 — 4개 exe 빌드(-Wall -Wextra 경고 없음), 크기 4,608/5,632, 기동·상주·오류 대화상자 없음·메모리 측정.
  **키를 누를 때 아이콘이 실제로 바뀌는 화면 확인과 메뉴 토글은 미실측** (세션에서 화면 자동 조작을 하지 않기로 함)
  → ROADMAP 과제로 등록. 수동 확인: `dist\nShiftSpace-led-x64.exe` 실행 후 아무 키나 누르고 있으면 트레이 아이콘이
  초록, 떼면 파랑. 우클릭 메뉴에서 "키보드 LED 표시" 체크 해제 시 반응 없음.

---

## 2026-08-02 — 문서 구조 정리 및 진행사항 최신화

### 요청
- 내용 정리 후 진행사항 최신화를 수행하고 commit 및 main 병합 후 push할 것.

### 분석 내용
- 작업 트리는 이미 clean, 브랜치는 `main`, `origin/main`과 0/0 동기 상태 —
  앞선 두 커밋(`55688fd`, `9c67156`)이 이미 main에 반영·푸시되어 있었다.
  **별도 작업 브랜치를 만들지 않고 main에서 직접 작업했으므로 병합할 대상이 없다.**
- 이번 세션에서 문서를 여러 차례 덧붙이며 생긴 정합성 문제를 점검한 결과:
  - **ROADMAP 절 번호 충돌** — Wiki 절을 `2-1`로 넣었는데, 목표 2의 하위 단계도
    `### 2-1. 기반 (엔진 골격)`이라 같은 번호가 문서 안에 두 번 등장했다.
  - 절 번호가 1·2·2-1·3·4·5·6으로 어긋나 있었다.
  - 2절 제목이 "남은 검증 1건"이지만 실제 미완 항목은 2건(winget 자동 제출 검증,
    자동 실행 절차 Windows 실측)이었다.
  - 1절 목표 표와 7절 완료 목록에 Wiki·자동 실행 문서화 성과가 빠져 있었다.
  - PACKAGING.md의 "설치 안내 (사용자용)" 절이 Wiki와 내용을 중복하게 되었는데,
    **어느 쪽이 정본인지 표시가 없어** 한쪽만 고칠 위험이 있었다.
  - 상시 운영 과제에 "제품 소개 페이지와 README 동기화"만 있고 **Wiki 재발행이 빠져** 있었다.

### 설계 방향
- 문서 3면(README / Wiki / 제품 소개 페이지)의 **동기화 대상을 한 항목으로 묶어** 명시한다.
  설치 명령·경로가 바뀌는 릴리스에서 Wiki 재발행 누락이 가장 실수하기 쉬운 지점이다.
- 중복이 불가피한 절에는 **정본 표시와 동반 수정 지시**를 달아 드리프트를 막는다.
- 절 번호는 최상위 흐름(배포 채널 → 사용자 문서 → 목표 2 → 목표 3 → 상시 → 완료)에 맞춰
  1~7로 정리하고, 목표 2의 하위 단계 번호(`2-1`~`2-5`)는 "목표 2의 n단계"라는 의미가
  유지되므로 그대로 둔다.

### 개발 내용 및 소스 위치
- `docs/ROADMAP.md`
  - 절 번호 재정리: Wiki 절을 `2-1` → **`3`** 으로 승격(하위 단계 번호와의 충돌 해소),
    이후 절을 4·5·6·7로 재배열
  - 2절 제목 "남은 검증 1건" → **"남은 과제 2건"** 으로 정정
  - 1절 목표 표에 **사용자 문서 창구(Wiki)** 행 추가
  - 3절에 **Wiki / `docs/` 역할 분담 표** 추가 (사용자용 vs 메인테이너용)
  - 3절 운영 방침에 재발행 시 삭제된 페이지도 반영된다는 동작 명시
  - 6절 상시 과제를 **"문서 3면 동기화"** 로 통합 — README · `docs/wiki/`(재발행 필수) ·
    제품 소개 페이지
  - 7절 완료 목록에 자동 실행 절차 정립·Wiki 발행 추가, 문서화 항목에 ROADMAP·wiki/ 반영
- `docs/PACKAGING.md` — "설치 안내 (사용자용)" 절 머리에 **Wiki 안내 및 정본 표시** 추가
  (사용자에게는 Wiki를 안내하고, 이 절을 고치면 `docs/wiki/` 원고도 함께 고쳐 재발행)
- 링크 검증: 저장소 `.md`의 상대 링크 전수 검사 및 `docs/wiki/` 내부 페이지 링크가
  실재 페이지를 가리키는지 검사 — 모두 정상

---

## 2026-08-02 — 배포 상태 재확인 · 자동 실행 안내 신설 · Wiki 원고 작성

### 요청
- winget, choco의 배포 상태를 확인하고 변경에 따라 문서를 최신화할 것.
- Wiki 배포가 가능한지 확인하고 Wiki 작성 및 배포를 진행할 것.
- winget 설치 시 바로가기를 만들고 자동 실행되도록 설정하는 방법이 쉽지 않으니
  관련 안내를 순서대로 자세히 정리해 문서에 반영할 것.
- choco 배치 시 `shell:startup` 폴더(사용자 시작프로그램)에 단축키가 만들어지도록 할 수 있는지 확인.

### 분석 내용

**1) 배포 상태 재확인 — 두 채널 모두 변동 없음**

- Chocolatey: 커뮤니티 피드 API 조회 결과 `IsApproved=true`, `PackageStatus=Approved`,
  `IsLatestVersion=true`, `PackageTestResultStatus=Passing`, `PackageValidationResultStatus=Passing`.
  게시 버전은 0.1.0 단일, 누적 다운로드 11로 7/30 점검 시점과 동일.
- winget: PR #397365 `merged=true`(2026-07-16T20:23:39Z UTC), 라벨 5종 유지.
  winget-pkgs master의 `manifests/s/SosomLab/nShiftSpace/`에 버전 디렉터리는 `0.1.0` 하나뿐이고
  매니페스트 3종 존재. 게시본에서 `PortableCommandAlias: nshiftspace`,
  `NestedInstallerType: portable` 재확인.
- v0.1.0 이후 새 태그·릴리스가 없으므로 **winget 자동 제출(wingetcreate) 검증은 여전히 미실행 상태**.

**2) winget 자동 실행 설정이 어려운 진짜 원인 — 경로 안정성**

기존 README는 시작 프로그램 경로로 `…\WinGet\Packages\SosomLab.nShiftSpace_*\nShiftSpace-x64.exe`를
안내했는데, 이 **Packages 경로는 업그레이드 시 폴더가 재생성되며 바로가기가 끊긴다.**
포터블 패키지는 `…\WinGet\Links\nshiftspace.exe`에 **별칭**이 생성되고 이 경로는 버전에
무관하게 유지되므로, 시작 프로그램은 반드시 이쪽을 가리켜야 한다.
탐색기로 설치 폴더를 뒤지면 자연스럽게 Packages 쪽 실제 exe를 집게 되는 것이
"쉽지 않다"고 느껴지는 핵심 원인이다.

부수 논점 — winget의 포터블 별칭은 심볼릭 링크로 만들어지므로 **개발자 모드가 꺼져 있으면
생성에 실패**할 수 있다. 이 경우 대비 경로(Packages 실제 exe 탐색)와 그 한계를 함께 안내해야 한다.

또한 README에 이미 기록된 제약 — **관리자 권한 창 포커스 시 단축키 미동작**(UIPI) — 은
시작 프로그램 폴더·Run 키로는 해결할 수 없고, **작업 스케줄러의 최고 권한 실행만이 해법**이다.
자동 실행 안내와 이 제약은 같은 자리에서 다뤄야 사용자가 연결지어 이해할 수 있다.

**3) Chocolatey의 `shell:startup` 바로가기 자동 생성 — 가능하나 즉시 적용은 부적절**

`Install-ChocolateyShortcut` 헬퍼로 `chocolateyinstall.ps1`에서 사용자 시작 프로그램 폴더에
`.lnk` 생성이 가능하다(winget에는 대응 수단 없음 — Chocolatey 고유 장점).
choco의 `tools` 경로에는 버전이 들어가지 않아 업그레이드에도 바로가기가 유지되는 궁합도 좋다.
다만 즉시 적용하지 않을 이유가 넷 있다.
- `choco install`은 관리자 권한 실행이라 **SYSTEM/타 사용자 컨텍스트**에서는 엉뚱한 프로필에 생성됨
- 요청하지 않은 자동 실행 등록은 침습적 — `--params "/Startup"` **opt-in**이 Chocolatey 관례
- 제거 시 바로가기 삭제를 위해 `chocolateyuninstall.ps1` 신설 필요 (현재 없음)
- 패키지 스크립트 변경 = 새 버전 배포 + 모더레이션 재심사 (0.1.0 실적 약 27일)

**4) Wiki 배포 가능 여부**

- 저장소 설정상 Wiki는 **활성화되어 있다** (`has_wiki=true`, public).
- 그러나 `https://github.com/SosomLab/nexa-shortcut.wiki.git`은 인증 여부와 무관하게
  `Repository not found` — **첫 페이지가 생성된 적이 없어 wiki 저장소 자체가 미생성**이다.
- GitHub REST API에는 Wiki 페이지 생성 엔드포인트가 없다. 즉 **웹 UI에서 최초 1회 페이지를
  저장하기 전까지는 어떤 자동화로도 푸시할 수 없다.** 이후부터는 git push로 완전 자동화 가능.

### 설계 방향

- **문서 역할 분리**: 저장소 `docs/`는 개발·배포 이력 중심(메인테이너용), **Wiki는 사용자용 안내**.
  Wiki 원고는 `docs/wiki/`에 두어 **버전 관리하고**, 스크립트로 wiki 저장소에 발행한다
  (Wiki에서 직접 편집하면 덮어써진다는 경고를 `_Footer.md`에 명시).
- **자동 실행 안내는 "경로 확정 → 등록 → 확인 → 해제" 4단계**로 구성한다. 등록은 목적에 따라
  A(바로가기)·B(Run 키)·C(작업 스케줄러) 중 하나만 고르게 하고, C에는 관리자 권한 창 대응이라는
  **선택 이유**를 명시한다. 1단계를 `Test-Path` 검사로 시작해 사용자가 자기 PC의 실제 상태를
  스스로 확정하게 만든다 (환경별 분기를 글로 나열하는 대신).
- **Chocolatey 자동 바로가기는 문서화하되 적용하지 않는다.** 가능하다는 사실·예시 코드·
  미적용 사유를 PACKAGING.md에 남기고 ROADMAP에 v0.1.1 후보 과제로 올려 판단을 보류한다.
  (패키지 변경은 재심사 27일을 유발하므로 단독으로 결정할 일이 아님)
- 검증되지 않은 부분은 **검증되지 않았다고 명시**한다 — 자동 실행 절차는 macOS에서 작성해
  Windows 실측을 거치지 않았으므로 ROADMAP에 실측 검증 과제를 남긴다.

### 개발 내용 및 소스 위치

- `docs/PACKAGING.md`
  - 한눈에 보기 표 기준일 2026-08-02로 갱신, "게시 버전은 0.1.0 단일" 명시
  - **"설치 후 자동 실행 설정" 절 신설** — 경로 안정성 비교표, 1단계 경로 확정(winget 별칭 /
    개발자 모드 미설정 시 대응 / Chocolatey), 2단계 방법 A·B·C, 3단계 확인, 해제, 업그레이드 주의
  - Chocolatey 절에 **"시작 프로그램 바로가기 자동 생성 — 가능하나 미적용"** 소절 신설
    (예시 코드 + 미적용 사유 5항목 표)
  - 양 채널 진행 이력에 2026-08-02 재확인 행 추가
- `docs/ROADMAP.md` — 기준일 갱신, 배포 채널 절에 재확인 결과·자동 실행 문서화 완료 반영,
  실측 검증 과제 추가, **"후보 과제 — Chocolatey 시작 프로그램 바로가기 자동 생성"** 신설,
  **"2-1. 사용자 문서 — GitHub Wiki"** 절 신설 (첫 페이지 수동 생성 필요 사실 포함)
- `docs/wiki/` **신설** — Wiki 원고 8개 페이지 + 사이드바·푸터
  - `Home.md` 30초 설치·문서 색인 / `Installation.md` 3채널 설치·비교표
  - `Autostart.md` 자동 실행 4단계 (winget 경로 함정, 방법 A·B·C, 해제, 업그레이드 주의)
  - `Usage.md` 동작 방식·제약 3종 / `Troubleshooting.md` 설치·실행·단축키·자동 실행 증상별 대응
  - `Build.md` 빌드 3방법·검증 / `Releasing.md` 태그 배포 자동화·운영 규칙
  - `Roadmap.md` 목표 2·3 요약 / `_Sidebar.md`, `_Footer.md`
- `tools/publish-wiki.sh` **신설** — `docs/wiki/*.md`를 wiki 저장소에 발행.
  wiki 미초기화 시 첫 페이지 생성 URL을 안내하고 종료한다 (실행해 동작 확인 완료)
- `README.md` — 상단에 Wiki 링크 추가, **"시작 프로그램 등록" 절 전면 개정**
  (Packages 경로 → `Links` 별칭 경로로 정정, PowerShell 등록 예시, 오른쪽 버튼 드래그 안내,
  관리자 권한 실행 팁, PACKAGING.md 상세 절 링크)

### 후속 — Wiki 발행 완료

사용자가 웹 UI에서 Home 페이지를 수동 생성(`17741d2 Initial Home page`)해
wiki 저장소가 초기화되었고, `./tools/publish-wiki.sh` 실행으로 8개 페이지를 발행했다.

- 발행 결과: https://github.com/SosomLab/nexa-shortcut/wiki — 8개 페이지 모두 HTTP 200 확인
- 앵커 링크 검증: 렌더링된 페이지의 실제 `user-content-*` 앵커 ID를 추출해 문서에 적은
  링크와 대조 확인. 한글 제목 + `—`/`★` 같은 기호가 섞인 앵커
  (`#방법-c--작업-스케줄러--관리자-창에서도-단축키가-먹게-하려면`)까지 정상 동작.
  `Installation#winget`/`#chocolatey`/`#zip-직접-다운로드`,
  `PACKAGING.md#설치-후-자동-실행-설정`도 모두 유효.
- 운영 방침 확정: **정본은 `docs/wiki/`**, Wiki 직접 편집분은 재발행 시 덮어써진다.

---

## 2026-07-30 22:48:36 — winget·Chocolatey 설치 안내 문서화

### 요청
- winget에 더불어 Chocolatey에 대한 설치 안내도 문서에 반영할 것.

### 분석 내용
- 기존 문서는 설치 **명령 한 줄씩**만 있고 사전 준비·실행 방법·업그레이드/제거·설치 경로가 없었다.
- 채널 간 실제 동작 차이 확인:
  - 실행 별칭이 다름 — winget은 매니페스트의 `PortableCommandAlias: nshiftspace`,
    Chocolatey는 tools 폴더 exe 이름 그대로의 shim(`nShiftSpace-x64`).
  - 권한이 다름 — winget은 일반 사용자, Chocolatey는 관리자 권한 PowerShell.
  - **실행 중 프로세스 처리가 다름** — Chocolatey는 `chocolateybeforemodify.ps1`이 종료해 주지만
    winget에는 대응 스크립트가 없어 사용자가 직접 종료해야 파일 잠김을 피할 수 있다.
  - 설치 경로가 다르고(포터블 패키지 vs choco lib), 업그레이드 시 경로가 바뀔 수 있어
    시작 프로그램 바로가기가 끊길 수 있다.

### 설계 방향
- README에는 **바로 복사해 쓰는 명령 위주**(사전 준비 → 설치 → 실행 → 업그레이드 → 제거)로 채널별 분리 기술,
  PACKAGING.md에는 **대조표 + 채널별 동작 차이 + 문제 해결**까지 담아 역할을 나눈다.
- 시작 프로그램 등록 절에 채널별 exe 경로 표를 넣고, 경로 고정이 필요하면 zip 직접 다운로드를 권장.

### 개발 내용 및 소스 위치
- `README.md` — 설치 절을 winget / Chocolatey 소절로 분리, Chocolatey 자체 설치 명령 포함,
  실행 별칭·업그레이드·제거·설치 경로 명시, 제거 전 종료 주의 추가.
  시작 프로그램 등록 절에 채널별 exe 경로 표 추가
- `docs/PACKAGING.md` — "설치 안내 (사용자용)" 절 신설: 명령 대조표(사전 준비·권한·설치·실행·
  업그레이드·제거·경로·아키텍처), 채널별 동작 차이 3항목, 문제 해결 표 5건

---

## 2026-07-30 22:46:17 — Chocolatey 상태 재점검 및 자동 게시 경로 검증 기록 정정

### 요청
- Chocolatey 진행 상태를 점검할 것.
- 다음 `v*` 태그 푸시 시 Chocolatey 배포가 자동으로 되도록 설정된 상태인지 확인할 것.
- 내용 정리 후 진행사항을 최신화하고 커밋·병합·푸시할 것.

### 분석 내용
- Chocolatey 상태(2026-07-30 22:40 점검): 패키지 페이지에 "approved by moderator flcdrg on 30 Jul 2026" 표시,
  피드 API `IsApproved=true`/`PackageStatus=Approved`, 자동 검사 `PackageTestResultStatus=Passing`·
  `PackageValidationResultStatus=Passing`, 커뮤니티 검색 피드 노출 확인. 누적 다운로드 11.
- 자동 배포 설정 점검: `chocolatey` 잡은 `if: startsWith(github.ref, 'refs/tags/v')` + `needs: build`,
  시크릿 `CHOCO_API_KEY` 등록됨(2026-07-03), 템플릿 플레이스홀더(`__VERSION__`/`__CHECKSUM32__`/
  `__CHECKSUM64__`)가 저장소 파일에 그대로 보존됨(치환은 러너에서만) → 반복 배포에 안전.
- **직전 기록 정정**: ROADMAP/PACKAGING에 "`choco push` 자동 경로가 아직 한 번도 실행된 적 없다"고
  적었으나 사실과 다름. v0.1.0 태그 런(28671030609)에서 chocolatey 잡의 전 스텝(`choco pack` → `choco push`)이
  성공했음을 확인. 수동이었던 것은 winget 최초 등록 PR뿐이다.
- 다만 게시(push)가 자동일 뿐 **승인은 별개**로, 새 버전도 모더레이션 큐를 다시 거친다
  (0.1.0 기준 제출→승인 약 27일).

### 설계 방향
- 문서에 "자동인 것(게시)"과 "자동이 아닌 것(승인)"을 분리해 명시 — 다음 릴리스 때 승인 지연을
  장애로 오인하지 않도록 소요 실적(27일)을 함께 남긴다.
- 미검증 항목은 winget `wingetcreate` 자동 제출 하나로 좁힌다.

### 개발 내용 및 소스 위치
- `docs/PACKAGING.md` — Chocolatey CI 잡 설명에 "v0.1.0에서 자동 게시 경로 검증 완료 + 승인은 별개"
  추가, 진행 이력에 2026-07-30 22:40 재점검 항목 추가
- `docs/ROADMAP.md` — Chocolatey 자동 게시 검증을 완료 항목으로 전환, 남은 미검증 항목을
  winget 자동 제출 1건으로 축소

---

## 2026-07-30 21:35:12 — 패키지 등록 완료 반영 (winget 병합, Chocolatey 승인)

### 요청
- 내용 정리 후 진행사항을 최신화하고 커밋·병합·푸시할 것.

### 분석 내용
- 실제 상태 재확인 결과 **두 채널 모두 최초 등록 심사를 통과**했다 (직전 기록 2026-07-05 기준은 모두 대기 상태였음).
  - winget: PR microsoft/winget-pkgs#397365 **병합** (2026-07-17 05:23 KST). 라벨에
    `Moderator-Approved`·`Publish-Pipeline-Succeeded` 추가, winget-pkgs master에 매니페스트 존재(HTTP 200) 확인.
  - Chocolatey: 커뮤니티 피드 OData 조회에서 `IsApproved=true`, `PackageStatus=Approved`,
    승인 시각 2026-07-30 21:01 KST, 누적 다운로드 10.
- CI 자동 갱신 경로(`choco push` 재게시 / `wingetcreate` 업데이트 PR)는 **아직 한 번도 실행된 적이 없음** —
  v0.1.0은 양쪽 모두 수동 최초 등록이었고, CI 잡은 `v*` 태그 전용이라 다음 버전에서 처음 동작한다.

### 설계 방향
- "심사 중" 전제로 쓰인 안내 문구를 문서 전반에서 제거하되, 상태 확인 방법 절은 유지
  (다음 버전 심사·문제 발생 시 다시 쓰이는 절차이므로).
- 승인으로 끝난 게 아니라 **미검증 항목(자동 갱신 경로)** 이 남았다는 점을 ROADMAP에 명시해
  다음 릴리스 때 확인하도록 남긴다.

### 개발 내용 및 소스 위치
- `docs/PACKAGING.md` — 상태 표를 2026-07-30 기준 "승인/등록 완료"로 갱신, 양 채널 진행 이력에
  병합·승인 항목 추가, winget CI 잡 설명을 "다음 버전부터 동작 + 첫 자동 제출 검증 필요"로 수정
- `README.md` — 설치 절의 "심사 진행 중" 안내를 등록 완료 안내로 교체
- `docs/ROADMAP.md` — 2절 제목을 "배포 채널 — 등록 완료, 남은 검증 1건"으로 바꾸고 완료 항목 체크,
  목표 요약 표의 배포 채널 상태를 완료로, 완료된 일 목록에 등록 통과 추가

---

## 2026-07-30 21:27:39 — 목표·할 일 문서(ROADMAP.md) 신설

### 요청
- 목표와 할 일이 따로 정리되어 있지 않으니 문서로 정리할 것.

### 분석 내용
- 목표는 DESIGN.md(3대 목표)와 README(로드맵)에 문장 형태로만 흩어져 있고,
  "무엇을 해야 완료인지"에 해당하는 실행 단위 할 일 목록은 어느 문서에도 없음.
- 진행 중인 일(패키지 심사 대응)은 PACKAGING.md에 이력·확인 방법만 있고 남은 액션이 목록화되어 있지 않음.
- 완료 상태 확인: 목표 1 완료(v0.1.0 배포), 목표 2·3 미착수(설계만 확정),
  Chocolatey/winget은 최초 등록 심사 대기.

### 설계 방향
- 기존 문서와 역할이 겹치지 않도록 분담을 명확히 함 —
  ROADMAP은 **앞으로 할 일**, DESIGN은 **설계 근거**, CHANGELOG는 **완료 이력**, PACKAGING은 **배포 현황**.
- 할 일은 목표 2를 기반→파서→가로채기→UX→검증 순으로 분해해 각 단계마다 동작하는 결과물이 나오게 배열.
- 초경량 원칙(8KB 예산, CRT 미링크, 설정 GUI 금지 등)을 "불변 제약"으로 문서 상단에 못박아
  새 기능 추가 시 완료 기준으로 쓰이게 함.

### 개발 내용 및 소스 위치
- `docs/ROADMAP.md` (신규) — 목표 요약 표(상태 포함), 불변 원칙, 진행 중인 배포 심사 액션,
  목표 2 할 일 5개 그룹, 목표 3 할 일, 상시 운영 과제, 완료된 일 요약
- `README.md` — 로드맵 절에 ROADMAP.md 링크 추가

---

## 2026-07-05 01:38:59 — Windows 개발환경(MSYS2) 구성·검증 및 문서 최신화

### 요청
- 프로젝트 진행 내용을 분석하고 빌드에 필요한 Windows 환경을 구성할 것.
- (진행 중) 32비트·64비트를 모두 컴파일할 수 있는 방법을 찾을 것, 필요하면 잘못 깐 도구는 제거할 것.
- GitHub Actions에서 빌드가 잘 되는지 확인할 것.
- 진행 내용을 정리해 프로젝트 빌드 환경 설정 방법을 상세 문서로 남기고, 전체 문서를 최신화할 것.

### 분석 내용
- 이 PC 초기 상태: C 컴파일러(gcc/cl)·make 전무. choco·winget·python만 존재.
- `choco install mingw`(mingw-builds 16.1.0)은 **64비트 전용**임을 실측 확인:
  `gcc -dumpmachine` = `x86_64-w64-mingw32`, `gcc -m32` 링크 실패(멀티립 아님),
  `i686-w64-mingw32-gcc` 부재 → **단일 mingw 패키지로는 32비트 빌드 불가**.
- 패키지 배포 상태 재점검(2026-07-05): Chocolatey는 커뮤니티 피드 미노출(모더레이션 중),
  winget PR #397365는 open·미병합이나 라벨이 `Validation-Completed`/`Azure-Pipeline-Passed`로 진전.
- GitHub Actions 최신 실행(main HEAD e3d0ec8) success — build(x64+x86)·크기 회귀 검사 통과,
  Package/Release·chocolatey/winget 잡은 `v*` 태그 전용이라 skipped(정상).

### 설계 방향
- 32/64비트를 한 도구로 모두 지원하기 위해 64비트 전용 `mingw`를 제거하고 **MSYS2** 채택
  (pacman으로 x86_64·i686 두 툴체인 + make + Unix 셸 일괄 제공, README의 UCRT64/MINGW32 구분과 일치).
- 빌드는 아키텍처별로 PATH를 격리(`/mingw64/bin` vs `/mingw32/bin`)해 `gcc` 모호성 제거,
  Makefile의 `CC*/RES*` 변수를 접두어 없는 `gcc`/`windres`로 덮어써 나눠 빌드.
- 환경 구성은 별도 문서로 분리(README는 명령 요약만 유지하는 기존 방침 준수) + README에서 링크.

### 개발 내용 및 소스 위치
- 실제 구성·검증: `choco uninstall mingw make` → `choco install msys2` →
  `pacman -S mingw-w64-x86_64-gcc mingw-w64-i686-gcc make binutils` →
  x64/x86 각각 빌드 성공 (`dist/nShiftSpace-x64.exe`, `-x86.exe` 각 **4,608바이트**, 8KB 예산 통과).
- `docs/DEV-ENV-WINDOWS.md` (신규) — MSYS2 설치·양쪽 툴체인 구성·빌드·검증·문제 해결 상세 절차.
- `README.md` — 방법 2(MSYS2)에 DEV-ENV-WINDOWS.md 링크 추가.
- `docs/PACKAGING.md` — 상태 기준일 2026-07-05로 갱신, winget 진전(Validation-Completed) 반영,
  Chocolatey/winget 진행 이력에 2026-07-05 점검 항목 추가.

---

## 2026-07-04 01:14:23 — 패키지 관리자 등록 문서 신설, README 진행 상태 반영

### 요청
- Chocolatey/winget 등록 진행 상황을 README에 반영할 것.
- 등록 진행 상태를 확인할 수 있는 정보와 진행 내용을 담은 패키지 관리자 등록 문서를 만들 것.

### 분석 내용
- 실시간 상태 확인(2026-07-04 01:10): Chocolatey는 모더레이션 심사 중(패키지 페이지 배너),
  winget PR은 wingetbot 검증 파이프라인 실행 중, CLA 체크 통과.

### 개발 내용 및 소스 위치
- `docs/PACKAGING.md` (신규) — 채널별 상태 요약 표, 상태 확인 방법(패키지 페이지·PR 라벨 해석),
  구성 요소, 진행 이력 타임라인, 운영 규칙(게시 태그 재발행 금지 등)
- `README.md` — 설치 섹션에 심사 진행 중 안내와 PACKAGING.md 링크 추가

---

## 2026-07-04 00:58:49 — winget 배포 진행 (SosomLab.nShiftSpace 0.1.0 PR 제출)

### 요청
- winget에 배포하는 과정을 진행할 것. (포크 kiros33/winget-pkgs 는 사용자가 미리 생성)

### 분석 내용
- winget은 microsoft/winget-pkgs 저장소에 매니페스트 YAML 3종(version/installer/defaultLocale)을
  PR로 제출하는 방식. zip 안의 포터블 exe는 `InstallerType: zip` + `NestedInstallerType: portable`로 표현.
- 매니페스트에 릴리스 zip의 SHA256이 고정되므로 **게시된 태그(v0.1.0)는 이후 재발행 금지**
  (재태깅 시 체크섬 불일치로 winget 설치가 깨짐).

### 설계 방향
- 패키지 식별자 `SosomLab.nShiftSpace`, 명령 별칭(moniker/alias) `nshiftspace`.
- 최초 등록은 GitHub API로 포크에 브랜치·파일 생성 후 수동 PR
  (신규 패키지는 wingetcreate update 불가). 매니페스트 사본을 packaging/winget/에 보관.
- 이후 버전은 CI `winget` 잡이 wingetcreate로 업데이트 PR 자동 제출
  (`WINGET_TOKEN` 시크릿 필요 — public_repo 권한 PAT, 없으면 스킵).

### 개발 내용 및 소스 위치
- `packaging/winget/manifests/s/SosomLab/nShiftSpace/0.1.0/` (신규) — 매니페스트 3종
- `.github/workflows/build.yml` — `winget` 잡 추가 (태그 트리거)
- `README.md` — 설치 섹션에 winget 명령 추가
- 제출된 PR: https://github.com/microsoft/winget-pkgs/pull/397365 (검증 파이프라인 + 심사 대기)

---

## 2026-07-04 00:53:41 — Chocolatey 첫 게시 완료 (nshiftspace v0.1.0)

### 요청
- CHOCO_API_KEY 시크릿 등록 완료에 따라 실제 게시 진행.

### 개발 내용 및 소스 위치
- v0.1.0 태그 재발행 → CI chocolatey 잡에서 `choco push` 성공,
  chocolatey.org에 nshiftspace 0.1.0 제출됨 (커뮤니티 모더레이션 심사 대기).
- 패키지 페이지: https://community.chocolatey.org/packages/nshiftspace

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
