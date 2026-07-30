# 패키지 관리자 등록 (Chocolatey / winget)

nShiftSpace의 Windows 패키지 관리자 배포 현황, 등록 절차, 상태 확인 방법을 기록한다.

## 한눈에 보기

| 채널 | 패키지 식별자 | 설치 명령 | 상태 (2026-07-30 기준) |
|---|---|---|---|
| Chocolatey | `nshiftspace` | `choco install nshiftspace` | 0.1.0 **승인 완료** (2026-07-30 21:01, `IsApproved=true`) |
| winget | `SosomLab.nShiftSpace` | `winget install SosomLab.nShiftSpace` | 0.1.0 **등록 완료** (PR #397365 병합, 2026-07-17 05:23) |

두 채널 모두 GitHub Release의 zip(`nShiftSpace-x64.zip` / `nShiftSpace-x86.zip`)을
다운로드해 설치하는 원격형 패키지다.
[GitHub Releases](https://github.com/SosomLab/nexa-shortcut/releases)에서 zip을 직접 받아 쓰는 것도 여전히 가능.

---

## 설치 안내 (사용자용)

README에도 요약이 있으며, 여기서는 채널별 차이와 문제 해결까지 다룬다.

### 명령 대조표

| 작업 | winget | Chocolatey |
|---|---|---|
| 사전 준비 | Windows 11 / Windows 10 1809+ 기본 포함 (없으면 Store의 "앱 설치 관리자") | Chocolatey 설치 필요 — https://chocolatey.org/install |
| 권한 | 일반 사용자 | **관리자 권한 PowerShell** |
| 설치 | `winget install SosomLab.nShiftSpace` | `choco install nshiftspace -y` |
| 실행 | `nshiftspace` (포터블 별칭) | `nShiftSpace-x64` (또는 `-x86`) |
| 업그레이드 | `winget upgrade SosomLab.nShiftSpace` | `choco upgrade nshiftspace -y` |
| 제거 | `winget uninstall SosomLab.nShiftSpace` | `choco uninstall nshiftspace -y` |
| 설치 경로 | `%LOCALAPPDATA%\Microsoft\WinGet\Packages\` (별칭 링크는 `…\WinGet\Links\`) | `%ChocolateyInstall%\lib\nshiftspace\tools\` |
| 설치되는 아키텍처 | 매니페스트의 x64/x86 중 OS에 맞는 것 | `Install-ChocolateyZipPackage`의 `url`/`url64bit` 중 OS에 맞는 것 하나 |

### 채널별 동작 차이

- **실행 중 프로세스 처리**: Chocolatey는 `chocolateybeforemodify.ps1`이 업그레이드/제거 전에
  실행 중인 nShiftSpace를 종료한다. **winget에는 이런 처리가 없으므로** 트레이에서 먼저 종료해야
  파일 잠김으로 실패하지 않는다.
- **shim과 콘솔**: Chocolatey는 tools 폴더의 exe에 shim을 만든다. GUI(트레이) 프로그램이라
  콘솔이 붙잡히지 않도록 설치 스크립트가 `nShiftSpace-*.exe.gui` 마커 파일을 생성한다.
- **시작 프로그램 등록**: 두 채널 모두 자동 등록하지 않는다. `Win+R` → `shell:startup`에
  위 설치 경로의 exe 바로가기를 넣는다. 업그레이드로 경로가 바뀔 수 있는 점에 유의
  (경로 고정이 필요하면 zip 직접 다운로드 권장).

### 문제 해결

| 증상 | 원인·조치 |
|---|---|
| `winget install` 시 패키지를 찾지 못함 | 소스 캐시가 오래됨 — `winget source update` 후 재시도 |
| `choco install`이 "package not found" | 승인 직후 피드 반영 지연 가능 — 잠시 후 재시도. `choco search nshiftspace`로 노출 확인 |
| 업그레이드/제거가 파일 잠김으로 실패 | 트레이 아이콘 우클릭 → 종료 후 재시도 (winget 경로에서 특히) |
| 실행해도 Shift+Space가 안 먹음 | 관리자 권한 창이 포커스를 가진 경우 일반 권한 프로세스의 단축키는 동작하지 않음 (알려진 제약) |
| 중복 실행된 것 같음 | 뮤텍스로 차단되어 두 번째 인스턴스는 즉시 종료된다. 트레이 아이콘이 사라졌다면 explorer 재시작 후 자동 복구됨 |

---

## Chocolatey

### 등록 상태 확인 방법

- **패키지 페이지**: https://community.chocolatey.org/packages/nshiftspace
  - "This package is under moderation review" 배너가 사라지고 버전에 *Approved* 가 표시되면 완료.
  - Version History 표에서 각 버전의 상태(Submitted → under review → Approved) 확인 가능.
- **계정 알림**: 심사 코멘트/수정 요청은 chocolatey.org 계정 메일로 통지됨.
- 모더레이션 단계: 자동 품질 검사(package-validator) → 설치 테스트(package-verifier) → 사람 심사.
  최초 등록은 통상 수일 소요.

### 구성 요소

- [packaging/chocolatey/nshiftspace.nuspec](../packaging/chocolatey/nshiftspace.nuspec) — 패키지 메타데이터 (`__VERSION__` 템플릿)
- [packaging/chocolatey/tools/chocolateyinstall.ps1](../packaging/chocolatey/tools/chocolateyinstall.ps1) — 릴리스 zip 다운로드·설치, GUI shim 마커
- [packaging/chocolatey/tools/chocolateybeforemodify.ps1](../packaging/chocolatey/tools/chocolateybeforemodify.ps1) — 업그레이드/제거 전 프로세스 종료
- CI `chocolatey` 잡 ([.github/workflows/build.yml](../.github/workflows/build.yml)) — `v*` 태그 시
  체크섬 주입 → `choco pack` → `choco push` (시크릿 `CHOCO_API_KEY`, 등록 완료).
  **v0.1.0에서 이미 전 스텝 성공 — 자동 게시 경로 검증 완료.** 다음 태그도 추가 작업 없이 게시된다.
  단 게시(push)가 자동일 뿐 **승인은 별개**로, 새 버전도 모더레이션 큐를 다시 거친다
  (0.1.0은 제출→승인에 약 27일 소요).

### 진행 이력

| 일시 | 내용 |
|---|---|
| 2026-07-04 00:43 | 패키지 명세·CI 자동 게시 파이프라인 구성, pack 단계 검증 |
| 2026-07-04 00:52 | `CHOCO_API_KEY` 시크릿 등록 (사용자) |
| 2026-07-04 00:53 | v0.1.0 `choco push` 성공 — nshiftspace 0.1.0 모더레이션 큐 진입 |
| 2026-07-05 01:38 | 상태 점검: `choco search nshiftspace` 미노출 — 여전히 모더레이션 심사 중 |
| 2026-07-30 21:01 | **모더레이션 승인 완료** — 커뮤니티 피드 API 확인 (`IsApproved=true`, `PackageStatus=Approved`, 누적 다운로드 10) |
| 2026-07-30 22:40 | 재점검: 패키지 페이지에 "approved by moderator flcdrg on 30 Jul 2026" 표시, 검증기 결과 `PackageTestResultStatus=Passing`·`PackageValidationResultStatus=Passing`, 커뮤니티 검색 피드 노출 확인. CI 자동 게시 잡은 v0.1.0 런(28671030609)에서 `choco push`까지 성공했음을 확인 |

---

## winget

### 등록 상태 확인 방법

- **등록 PR**: https://github.com/microsoft/winget-pkgs/pull/397365
  - `wingetbot`이 단 Validation Pipeline 링크에서 자동 검증(매니페스트 검사 + 설치 테스트) 진행 상황 확인.
  - 라벨 의미: `New-Package`(신규 패키지), `Validation-Completed`(검증 통과, 병합 대기),
    `Needs-Attention`/`Needs-Author-Feedback`(수정 필요 — 대응 필요).
  - PR이 **병합되면 등록 완료**. 수 시간 내 `winget search nshiftspace`로 확인 가능.
- CLA: 체크 통과 (license/cla SUCCESS).

### 구성 요소

- [packaging/winget/manifests/s/SosomLab/nShiftSpace/0.1.0/](../packaging/winget/manifests/s/SosomLab/nShiftSpace/0.1.0/) —
  매니페스트 3종 사본 (version / installer / defaultLocale, 스키마 1.6)
  - zip 안의 포터블 exe: `InstallerType: zip` + `NestedInstallerType: portable`, 별칭 `nshiftspace`
- CI `winget` 잡 — `v*` 태그 시 `wingetcreate update`로 업데이트 PR 자동 제출
  (시크릿 `WINGET_TOKEN`, 등록 완료). 최초 등록 PR이 병합되었으므로 **다음 버전부터 이 경로로 동작**
  — 첫 자동 제출은 v0.1.1 배포 시 실제 동작을 확인해야 한다.
- 제출용 포크: https://github.com/kiros33/winget-pkgs (브랜치 `sosomlab-nshiftspace-0.1.0`)

### 진행 이력

| 일시 | 내용 |
|---|---|
| 2026-07-04 00:57 | 매니페스트 3종 작성 (v0.1.0 zip SHA256 고정), 포크에 브랜치 생성 |
| 2026-07-04 00:58 | microsoft/winget-pkgs#397365 PR 제출 — 검증 파이프라인 실행 중 |
| 2026-07-04 01:04 | `WINGET_TOKEN` 시크릿 등록 (사용자) — 이후 버전 자동 제출 준비 완료 |
| 2026-07-04 01:22 | 상태 점검: CLA 체크 통과(Needs-CLA 라벨은 갱신 지연, 무해), `New-Package` 분류, Azure 검증 파이프라인 진행 중, 모더레이터 승인 대기 |
| 2026-07-05 01:38 | 상태 점검: PR open·미병합, 라벨 `Azure-Pipeline-Passed`+`Validation-Completed` (검증 통과) — 모더레이터 병합 대기 |
| 2026-07-17 05:23 | **PR #397365 병합 — 등록 완료.** 라벨 `Moderator-Approved`, `Publish-Pipeline-Succeeded` 추가. winget-pkgs master에 매니페스트 반영 확인 |

---

## 운영 규칙

- **게시된 태그는 재발행 금지.** winget 매니페스트와 Chocolatey 설치 스크립트에 릴리스 zip의
  SHA256이 고정되므로, 태그를 다시 만들면 체크섬 불일치로 설치가 깨진다.
  수정이 필요하면 반드시 새 버전(v0.1.1 …)으로 배포한다.
- 새 버전 배포는 `git tag v0.x.y && git push origin v0.x.y` 한 번으로
  GitHub Release → Chocolatey push → winget PR까지 자동 진행된다.
- 심사 중 수정 요청(양쪽 모두 가능)이 오면 패키지 스크립트/매니페스트를 고쳐 재제출한다.
