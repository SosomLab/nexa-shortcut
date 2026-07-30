# nexa-shortcut

윈도우용 초경량 단축키 유틸리티 모음. GUI 프레임워크와 CRT 없이 순수 Win32 API로만 작성해
실행 파일 크기와 메모리 사용을 최소화한다.

- **SosomLab 홈페이지**: https://sosomlab.com
- **제품 소개 페이지**: https://sosomlab.com/apps/nexa-shortcut/
- **다운로드**: [GitHub Releases](https://github.com/SosomLab/nexa-shortcut/releases)

## nShiftSpace (목표 1 — 완료)

**Nexa ShiftSpace** — Shift+Space로 한/영 전환만 하는 단일 기능 상주 프로그램.

- 파일 크기: **4.5KB** (참고: 원조 jwShiftSpaceKey.exe는 12KB)
- 동작: Shift+Space 입력 시 한/영 키(VK_HANGUL)를 대신 눌러줌
- 자체 아이콘 내장: '가·A' 픽셀 아트, 198바이트 1bpp ICO (`tools/make_icon.py`로 생성)
- 트레이 아이콘 우클릭 → 종료
- 중복 실행 방지, 탐색기 재시작 시 트레이 아이콘 자동 복구

### 빌드

#### 방법 1 — macOS/Linux에서 크로스 컴파일

```bash
brew install mingw-w64   # 또는 apt install gcc-mingw-w64
make                     # dist/nShiftSpace-x64.exe, dist/nShiftSpace-x86.exe 생성
```

#### 방법 2 — Windows에서 컴파일 (MSYS2 / MinGW-w64)

**MSYS2 UCRT64** 셸에서:

```bash
make x64 CC64=gcc RES64=windres    # dist/nShiftSpace-x64.exe 생성
```

32비트 빌드는 **MSYS2 MINGW32** 셸에서 `make x86 CC32=gcc RES32=windres`.

> MSYS2 설치부터 32/64비트 툴체인 구성까지 처음부터 세팅하는 상세 절차는
> [docs/DEV-ENV-WINDOWS.md](docs/DEV-ENV-WINDOWS.md) 참고.

#### 방법 3 — Windows에서 컴파일 (Visual Studio / MSVC)

**x64 Native Tools Command Prompt for VS**에서 저장소 폴더로 이동 후:

```bat
build.bat               # dist\nShiftSpace-x64.exe 생성
```

MSVC에서도 CRT를 링크하지 않으므로(/NODEFAULTLIB + /ENTRY:start) 초경량 결과물이 나온다.

### 설치 (winget / Chocolatey)

> ✅ **두 채널 모두 등록이 완료되었습니다** (winget 2026-07-17 병합, Chocolatey 2026-07-30 승인).
> [GitHub Releases](https://github.com/SosomLab/nexa-shortcut/releases)에서 zip을 직접 내려받아
> 쓰는 것도 가능합니다.

#### winget (일반 PowerShell)

Windows 11과 Windows 10 1809 이상에는 winget이 기본 포함되어 있다(없으면 Microsoft Store에서 "앱 설치 관리자" 설치).

```powershell
winget install SosomLab.nShiftSpace   # 설치
nshiftspace                           # 실행 (포터블 별칭)
winget upgrade SosomLab.nShiftSpace   # 업그레이드
winget uninstall SosomLab.nShiftSpace # 제거
```

포터블 패키지로 설치되며 실행 파일은 `%LOCALAPPDATA%\Microsoft\WinGet\Packages\` 아래,
별칭 링크는 `%LOCALAPPDATA%\Microsoft\WinGet\Links\`에 생성된다.

#### Chocolatey (관리자 권한 PowerShell)

Chocolatey 자체가 없다면 먼저 설치한다([공식 안내](https://chocolatey.org/install)).

```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

이후 **관리자 권한** PowerShell에서:

```powershell
choco install nshiftspace -y   # 설치
nShiftSpace-x64                # 실행 (64비트 기준. 32비트 OS는 nShiftSpace-x86)
choco upgrade nshiftspace -y   # 업그레이드
choco uninstall nshiftspace -y # 제거
```

OS 아키텍처에 맞는 exe 하나만 `%ChocolateyInstall%\lib\nshiftspace\tools\`에 설치되고,
같은 이름의 shim이 PATH에 등록된다. GUI 프로그램이므로 shim이 콘솔을 붙잡지 않도록 `.gui` 마커가 함께 생성된다.

> 제거 전에 실행 중이면 Chocolatey가 프로세스를 먼저 종료한다(`chocolateybeforemodify.ps1`).
> winget은 그런 처리가 없으므로 **실행 중인 nShiftSpace를 트레이에서 종료한 뒤** 제거/업그레이드할 것.

패키지 명세는 [packaging/](packaging/)에 있으며, `v*` 태그 배포 시 CI가
chocolatey.org 게시와 winget-pkgs 업데이트 PR 제출을 자동으로 수행한다.
채널별 상태·상세 절차·진행 이력은 [docs/PACKAGING.md](docs/PACKAGING.md) 참고.

### 배포 (자동 릴리스)

`v*` 형식의 태그를 푸시하면 GitHub Actions가 빌드 후 x64/x86 실행 파일 2개를
GitHub Release에 자동 첨부한다:

```bash
git tag v0.1.0 && git push origin v0.1.0
```

### 시작 프로그램 등록

`Win+R` → `shell:startup` → 폴더에 exe 바로가기를 넣으면 로그인 시 자동 실행.
설치 경로는 채널마다 다르다.

| 설치 방법 | exe 경로 |
|---|---|
| winget | `%LOCALAPPDATA%\Microsoft\WinGet\Packages\SosomLab.nShiftSpace_*\nShiftSpace-x64.exe` |
| Chocolatey | `%ChocolateyInstall%\lib\nshiftspace\tools\nShiftSpace-x64.exe` |
| zip 직접 다운로드 | 압축을 푼 위치 |

> 업그레이드로 경로가 바뀌면 바로가기가 끊길 수 있다. 경로가 고정된 곳에 exe를 두고 쓰려면
> zip 직접 다운로드 방식이 안전하다.

## 로드맵

- **목표 2 — nexa-mapper**: `mappings.ini`로 사용자가 직접 정의하는 단순 키 재매핑 엔진
- **목표 3 — exe 내보내기**: 현재 매핑만 내장한 독립 exe 생성 (스텁 복사 + 설정 덧붙이기 방식)

목표별 할 일 목록과 진행 상태는 [docs/ROADMAP.md](docs/ROADMAP.md),
상세 설계는 [docs/DESIGN.md](docs/DESIGN.md), 변경 이력은 [docs/CHANGELOG.md](docs/CHANGELOG.md) 참고.

## 라이선스

[MIT License](LICENSE) — 누구나 무료로 사용·복사·수정·배포할 수 있습니다.
