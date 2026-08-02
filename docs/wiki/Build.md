# 직접 빌드하기

소스는 [`src/nShiftSpace.c`](https://github.com/SosomLab/nexa-shortcut/blob/main/src/nShiftSpace.c)
한 파일입니다. 아래 3가지 방법 중 환경에 맞는 것을 고르세요.

결과물: `dist/nShiftSpace-x64.exe`, `dist/nShiftSpace-x86.exe` — 각 **4,608바이트**

| 방법 | 환경 | x64 | x86 |
|---|---|---|---|
| [MinGW 크로스 컴파일](#방법-1--macos--linux-크로스-컴파일) | macOS / Linux | ✅ | ✅ |
| [MSYS2](#방법-2--windows--msys2--mingw-w64) | Windows | ✅ | ✅ |
| [MSVC](#방법-3--windows--visual-studio--msvc) | Windows | ✅ | ❌ |

---

## 방법 1 — macOS / Linux 크로스 컴파일

가장 간단합니다. 한 번에 x64와 x86이 모두 나옵니다.

```bash
brew install mingw-w64        # macOS
# sudo apt install gcc-mingw-w64   # Debian / Ubuntu

git clone https://github.com/SosomLab/nexa-shortcut.git
cd nexa-shortcut
make
```

---

## 방법 2 — Windows / MSYS2 (MinGW-w64)

**MSYS2 UCRT64** 셸에서:

```bash
make x64 CC64=gcc RES64=windres
```

32비트는 **MSYS2 MINGW32** 셸에서:

```bash
make x86 CC32=gcc RES32=windres
```

> 인자 없는 `make`를 쓰지 않는 이유: Makefile의 기본값은 `x86_64-w64-mingw32-gcc` 같은
> **접두어 이름**을 기대하는데(리눅스 크로스 컴파일 기준), MSYS2의 각 셸은 접두어 없는
> `gcc`/`windres`를 제공하기 때문입니다.

> ⚠️ `choco install mingw`로 설치되는 패키지는 **64비트 전용**이라 32비트를 만들 수 없습니다.
> 양쪽이 다 필요하면 MSYS2를 쓰세요.

MSYS2 설치부터 양쪽 툴체인 구성까지의 상세 절차는
[docs/DEV-ENV-WINDOWS.md](https://github.com/SosomLab/nexa-shortcut/blob/main/docs/DEV-ENV-WINDOWS.md)에 있습니다.

---

## 방법 3 — Windows / Visual Studio (MSVC)

**x64 Native Tools Command Prompt for VS** 에서 저장소 폴더로 이동 후:

```bat
build.bat
```

MSVC에서도 CRT를 링크하지 않으므로(`/NODEFAULTLIB` + `/ENTRY:start`) 초경량 결과물이 나옵니다.
단, **x64만** 생성됩니다.

---

## 결과물 검증

초경량 원칙상 **exe 하나당 8KB 이내**여야 합니다. PowerShell에서:

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

GitHub Actions CI도 동일하게 검증하며, **8KB를 초과하면 빌드를 실패**시킵니다.

---

## 초경량이 유지되는 이유

| 기법 | 효과 |
|---|---|
| `-nostdlib`, 진입점 `start` | CRT 미링크 — 수십 KB 절약 |
| kernel32 / user32 / shell32 만 링크 | import 테이블 최소화 |
| 큰 구조체를 전역(`.bss`)에 배치 | 컴파일러의 `memset` 호출 생성 회피 |
| 16×16 1bpp 단일 이미지 ICO (198바이트) | 다중 해상도 트루컬러 ICO는 수십 KB |
| `RegisterHotKey` 사용 (훅 아님) | 코드 단순 + CPU 비용 0 |

설계 배경은
[docs/DESIGN.md](https://github.com/SosomLab/nexa-shortcut/blob/main/docs/DESIGN.md) 참고.

## 빌드 문제 해결

| 증상 | 원인 / 해결 |
|---|---|
| `gcc: command not found` | PATH에 해당 아키텍처 bin이 없음. `PATH=/mingw64/bin:/usr/bin` (또는 mingw32) 지정 |
| `-m32` 링크 실패 | 64비트 전용 mingw로 32비트를 만들려는 것. i686 툴체인 필요 |
| `mkdir: invalid option` | Makefile은 `mkdir -p`를 쓰므로 **MSYS2 bash**에서 실행해야 합니다 |
| `make_icon.py` / python 오류 | 아이콘 재생성 시에만 필요. `res/nShiftSpace.ico`는 저장소에 포함되어 있으므로 `touch res/nShiftSpace.ico`로 재생성을 건너뛰면 됩니다 |
