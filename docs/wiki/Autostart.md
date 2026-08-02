# 자동 실행 설정 (시작 프로그램 등록)

nShiftSpace는 상주 프로그램입니다. **로그인할 때 자동으로 떠 있어야** 제 역할을 하는데,
winget과 Chocolatey **둘 다 시작 프로그램에 자동 등록하지 않습니다.** 아래 절차를 한 번만 하면 됩니다.

> 이 문서의 명령은 모두 **Windows PowerShell** 기준입니다.

---

## 왜 winget 쪽이 헷갈리는가 — 먼저 읽어보세요

탐색기로 winget 설치 폴더를 찾아 들어가면 이런 경로의 exe를 만나게 됩니다.

```
%LOCALAPPDATA%\Microsoft\WinGet\Packages\SosomLab.nShiftSpace_Microsoft.Winget.Source_8wekyb3d8bbwe\nShiftSpace-x64.exe
```

**이 경로로 바로가기를 만들면 안 됩니다.** `winget upgrade` 때 패키지 폴더가 다시 만들어지면서
바로가기가 끊어집니다. 대신 **별칭 경로**를 써야 합니다.

```
%LOCALAPPDATA%\Microsoft\WinGet\Links\nshiftspace.exe   ← 이걸 가리키세요
```

| 채널 | 가리킬 경로 | 업그레이드 후 유지 |
|---|---|---|
| winget | `%LOCALAPPDATA%\Microsoft\WinGet\Links\nshiftspace.exe` | ✅ |
| Chocolatey | `%ChocolateyInstall%\lib\nshiftspace\tools\nShiftSpace-x64.exe` | ✅ (경로에 버전 없음) |
| zip 직접 | 압축 푼 위치의 exe | ✅ |

---

## 1단계 — 가리킬 exe 경로 확정

### winget으로 설치한 경우

```powershell
$exe = "$env:LOCALAPPDATA\Microsoft\WinGet\Links\nshiftspace.exe"
Test-Path $exe
```

- **`True`** → 그대로 [2단계](#2단계--자동-실행-등록-abc-중-하나만)로 넘어가세요.
- **`False`** → 별칭이 만들어지지 않았습니다. 아래를 보세요.

<details>
<summary><b>별칭이 없을 때 (<code>False</code>가 나온 경우)</b></summary>

winget이 포터블 패키지의 별칭을 만들 때 **심볼릭 링크**를 생성하는데,
Windows에서 **개발자 모드가 꺼져 있으면** 이 생성이 실패합니다.

**(a) 개발자 모드를 켜고 재설치 — 권장**

```
설정 → 개인 정보 및 보안 → 개발자용 → "개발자 모드" 켬
```

```powershell
winget uninstall SosomLab.nShiftSpace
winget install  SosomLab.nShiftSpace
Test-Path "$env:LOCALAPPDATA\Microsoft\WinGet\Links\nshiftspace.exe"
```

**(b) 개발자 모드를 켤 수 없다면 — 실제 exe 경로를 직접 찾기**

```powershell
$exe = (Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages\SosomLab.nShiftSpace_*" `
        -Recurse -Filter "nShiftSpace-*.exe" |
        Select-Object -First 1).FullName
$exe
```

이 경우 **업그레이드할 때마다 다시 등록해야 할 수 있습니다.** 경로 고정이 중요하다면
[zip 직접 다운로드](Installation#zip-직접-다운로드)나 Chocolatey 설치를 권합니다.

</details>

### Chocolatey로 설치한 경우

```powershell
$exe = "$env:ChocolateyInstall\lib\nshiftspace\tools\nShiftSpace-x64.exe"   # 32비트 OS면 -x86
Test-Path $exe
```

`$env:ChocolateyInstall`이 비어 있으면 기본값은 `C:\ProgramData\chocolatey` 입니다.

### zip으로 설치한 경우

```powershell
$exe = "C:\Tools\nShiftSpace\nShiftSpace-x64.exe"   # 압축 푼 실제 위치로
Test-Path $exe
```

---

## 2단계 — 자동 실행 등록 (A·B·C 중 하나만)

**1단계에서 `$exe`를 설정한 같은 PowerShell 창**에서 이어서 실행하세요.

### 방법 A — 시작 프로그램 폴더에 바로가기 (가장 표준적)

```powershell
$startup = [Environment]::GetFolderPath('Startup')
$lnk     = Join-Path $startup 'nShiftSpace.lnk'

$ws = New-Object -ComObject WScript.Shell
$sc = $ws.CreateShortcut($lnk)
$sc.TargetPath       = $exe
$sc.WorkingDirectory = Split-Path $exe
$sc.Description      = 'nShiftSpace - Shift+Space 한/영 전환'
$sc.Save()

