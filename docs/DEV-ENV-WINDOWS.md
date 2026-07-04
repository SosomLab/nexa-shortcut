# Windows 개발환경 설치 (MSYS2 / MinGW-w64)

Windows에서 nShiftSpace를 **32비트·64비트 모두** 빌드할 수 있는 개발환경을 처음부터
구성하는 방법을 기록한다. 아래 절차대로 실제 설치·빌드해 검증했다 (2026-07-05).

> macOS/Linux를 쓴다면 이 문서가 필요 없다. `apt install gcc-mingw-w64` 또는
> `brew install mingw-w64` 후 `make` 한 번이면 x64/x86이 함께 나온다 (README 방법 1).

## 한눈에 보기

| 도구 | 역할 | 설치 방법 |
|---|---|---|
| Chocolatey | Windows 패키지 관리자 (MSYS2 설치용) | 사전 설치 필요 |
| **MSYS2** | Unix 셸 + pacman + 양쪽 MinGW 툴체인 | `choco install msys2` |
| `mingw-w64-x86_64-gcc` | **64비트** 컴파일러/windres (`/mingw64`) | pacman |
| `mingw-w64-i686-gcc` | **32비트** 컴파일러/windres (`/mingw32`) | pacman |
| `make` | Makefile 실행 | pacman |

결과물: `dist/nShiftSpace-x64.exe`, `dist/nShiftSpace-x86.exe` — 각 **4,608바이트**.

---

## 왜 MSYS2인가 — MinGW 단일 패키지의 한계

`choco install mingw` 으로 설치되는 mingw-builds 패키지는 **64비트 전용**이다.
직접 검증한 결과:

- `gcc -dumpmachine` → `x86_64-w64-mingw32` (64비트 타깃 고정)
- `gcc -m32` → **실패** (`skipping incompatible ... libmingw32.a`, 32비트 라이브러리 없음 — 멀티립 빌드가 아님)
- `bin/`에 `i686-w64-mingw32-gcc` 없음 → 32비트 컴파일 불가

즉 **MinGW 패키지 하나로는 32비트를 만들 수 없다.** 32비트가 필요하면 별도 i686 툴체인을
추가해야 하며, MSYS2는 pacman으로 x86_64·i686 두 툴체인 + make + Unix 셸(Makefile이 요구하는
`mkdir -p`, `windres` 등)을 한 번에 제공해 가장 깔끔하다. README가 안내하는 UCRT64/MINGW32 셸
구분과도 정확히 일치한다.

> 이미 64비트 전용 `choco install mingw` 를 깔아 두었다면 먼저 제거한다 —
> PATH의 `gcc.exe`가 MSYS2 것과 충돌한다.
> ```powershell
> choco uninstall mingw make -y
> ```

---

## 설치 절차

### 0. 사전 준비

