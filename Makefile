# nexa-shortcut — macOS/Linux에서 mingw-w64로 Windows용 크로스 빌드
#
#   make          # x64 + x86 모두 빌드
#   make x64      # 64비트만
#   make x86      # 32비트만 (더 작음, 모든 Windows에서 동작)

CC64 := x86_64-w64-mingw32-gcc
CC32 := i686-w64-mingw32-gcc

# CRT 미링크 + 크기 최소화 플래그
CFLAGS := -Os -s -mwindows -nostdlib -DUNICODE -D_UNICODE \
          -fno-ident -fno-asynchronous-unwind-tables \
          -fno-stack-protector -fno-stack-check \
          -finput-charset=UTF-8
LIBS   := -lkernel32 -luser32 -lshell32

SRC := src/hangul_toggle.c

all: x64 x86

x64: dist/nexa-hangul-x64.exe
x86: dist/nexa-hangul-x86.exe

dist:
	mkdir -p dist

dist/nexa-hangul-x64.exe: $(SRC) | dist
	$(CC64) $(CFLAGS) -Wl,-e,start $< -o $@ $(LIBS)

dist/nexa-hangul-x86.exe: $(SRC) | dist
	$(CC32) $(CFLAGS) -Wl,-e,_start $< -o $@ $(LIBS)

clean:
	rm -rf dist

.PHONY: all x64 x86 clean