Test-Path $lnk        # True 면 등록 완료
```

**GUI로 하고 싶다면**: `Win+R` → `shell:startup` 입력 → 열린 폴더에
exe를 **마우스 오른쪽 버튼으로 드래그**해서 놓고 **"여기에 바로 가기 만들기"** 선택.

> 왼쪽 버튼으로 그냥 끌면 복사나 이동이 되어버립니다. 꼭 **오른쪽 버튼 드래그**로 하세요.

### 방법 B — 레지스트리 Run 키 (파일 없이 한 줄로)

```powershell
New-ItemProperty -Path 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run' `
  -Name 'nShiftSpace' -Value "`"$exe`"" -PropertyType String -Force
```

경로에 공백이 들어갈 수 있으므로 **큰따옴표로 감싸는 것**이 중요합니다.

### 방법 C — 작업 스케줄러 ★ 관리자 창에서도 단축키가 먹게 하려면

nShiftSpace에는 알려진 제약이 있습니다 — **관리자 권한으로 실행된 창이 포커스를 가지면
일반 권한 프로그램의 단축키는 동작하지 않습니다.** (Windows의 보안 정책이라 우회 불가)

이걸 피하려면 nShiftSpace 자체를 **높은 권한으로** 띄워야 하는데,
시작 프로그램 폴더나 Run 키로는 불가능하고 **작업 스케줄러만** 가능합니다.

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

핵심 옵션 설명:

| 옵션 | 이유 |
|---|---|
| `-RunLevel Highest` | **이게 핵심.** 관리자 창 위에서도 Shift+Space가 동작하게 함 |
| `-ExecutionTimeLimit 0` | 시간 제한 없음. 상주 프로그램이라 필수 (기본값 3일이면 종료됨) |
| `-AllowStartIfOnBatteries` | 노트북에서 배터리 사용 중에도 실행 |
| `-LogonType Interactive` | 로그인 세션에서 실행 (트레이 아이콘이 보이려면 필요) |

- 승격 실행이지만 **UAC 프롬프트는 뜨지 않습니다** — 로그인 때 조용히 실행됩니다.
- 관리자 계정이 아니거나 조직 정책으로 막힌 PC에서는 실패할 수 있습니다. 그럴 땐 방법 A를 쓰세요.

---

## 3단계 — 확인

로그아웃 후 다시 로그인(또는 재부팅)해서 트레이에 **'가·A' 아이콘**이 보이는지 확인합니다.

재부팅 없이 지금 바로 확인하려면:

```powershell
Start-Process $exe
Get-Process nShiftSpace-x64, nshiftspace -ErrorAction SilentlyContinue
```

아무 곳에나 커서를 두고 **Shift+Space**를 눌러 한/영이 전환되면 정상입니다.

> 중복 실행은 프로그램 내부의 뮤텍스로 차단되므로, 두 번 실행돼서 충돌할 걱정은 없습니다.

---

## 해제 방법

등록할 때 쓴 방법과 **같은 방법**으로 지웁니다.

```powershell
# 방법 A - 바로가기 삭제
Remove-Item (Join-Path ([Environment]::GetFolderPath('Startup')) 'nShiftSpace.lnk')

# 방법 B - Run 키 삭제
Remove-ItemProperty -Path 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run' -Name 'nShiftSpace'

# 방법 C - 예약 작업 삭제 (관리자 권한 PowerShell)
Unregister-ScheduledTask -TaskName 'nShiftSpace' -Confirm:$false
```

GUI로 확인·해제하려면 **작업 관리자 → 시작 프로그램** 탭 (방법 A·B),
**작업 스케줄러** (방법 C)에서도 가능합니다.

---

## 업그레이드할 때 주의

자동 실행을 걸어 두면 nShiftSpace가 항상 떠 있으므로 업그레이드 시 **파일 잠김**이 생깁니다.

| 채널 | 해야 할 일 |
|---|---|
| Chocolatey | 없음 — 알아서 종료시켜 줍니다. 그대로 `choco upgrade nshiftspace -y` |
| winget | **트레이 아이콘 우클릭 → 종료** 후 `winget upgrade SosomLab.nShiftSpace` |

업그레이드 후, 방법 A·B를 썼다면 1단계의 `Test-Path`로 경로가 아직 유효한지 한 번 확인하세요.
`Links` 별칭 경로를 썼다면 바뀌지 않습니다. 방법 C의 예약 작업도 그대로 유지됩니다.

---

## 어떤 방법을 고를까

| 상황 | 추천 |
|---|---|
| 그냥 자동으로 뜨기만 하면 됨 | **방법 A** |
| 시작 프로그램 폴더를 깔끔히 두고 싶음 | 방법 B |
| 관리자 권한 창(작업 관리자, 승격된 터미널 등)에서도 한/영 전환이 필요 | **방법 C** |

---

- 다음: [사용법](Usage) · [문제 해결](Troubleshooting)
- 설치부터 다시: [설치하기](Installation)