- **관리자 권한 PowerShell** (choco 설치·PATH 변경에 필요)
- **Chocolatey** — 없으면 [chocolatey.org/install](https://chocolatey.org/install) 참고

### 1. MSYS2 설치

```powershell
choco install msys2 -y
```

`C:\tools\msys64` 에 설치되고 최초 시스템 업그레이드까지 자동으로 수행된다.

### 2. 양쪽 툴체인 + make 설치 (pacman)

MSYS2의 bash로 패키지 DB를 갱신하고 필요한 것만 설치한다:

```powershell
& "C:\tools\msys64\usr\bin\bash.exe" -lc "pacman -Sy --noconfirm && pacman -S --needed --noconfirm mingw-w64-x86_64-gcc mingw-w64-i686-gcc make binutils"
```

설치되는 것:

- `mingw-w64-x86_64-gcc` → `/mingw64/bin` (64비트 gcc, windres 등)
- `mingw-w64-i686-gcc` → `/mingw32/bin` (32비트 gcc, windres 등)
- `make`, `binutils` → `/usr/bin`

> **처음 pacman 실행 시** 키링/DB 문제로 실패하면 `pacman -S --noconfirm msys2-keyring`
> 후 재시도한다. `pacman -Syu`(전체 업그레이드)는 셸을 닫으라고 요구할 수 있으므로,
> 위처럼 `pacman -Sy`(DB만 갱신) + 개별 설치를 권장한다.

---

## 빌드

핵심은 **아키텍처별로 PATH를 격리**하는 것이다. `/mingw64/bin`과 `/mingw32/bin`을
동시에 PATH에 두면 `gcc`가 어느 쪽인지 모호해지므로, 각 빌드에서 한쪽만 노출한다.

PowerShell에서 MSYS2 bash를 통해 (`<사용자>`는 실제 경로로 교체):

```powershell
$bash = "C:\tools\msys64\usr\bin\bash.exe"

# 64비트
& $bash -lc "cd /c/Users/<사용자>/Projects/nexa-shortcut && PATH=/mingw64/bin:/usr/bin make x64 CC64=gcc RES64=windres"

# 32비트
& $bash -lc "cd /c/Users/<사용자>/Projects/nexa-shortcut && PATH=/mingw32/bin:/usr/bin make x86 CC32=gcc RES32=windres"
```

또는 MSYS2 셸을 직접 열어서 (README 방식):

- **MINGW64 셸** → `make x64 CC64=gcc RES64=windres`
- **MINGW32 셸** → `make x86 CC32=gcc RES32=windres`

> **`make`(인자 없는 기본 타깃)를 쓰지 않는 이유:** Makefile의 기본값은
> `x86_64-w64-mingw32-gcc` 같은 **접두어 이름**을 기대하는데(리눅스 크로스 컴파일 기준),
> MSYS2의 각 셸은 접두어 없는 `gcc`/`windres`를 제공한다. 그래서 위처럼
> `CC*/RES*` 변수를 덮어써서 아키텍처별로 나눠 빌드한다.

---

## 검증

빌드 산출물의 크기·아키텍처를 확인한다 (초경량 원칙: 각 exe ≤ 8KB):

```powershell
Get-ChildItem dist\*.exe | ForEach-Object {
  $b = [IO.File]::ReadAllBytes($_.FullName)
  $pe = [BitConverter]::ToInt32($b,0x3C); $m = [BitConverter]::ToUInt16($b,$pe+4)
  $arch = @{0x8664='x64';0x14c='x86'}[$m]
  "{0,-22} {1,6} bytes  {2}" -f $_.Name, $_.Length, $arch
}
```

기대 결과:

```
nShiftSpace-x64.exe      4608 bytes  x64
nShiftSpace-x86.exe      4608 bytes  x86
```

CI(GitHub Actions)도 동일 결과를 검증한다 — `ubuntu-latest`에서 `gcc-mingw-w64`로 x64/x86을
빌드하고, 8KB를 초과하면 빌드를 실패시킨다. 상세는 [.github/workflows/build.yml](../.github/workflows/build.yml).

---

## 문제 해결

| 증상 | 원인 / 해결 |
|---|---|
| `gcc: command not found` | PATH에 해당 아키텍처 bin이 없음. `PATH=/mingw64/bin:/usr/bin`(또는 mingw32) 지정 |
| `-m32` 링크 실패 | 64비트 전용 mingw로 32비트를 만들려는 것. i686 툴체인(위 pacman) 필요 |
| `mkdir: invalid option` 등 | Windows cmd/PowerShell에서 make를 직접 돌린 경우. Makefile은 `mkdir -p`를 쓰므로 **MSYS2 bash**에서 실행해야 함 |
| `make_icon.py` / python 오류 | 아이콘 재생성 시에만 필요. `res/nShiftSpace.ico`는 저장소에 포함되어 있으므로, `touch res/nShiftSpace.ico`로 타임스탬프를 올려 재생성을 건너뛰면 됨 |
| PATH 변경이 반영 안 됨 | choco 설치 직후 셸을 다시 열거나 `refreshenv` 실행 |

---

## 대안 — Visual Studio / MSVC (x64 전용)

GNU 툴체인 없이 MSVC로도 빌드할 수 있으나 **x64만** 나온다. **x64 Native Tools Command
Prompt for VS** 에서 저장소 폴더로 이동 후 `build.bat` 실행. 상세는 [README.md](../README.md).

---

## 참고

- 빌드 명령 요약: [README.md](../README.md) "빌드" 섹션
- 크기 최소화 플래그·초경량 원칙: [docs/DESIGN.md](DESIGN.md)
- 패키지 배포(Chocolatey/winget): [docs/PACKAGING.md](PACKAGING.md)
