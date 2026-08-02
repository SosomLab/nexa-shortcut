# 패키지 관리자 등록 (Chocolatey / winget)

nShiftSpace의 Windows 패키지 관리자 배포 현황, 등록 절차, 상태 확인 방법을 기록한다.

## 한눈에 보기

| 채널 | 패키지 식별자 | 설치 명령 | 상태 (2026-08-02 재확인) |
|---|---|---|---|
| Chocolatey | `nshiftspace` | `choco install nshiftspace` | 0.1.0 **승인 완료·유지 중** (2026-07-30 21:01 승인, `IsApproved=true`) |
| winget | `SosomLab.nShiftSpace` | `winget install SosomLab.nShiftSpace` | 0.1.0 **등록 완료·유지 중** (PR #397365 병합, 2026-07-17 05:23) |

현재 배포된 버전은 **양쪽 모두 0.1.0 하나뿐**이며, v0.1.0 이후 새 태그·릴리스는 없다.

두 채널 모두 GitHub Release의 zip(`nShiftSpace-x64.zip` / `nShiftSpace-x86.zip`)을
다운로드해 설치하는 원격형 패키지다.
[GitHub Releases](https://github.com/SosomLab/nexa-shortcut/releases)에서 zip을 직접 받아 쓰는 것도 여전히 가능.

---

## 설치 안내 (사용자용)

> 📖 **사용자에게 안내할 때는 [Wiki](https://github.com/SosomLab/nexa-shortcut/wiki)를 가리킨다.**
> 같은 내용을 읽기 쉽게 페이지로 나눠 두었다
> ([설치하기](https://github.com/SosomLab/nexa-shortcut/wiki/Installation) ·
> [자동 실행 설정](https://github.com/SosomLab/nexa-shortcut/wiki/Autostart) ·
> [문제 해결](https://github.com/SosomLab/nexa-shortcut/wiki/Troubleshooting)).
> **Wiki 원고의 정본은 [docs/wiki/](wiki/)** 이며, 이 절의 내용을 고치면
> 해당 원고도 함께 고치고 `./tools/publish-wiki.sh`로 재발행해야 한다.

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
- **시작 프로그램 등록**: 두 채널 모두 자동 등록하지 않는다. 아래
  ["설치 후 자동 실행 설정"](#설치-후-자동-실행-설정) 절 참고.

### 문제 해결

| 증상 | 원인·조치 |
|---|---|
| `winget install` 시 패키지를 찾지 못함 | 소스 캐시가 오래됨 — `winget source update` 후 재시도 |
| `choco install`이 "package not found" | 승인 직후 피드 반영 지연 가능 — 잠시 후 재시도. `choco search nshiftspace`로 노출 확인 |
| 업그레이드/제거가 파일 잠김으로 실패 | 트레이 아이콘 우클릭 → 종료 후 재시도 (winget 경로에서 특히) |
| 실행해도 Shift+Space가 안 먹음 | 관리자 권한 창이 포커스를 가진 경우 일반 권한 프로세스의 단축키는 동작하지 않음 (알려진 제약) |
| 중복 실행된 것 같음 | 뮤텍스로 차단되어 두 번째 인스턴스는 즉시 종료된다. 트레이 아이콘이 사라졌다면 explorer 재시작 후 자동 복구됨 |

---

## 설치 후 자동 실행 설정

nShiftSpace는 상주 프로그램이므로 **로그인할 때 자동으로 떠 있어야** 제 역할을 한다.
두 채널 모두 시작 프로그램에 **자동 등록하지 않으므로** 아래 절차를 한 번 수행한다.

> ℹ️ 이 절의 명령은 Windows PowerShell 기준으로 작성했다. exe 경로 규칙과 권한 조건은
> 각 패키지 관리자의 공식 동작을 근거로 정리한 것이며, 사용자의 Windows 환경에서
> 1단계 `Test-Path` 확인부터 차례로 실행하면 자기 PC의 실제 경로를 확정할 수 있다.

### 왜 winget 쪽이 까다로운가 — 경로 안정성 차이

| 채널 | 시작 프로그램이 가리킬 경로 | 업그레이드 후에도 유지되나 |
|---|---|---|
| Chocolatey | `%ChocolateyInstall%\lib\nshiftspace\tools\nShiftSpace-x64.exe` | ✅ 유지 (경로에 버전이 들어가지 않음) |
| winget (별칭) | `%LOCALAPPDATA%\Microsoft\WinGet\Links\nshiftspace.exe` | ✅ 유지 — **winget에서는 이 경로를 써야 한다** |
| winget (실제 exe) | `%LOCALAPPDATA%\Microsoft\WinGet\Packages\SosomLab.nShiftSpace_Microsoft.Winget.Source_8wekyb3d8bbwe\nShiftSpace-x64.exe` | ⚠️ 패키지 폴더가 재생성되며 끊길 수 있음 |

winget 자동 실행 설정이 "쉽지 않은" 이유가 여기 있다. 탐색기로 설치 폴더를 찾아 들어가면
버전·소스 ID가 섞인 **Packages 폴더의 실제 exe**를 집게 되는데, 이 경로로 바로가기를 만들면
`winget upgrade` 후 바로가기가 끊긴다. **`Links` 폴더의 별칭(`nshiftspace.exe`)을 가리켜야**
업그레이드에도 살아남는다.

---

### 1단계 — 가리킬 exe 경로 확정 (winget)

일반 PowerShell에서:

```powershell
$exe = "$env:LOCALAPPDATA\Microsoft\WinGet\Links\nshiftspace.exe"
Test-Path $exe
```

- **`True`** → 이 `$exe`를 그대로 2단계에서 쓴다. (권장 경로)
- **`False`** → 별칭이 만들어지지 않은 것이다. 원인과 대응은 아래.

#### 별칭이 없을 때 (`False`인 경우)

winget이 포터블 패키지의 별칭을 만들 때는 **심볼릭 링크**를 생성하므로,
Windows에서 **개발자 모드가 꺼져 있으면** 별칭 생성이 실패하고 설치 로그에 경고가 남는다.
둘 중 하나로 해결한다.

**(a) 개발자 모드를 켜고 재설치 — 권장**

```
설정 → 개인 정보 및 보안 → 개발자용 → "개발자 모드" 켬
```

켠 뒤 재설치하면 별칭이 생성된다:

```powershell
winget uninstall SosomLab.nShiftSpace
winget install  SosomLab.nShiftSpace
Test-Path "$env:LOCALAPPDATA\Microsoft\WinGet\Links\nshiftspace.exe"
```

**(b) 개발자 모드를 켤 수 없는 환경 — 실제 exe 경로를 직접 찾는다**

```powershell
$exe = (Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages\SosomLab.nShiftSpace_*" `
        -Recurse -Filter "nShiftSpace-*.exe" |
        Select-Object -First 1).FullName
$exe    # 경로 출력 확인
```

> 이 방법을 쓰면 **`winget upgrade` 때마다 시작 프로그램을 다시 등록해야 할 수 있다.**
> 경로 고정이 중요하다면 Chocolatey로 설치하거나, GitHub Releases의 zip을 원하는 폴더에
> 풀어 쓰는 편이 확실하다.

#### Chocolatey로 설치한 경우의 1단계

관리자 권한이 아니어도 경로 확인은 된다:

```powershell
$exe = "$env:ChocolateyInstall\lib\nshiftspace\tools\nShiftSpace-x64.exe"   # 32비트 OS면 -x86
Test-Path $exe
```

`$env:ChocolateyInstall`이 비어 있으면 기본값 `C:\ProgramData\chocolatey`를 쓴다.

---

### 2단계 — 자동 실행 등록 (A·B·C 중 하나만)

#### 방법 A — 시작 프로그램 폴더에 바로가기 (가장 표준적)

`$exe`가 1단계에서 잡혀 있는 **같은 PowerShell 창**에서 이어서 실행한다:

```powershell
$startup  = [Environment]::GetFolderPath('Startup')
$lnk      = Join-Path $startup 'nShiftSpace.lnk'

$ws = New-Object -ComObject WScript.Shell
$sc = $ws.CreateShortcut($lnk)
$sc.TargetPath       = $exe
$sc.WorkingDirectory = Split-Path $exe
$sc.Description      = 'nShiftSpace - Shift+Space 한/영 전환'
$sc.Save()

Test-Path $lnk        # True 면 등록 완료
```

GUI로 하고 싶다면: `Win+R` → `shell:startup` → 열린 폴더에서 마우스 **오른쪽 버튼 드래그**로
`$exe`를 끌어다 놓고 **"여기에 바로 가기 만들기"** 선택. (그냥 끌면 복사/이동이 되어버린다.)

#### 방법 B — 레지스트리 Run 키 (한 줄, 바로가기 파일 없음)

```powershell
New-ItemProperty -Path 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run' `
  -Name 'nShiftSpace' -Value "`"$exe`"" -PropertyType String -Force
```

경로에 공백이 있을 수 있으므로 **큰따옴표로 감싸는 것**이 중요하다.

#### 방법 C — 작업 스케줄러 (관리자 권한 창에서도 단축키가 먹게 하려면 ★)

nShiftSpace의 알려진 제약 — **관리자 권한 창이 포커스를 가지면 일반 권한 프로세스의 단축키는
동작하지 않는다.** 이걸 피하려면 로그인 시 **높은 권한으로** 실행해야 하는데, 시작 프로그램
폴더나 Run 키로는 불가능하고 작업 스케줄러만 가능하다.

**관리자 권한 PowerShell**에서:

```powershell
$exe = "$env:LOCALAPPDATA\Microsoft\WinGet\Links\nshiftspace.exe"   # 1단계에서 확정한 경로

$action    = New-ScheduledTaskAction  -Execute $exe
$trigger   = New-ScheduledTaskTrigger -AtLogOn -User $env:USERNAME
$principal = New-ScheduledTaskPrincipal -UserId $env:USERNAME `
               -LogonType Interactive -RunLevel Highest
$settings  = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries `
               -DontStopIfGoingOnBatteries -ExecutionTimeLimit 0

Register-ScheduledTask -TaskName 'nShiftSpace' -Action $action -Trigger $trigger `
  -Principal $principal -Settings $settings -Force
```

- `-RunLevel Highest` 가 핵심 — 이게 있어야 관리자 창 위에서도 Shift+Space가 동작한다.
- `-ExecutionTimeLimit 0` = 시간 제한 없음. 상주 프로그램이므로 반드시 넣는다
  (기본값 3일이 지나면 작업이 종료된다).
- 이 방식은 **UAC 프롬프트 없이** 승격 실행되므로 로그인 때 창이 뜨지 않는다.
- 관리자 계정이 아니거나 조직 정책으로 막힌 PC에서는 실패할 수 있다 — 그럴 땐 방법 A를 쓴다.

---

### 3단계 — 확인

로그아웃 후 다시 로그인(또는 재부팅)해서 확인한다. 재부팅 없이 바로 확인하려면:

```powershell
Start-Process $exe
Get-Process nShiftSpace-x64, nshiftspace -ErrorAction SilentlyContinue   # 프로세스 확인
```

트레이에 '가·A' 아이콘이 보이고 Shift+Space로 한/영이 전환되면 정상이다.
중복 실행은 뮤텍스로 차단되므로 두 번 떠서 충돌할 걱정은 없다.

### 해제 방법

등록에 쓴 방법과 **같은 방법**으로 지운다.

```powershell
# A - 바로가기
Remove-Item (Join-Path ([Environment]::GetFolderPath('Startup')) 'nShiftSpace.lnk')

# B - Run 키
Remove-ItemProperty -Path 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run' -Name 'nShiftSpace'

# C - 예약 작업 (관리자 권한 PowerShell)
Unregister-ScheduledTask -TaskName 'nShiftSpace' -Confirm:$false
```

### 업그레이드할 때 주의

자동 실행을 걸어 두면 nShiftSpace가 항상 떠 있으므로, 업그레이드 시 **파일 잠김**이 발생한다.

- **Chocolatey**: `chocolateybeforemodify.ps1`이 알아서 종료시킨다 — 그대로 `choco upgrade`.
- **winget**: 종료 처리가 없다. **트레이 아이콘 우클릭 → 종료** 후 `winget upgrade`를 실행한다.

업그레이드 후 방법 A·B를 쓴 경우 1단계 `Test-Path`로 경로가 유효한지 한 번 확인한다
(`Links` 별칭 경로를 썼다면 바뀌지 않는다). 방법 C는 예약 작업이 그대로 유지된다.

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

### 시작 프로그램 바로가기 자동 생성 — 가능하나 미적용 (검토 결과)

**결론: 기술적으로 가능하다.** Chocolatey 헬퍼 `Install-ChocolateyShortcut`으로
`chocolateyinstall.ps1`에서 사용자 시작 프로그램 폴더(`shell:startup`)에 `.lnk`를 만들 수 있다.
winget에는 대응 수단이 없는, **Chocolatey만의 장점**이다.

```powershell
# chocolateyinstall.ps1 에 추가할 형태 (opt-in 예시)
$pp = Get-PackageParameters
if ($pp.Startup) {
  Install-ChocolateyShortcut `
    -ShortcutFilePath (Join-Path ([Environment]::GetFolderPath('Startup')) 'nShiftSpace.lnk') `
    -TargetPath       (Join-Path $toolsDir 'nShiftSpace-x64.exe') `
    -WorkingDirectory $toolsDir `
    -Description      'nShiftSpace - Shift+Space 한/영 전환'
}
```

경로 측면의 궁합도 좋다 — choco의 `tools` 경로에는 버전이 들어가지 않아
업그레이드해도 바로가기가 끊기지 않는다.

다만 **바로 적용하지 않은 이유**가 있다.

| 고려사항 | 내용 |
|---|---|
| 설치 컨텍스트 | `choco install`은 **관리자 권한**으로 실행된다. 같은 사용자가 UAC 승격한 것이라면 `Startup`이 그 사용자 폴더로 잡히지만, **SYSTEM 계정이나 다른 사용자로 실행**되면(배포 도구·CM 연동 등) 엉뚱한 프로필에 바로가기가 생긴다 |
| 기본값 | 사용자가 요청하지 않은 자동 실행 등록은 침습적이다. `--params "/Startup"` **opt-in**으로 두는 것이 Chocolatey 관례에 맞다 |
| 제거 처리 | `chocolateyuninstall.ps1`을 새로 만들어 바로가기를 지워야 한다 (현재 패키지에는 uninstall 스크립트가 없음) |
| 배포 비용 | 패키지 스크립트 변경 = **새 버전 배포 + 모더레이션 재심사**. 0.1.0 실적 기준 승인까지 약 27일 |
| 채널 간 불일치 | winget에는 같은 기능을 넣을 수 없어 두 채널의 설치 후 상태가 달라진다 |

→ 적용한다면 **v0.1.1에 opt-in 방식**(`choco install nshiftspace --params "/Startup"`)으로
uninstall 스크립트와 함께 넣는 것이 타당하다. [ROADMAP.md](ROADMAP.md)에 후보 과제로 올려 두었다.

### 진행 이력

| 일시 | 내용 |
|---|---|
| 2026-07-04 00:43 | 패키지 명세·CI 자동 게시 파이프라인 구성, pack 단계 검증 |
| 2026-07-04 00:52 | `CHOCO_API_KEY` 시크릿 등록 (사용자) |
| 2026-07-04 00:53 | v0.1.0 `choco push` 성공 — nshiftspace 0.1.0 모더레이션 큐 진입 |
| 2026-07-05 01:38 | 상태 점검: `choco search nshiftspace` 미노출 — 여전히 모더레이션 심사 중 |
| 2026-07-30 21:01 | **모더레이션 승인 완료** — 커뮤니티 피드 API 확인 (`IsApproved=true`, `PackageStatus=Approved`, 누적 다운로드 10) |
| 2026-07-30 22:40 | 재점검: 패키지 페이지에 "approved by moderator flcdrg on 30 Jul 2026" 표시, 검증기 결과 `PackageTestResultStatus=Passing`·`PackageValidationResultStatus=Passing`, 커뮤니티 검색 피드 노출 확인. CI 자동 게시 잡은 v0.1.0 런(28671030609)에서 `choco push`까지 성공했음을 확인 |
| 2026-08-02 | 상태 재확인 — 변동 없음. 피드 API `IsApproved=true`·`PackageStatus=Approved`·`IsLatestVersion=true`, 검증/테스트 모두 `Passing` 유지, 게시 버전은 0.1.0 단일, 누적 다운로드 11(7/30과 동일) |

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
| 2026-08-02 | 상태 재확인 — 변동 없음. PR #397365 `merged=true`(2026-07-16T20:23:39Z UTC), winget-pkgs master의 `manifests/s/SosomLab/nShiftSpace/0.1.0/`에 매니페스트 3종 존재. 등록된 버전 디렉터리는 `0.1.0` 하나뿐이며, 게시 매니페스트에서 별칭 `PortableCommandAlias: nshiftspace`·`NestedInstallerType: portable` 재확인 |

---

## 운영 규칙

- **게시된 태그는 재발행 금지.** winget 매니페스트와 Chocolatey 설치 스크립트에 릴리스 zip의
  SHA256이 고정되므로, 태그를 다시 만들면 체크섬 불일치로 설치가 깨진다.
  수정이 필요하면 반드시 새 버전(v0.1.1 …)으로 배포한다.
- 새 버전 배포는 `git tag v0.x.y && git push origin v0.x.y` 한 번으로
  GitHub Release → Chocolatey push → winget PR까지 자동 진행된다.
- 심사 중 수정 요청(양쪽 모두 가능)이 오면 패키지 스크립트/매니페스트를 고쳐 재제출한다.
