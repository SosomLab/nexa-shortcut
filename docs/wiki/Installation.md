# 설치하기

nShiftSpace는 **설치 프로그램이 없는 포터블 exe** 하나가 전부입니다.
아래 3가지 방법 중 편한 것을 고르세요.

| 방법 | 권한 | 추천 대상 |
|---|---|---|
| [winget](#winget) | 일반 사용자 | Windows 10 1809+ / 11 사용자 대부분 |
| [Chocolatey](#chocolatey) | **관리자 권한** | 이미 choco를 쓰는 경우 |
| [zip 직접 다운로드](#zip-직접-다운로드) | 일반 사용자 | 경로를 내가 정하고 싶을 때 |

설치 후 한 번 실행하고 트레이 아이콘 우클릭 → **로그인 시 자동 실행** 을 체크하면 로그인 때마다 뜹니다.
세부와 대안은 **[자동 실행 설정](Autostart)** 참고.

---

## 일반판과 LED판 — 패키지 이름이 다릅니다

| | 일반판 | [키보드 LED 표시판](Usage#키보드-led-표시판-선택) |
|---|---|---|
| winget | `SosomLab.nShiftSpace` | `SosomLab.nShiftSpace.LED` |
| Chocolatey | `nshiftspace` | `nshiftspace-led` |
| zip | `nShiftSpace-x64.zip` / `-x86.zip` | `nShiftSpace-led-x64.zip` / `-x86.zip` |
| 실행 별칭 | `nshiftspace` (winget) / `nShiftSpace-x64` (choco) | `nshiftspace-led` (winget) / `nShiftSpace-led-x64` (choco) |

둘 중 **하나만** 설치하세요 (같은 단축키를 쓰므로 동시에 실행되지 않습니다). LED판 패키지는 v0.3.0에서 처음 제출되어
각 채널의 심사가 끝나야 설치됩니다. 아래 명령은 일반판 기준이며, LED판은 패키지 이름만 바꾸면 됩니다.

---

## winget

Windows 11과 Windows 10 1809 이상에는 winget이 기본 포함되어 있습니다.
없다면 Microsoft Store에서 **"앱 설치 관리자"** 를 설치하세요.

```powershell
winget install SosomLab.nShiftSpace   # 설치
nshiftspace                           # 실행
winget upgrade SosomLab.nShiftSpace   # 업그레이드
winget uninstall SosomLab.nShiftSpace # 제거
```

포터블 패키지로 설치되며 파일 위치는 이렇습니다.

| 항목 | 경로 |
|---|---|
| 실제 exe | `%LOCALAPPDATA%\Microsoft\WinGet\Packages\SosomLab.nShiftSpace_Microsoft.Winget.Source_8wekyb3d8bbwe\` |
| 실행 별칭 | `%LOCALAPPDATA%\Microsoft\WinGet\Links\nshiftspace.exe` |

> ⚠️ **업그레이드·제거 전에 트레이에서 먼저 종료하세요.** winget에는 실행 중인 프로그램을
> 종료해 주는 처리가 없어서, 켜져 있으면 파일 잠김으로 실패합니다.

---

## Chocolatey

Chocolatey가 없다면 **관리자 권한 PowerShell**에서 먼저 설치합니다
([공식 안내](https://chocolatey.org/install)).

```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

이후 **관리자 권한** PowerShell에서:

```powershell
choco install nshiftspace -y   # 설치
nShiftSpace-x64                # 실행 (32비트 OS는 nShiftSpace-x86)
choco upgrade nshiftspace -y   # 업그레이드
choco uninstall nshiftspace -y # 제거
```

OS 아키텍처에 맞는 exe **하나만** `%ChocolateyInstall%\lib\nshiftspace\tools\` 에 설치되고,
같은 이름의 shim이 PATH에 등록됩니다. GUI 프로그램이라 shim이 콘솔을 붙잡지 않도록
`.gui` 마커 파일이 함께 생성됩니다.

> ✅ Chocolatey는 업그레이드·제거 전에 실행 중인 nShiftSpace를 **자동으로 종료**합니다.

---

## zip 직접 다운로드

1. [Releases 페이지](https://github.com/SosomLab/nexa-shortcut/releases)에서
   `nShiftSpace-x64.zip` (64비트) 또는 `nShiftSpace-x86.zip` (32비트) 다운로드.
   키를 누르는 동안 트레이 아이콘이 켜지는 [키보드 LED 표시판](Usage#키보드-led-표시판-선택)을
   원하면 `nShiftSpace-led-x64.zip` / `nShiftSpace-led-x86.zip`
2. 원하는 폴더에 압축 해제 (예: `C:\Tools\nShiftSpace\`)
3. exe를 더블클릭하면 트레이에 아이콘이 뜹니다

**경로를 내가 정할 수 있다는 것이 장점**입니다. 패키지 관리자로 설치하면 업그레이드할 때
경로가 바뀔 수 있지만, 이 방법은 그럴 일이 없습니다.

---

## 채널 비교표

| 작업 | winget | Chocolatey |
|---|---|---|
| 사전 준비 | 대부분 기본 포함 | Chocolatey 설치 필요 |
| 권한 | 일반 사용자 | **관리자 권한 PowerShell** |
| 설치 | `winget install SosomLab.nShiftSpace` | `choco install nshiftspace -y` |
| 실행 | `nshiftspace` | `nShiftSpace-x64` |
| 업그레이드 | `winget upgrade SosomLab.nShiftSpace` | `choco upgrade nshiftspace -y` |
| 제거 | `winget uninstall SosomLab.nShiftSpace` | `choco uninstall nshiftspace -y` |
| 실행 중 자동 종료 | ❌ 직접 종료 필요 | ✅ 자동 |
| 설치 경로 안정성 | 별칭(`Links`) 경로는 안정적 | 안정적 |

---

## 다음 단계

- **[자동 실행 설정](Autostart)** — 로그인할 때 자동으로 뜨게 하기
- [사용법](Usage) · [문제 해결](Troubleshooting)
