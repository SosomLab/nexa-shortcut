# nexa-shortcut — macOS/Linux에서 mingw-w64로 Windows용 크로스 빌드
#
#   make          # x64 + x86 모두 빌드
#   make x64      # 64비트만
#   make x86      # 32비트만 (더 작음, 모든 Windows에서 동작)

CC64 := x86_64-w64-mingw32-gcc
CC32 := i686-w64-mingw32-gcc
RES64 := x86_64-w64-mingw32-windres
RES32 := i686-w64-mingw32-windres

# CRT 미링크 + 크기 최소화 플래그
CFLAGS := -Os -s -mwindows -nostdlib -DUNICODE -D_UNICODE \
          -fno-ident -fno-asynchronous-unwind-tables \
          -fno-stack-protector -fno-stack-check \
          -finput-charset=UTF-8
LIBS   := -lkernel32 -luser32 -lshell32

SRC := src/nShiftSpace.c
RC  := res/nShiftSpace.rc
ICO := res/nShiftSpace.ico

all: x64 x86

x64: dist/nShiftSpace-x64.exe
x86: dist/nShiftSpace-x86.exe

dist build:
	mkdir -p $@

$(ICO): tools/make_icon.py
	python3 tools/make_icon.py $@

build/rsrc-x64.o: $(RC) $(ICO) | build
	$(RES64) --include-dir res $(RC) -O coff -o $@

build/rsrc-x86.o: $(RC) $(ICO) | build
	$(RES32) --include-dir res $(RC) -O coff -o $@

dist/nShiftSpace-x64.exe: $(SRC) build/rsrc-x64.o | dist
	$(CC64) $(CFLAGS) -Wl,-e,start $(SRC) build/rsrc-x64.o -o $@ $(LIBS)

dist/nShiftSpace-x86.exe: $(SRC) build/rsrc-x86.o | dist
	$(CC32) $(CFLAGS) -Wl,-e,_start $(SRC) build/rsrc-x86.o -o $@ $(LIBS)

clean:
	rm -rf dist build

.PHONY: all x64 x86 clean
