# 변경기록 (CHANGELOG)

모든 변경은 이 문서에 시간 역순(최신이 위)으로 기록한다.
각 항목은 **요청 / 분석 내용 / 설계 방향 / 개발 내용 및 소스 위치**를 남긴다.

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
