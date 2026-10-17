# nexa-shortcut — macOS/Linux에서 mingw-w64로 Windows용 크로스 빌드
#
#   make          # 일반(x64 + x86) + LED(x64 + x86) 모두 빌드
#   make x64      # 64비트 일반판만
#   make x86      # 32비트 일반판만 (더 작음, 모든 Windows에서 동작)
#   make led      # 키보드 LED 표시판 (x64 + x86) — -DNSS_LED
#   make led-x64  # LED 64비트만 (MSYS2처럼 셸별로 나눠 빌드할 때)
#   make led-x86  # LED 32비트만
#
# 일반판과 LED판은 같은 소스(src/nShiftSpace.c)에서 NSS_LED 매크로로 갈라진다.
# 일반판에는 LED 코드·켜짐 아이콘·Raw Input import 가 전혀 포함되지 않는다.

CC64 := x86_64-w64-mingw32-gcc
CC32 := i686-w64-mingw32-gcc
RES64 := x86_64-w64-mingw32-windres
RES32 := i686-w64-mingw32-windres

# CRT 미링크 + 크기 최소화 플래그
CFLAGS := -Os -s -mwindows -nostdlib -DUNICODE -D_UNICODE \
          -fno-ident -fno-asynchronous-unwind-tables \
          -fno-stack-protector -fno-stack-check \
          -finput-charset=UTF-8
# advapi32 는 "로그인 시 자동 실행"(HKCU Run 값) 전용
LIBS   := -lkernel32 -luser32 -lshell32 -ladvapi32

# LED판: 컴파일러·windres 양쪽에 같은 매크로를 준다 (rc의 #ifdef로 켜짐 아이콘 포함)
LEDDEF := -DNSS_LED

SRC := src/nShiftSpace.c
RC  := res/nShiftSpace.rc
ICO := res/nShiftSpace.ico
ICO_ON := res/nShiftSpace-on.ico

all: x64 x86 led

x64: dist/nShiftSpace-x64.exe
x86: dist/nShiftSpace-x86.exe
led: led-x64 led-x86
led-x64: dist/nShiftSpace-led-x64.exe
led-x86: dist/nShiftSpace-led-x86.exe

dist build:
	mkdir -p $@

$(ICO): tools/make_icon.py
	python3 tools/make_icon.py $@

$(ICO_ON): tools/make_icon.py
	python3 tools/make_icon.py $@ on

# --- 리소스 (일반) ---
build/rsrc-x64.o: $(RC) $(ICO) | build
	$(RES64) --include-dir res $(RC) -O coff -o $@

build/rsrc-x86.o: $(RC) $(ICO) | build
	$(RES32) --include-dir res $(RC) -O coff -o $@

# --- 리소스 (LED: 켜짐 아이콘 추가) ---
build/rsrc-led-x64.o: $(RC) $(ICO) $(ICO_ON) | build
	$(RES64) --include-dir res $(LEDDEF) $(RC) -O coff -o $@

build/rsrc-led-x86.o: $(RC) $(ICO) $(ICO_ON) | build
	$(RES32) --include-dir res $(LEDDEF) $(RC) -O coff -o $@

# --- 실행 파일 (일반) ---
dist/nShiftSpace-x64.exe: $(SRC) build/rsrc-x64.o | dist
	$(CC64) $(CFLAGS) -Wl,-e,start $(SRC) build/rsrc-x64.o -o $@ $(LIBS)

dist/nShiftSpace-x86.exe: $(SRC) build/rsrc-x86.o | dist
	$(CC32) $(CFLAGS) -Wl,-e,_start $(SRC) build/rsrc-x86.o -o $@ $(LIBS)

# --- 실행 파일 (LED) ---
dist/nShiftSpace-led-x64.exe: $(SRC) build/rsrc-led-x64.o | dist
	$(CC64) $(CFLAGS) $(LEDDEF) -Wl,-e,start $(SRC) build/rsrc-led-x64.o -o $@ $(LIBS)

dist/nShiftSpace-led-x86.exe: $(SRC) build/rsrc-led-x86.o | dist
	$(CC32) $(CFLAGS) $(LEDDEF) -Wl,-e,_start $(SRC) build/rsrc-led-x86.o -o $@ $(LIBS)

clean:
	rm -rf dist build

.PHONY: all x64 x86 led led-x64 led-x86 clean
