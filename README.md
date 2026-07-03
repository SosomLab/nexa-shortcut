# nexa-shortcut

윈도우용 초경량 단축키 유틸리티 모음. GUI 프레임워크와 CRT 없이 순수 Win32 API로만 작성해
실행 파일 크기와 메모리 사용을 최소화한다.

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

1. MSYS2 설치 — Windows 기본 패키지 관리자 winget 사용 (또는 [msys2.org](https://www.msys2.org/)에서 설치 파일 다운로드):

```powershell
winget install MSYS2.MSYS2
```

2. 시작 메뉴에서 **MSYS2 UCRT64** 셸 실행 후 도구 설치 및 빌드:

```bash
pacman -S mingw-w64-ucrt-x86_64-toolchain make python
make x64 CC64=gcc RES64=windres    # dist/nShiftSpace-x64.exe 생성
```

32비트 빌드는 **MSYS2 MINGW32** 셸에서 `pacman -S mingw-w64-i686-toolchain` 후
`make x86 CC32=gcc RES32=windres`.

#### 방법 3 — Windows에서 컴파일 (Visual Studio / MSVC)

1. Visual Studio Build Tools + C++ 워크로드 설치 — winget 사용 (이미 Visual Studio에
   "C++ 데스크톱 개발" 워크로드가 있다면 생략):

```powershell
winget install Microsoft.VisualStudio.2022.BuildTools --override "--wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
```

2. 시작 메뉴에서 **x64 Native Tools Command Prompt for VS** 실행
3. 저장소 폴더에서:

```bat
build.bat               # dist\nShiftSpace-x64.exe 생성
```

MSVC에서도 CRT를 링크하지 않으므로(/NODEFAULTLIB + /ENTRY:start) 초경량 결과물이 나온다.

### 시작 프로그램 등록

`Win+R` → `shell:startup` → 폴더에 exe 바로가기를 넣으면 로그인 시 자동 실행.

## 로드맵

- **목표 2 — nexa-mapper**: `mappings.ini`로 사용자가 직접 정의하는 단순 키 재매핑 엔진
- **목표 3 — exe 내보내기**: 현재 매핑만 내장한 독립 exe 생성 (스텁 복사 + 설정 덧붙이기 방식)

상세 설계는 [docs/DESIGN.md](docs/DESIGN.md), 변경 이력은 [docs/CHANGELOG.md](docs/CHANGELOG.md) 참고.

## 라이선스

[MIT License](LICENSE) — 누구나 무료로 사용·복사·수정·배포할 수 있습니다.
