# 패키지 관리자 등록 (Chocolatey / winget)

nShiftSpace의 Windows 패키지 관리자 배포 현황, 등록 절차, 상태 확인 방법을 기록한다.

## 한눈에 보기

| 채널 | 패키지 식별자 | 설치 명령 | 상태 (2026-07-05 기준) |
|---|---|---|---|
| Chocolatey | `nshiftspace` | `choco install nshiftspace` | 0.1.0 제출 완료 — **모더레이션 심사 중** (피드 미노출) |
| winget | `SosomLab.nShiftSpace` | `winget install SosomLab.nShiftSpace` | 0.1.0 PR **검증 완료(Validation-Completed)** — 모더레이터 병합 대기 |

두 채널 모두 GitHub Release의 zip(`nShiftSpace-x64.zip` / `nShiftSpace-x86.zip`)을
다운로드해 설치하는 원격형 패키지다. 심사 완료 전에는
[GitHub Releases](https://github.com/SosomLab/nexa-shortcut/releases)가 유일한 설치 경로.

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
  체크섬 주입 → `choco pack` → `choco push` (시크릿 `CHOCO_API_KEY`, 등록 완료)

### 진행 이력

| 일시 | 내용 |
|---|---|
| 2026-07-04 00:43 | 패키지 명세·CI 자동 게시 파이프라인 구성, pack 단계 검증 |
| 2026-07-04 00:52 | `CHOCO_API_KEY` 시크릿 등록 (사용자) |
| 2026-07-04 00:53 | v0.1.0 `choco push` 성공 — nshiftspace 0.1.0 모더레이션 큐 진입 |
| 2026-07-05 01:38 | 상태 점검: `choco search nshiftspace` 미노출 — 여전히 모더레이션 심사 중 |

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
  (시크릿 `WINGET_TOKEN`, 등록 완료). **최초 등록 PR 병합 이후 버전부터 동작.**
- 제출용 포크: https://github.com/kiros33/winget-pkgs (브랜치 `sosomlab-nshiftspace-0.1.0`)

### 진행 이력

| 일시 | 내용 |
|---|---|
| 2026-07-04 00:57 | 매니페스트 3종 작성 (v0.1.0 zip SHA256 고정), 포크에 브랜치 생성 |
| 2026-07-04 00:58 | microsoft/winget-pkgs#397365 PR 제출 — 검증 파이프라인 실행 중 |
| 2026-07-04 01:04 | `WINGET_TOKEN` 시크릿 등록 (사용자) — 이후 버전 자동 제출 준비 완료 |
| 2026-07-04 01:22 | 상태 점검: CLA 체크 통과(Needs-CLA 라벨은 갱신 지연, 무해), `New-Package` 분류, Azure 검증 파이프라인 진행 중, 모더레이터 승인 대기 |
| 2026-07-05 01:38 | 상태 점검: PR open·미병합, 라벨 `Azure-Pipeline-Passed`+`Validation-Completed` (검증 통과) — 모더레이터 병합 대기 |

---

## 운영 규칙

- **게시된 태그는 재발행 금지.** winget 매니페스트와 Chocolatey 설치 스크립트에 릴리스 zip의
  SHA256이 고정되므로, 태그를 다시 만들면 체크섬 불일치로 설치가 깨진다.
  수정이 필요하면 반드시 새 버전(v0.1.1 …)으로 배포한다.
- 새 버전 배포는 `git tag v0.x.y && git push origin v0.x.y` 한 번으로
  GitHub Release → Chocolatey push → winget PR까지 자동 진행된다.
- 심사 중 수정 요청(양쪽 모두 가능)이 오면 패키지 스크립트/매니페스트를 고쳐 재제출한다.
