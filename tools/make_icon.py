#!/usr/bin/env python3
"""nShiftSpace 초경량 아이콘 생성기.

16x16, 1bpp(2색: 파랑 바탕 + 흰 '한' 픽셀아트) 단일 이미지 ICO를 생성한다.
결과물은 약 198바이트 — 다중 해상도/트루컬러 ICO(수십 KB) 대신 크기 최소화.
사용법: python3 tools/make_icon.py res/nShiftSpace.ico
"""
import struct
import sys

# 16x16 픽셀 맵: '#' = 흰색(전경), '.' = 파랑(배경)
# 디자인: '가'(좌상) + 'A'(우하) 대각 배치 — Windows IME 표시(가/A) 관례
# 외곽 1px은 여백으로 비워둔다 (rows/cols 1–14 안에만 그림)
PIXELS = [
    "................",
    ".######.#.......",
    "......#.#.......",
    "......#.#.......",
    "......#.###.....",
    "......#.#.......",
    "......#.#.......",
    "......#.#.......",
    "........#..#....",
    "..........#.#...",
    "..........#.#...",
    ".........#...#..",
    ".........#####..",
    "........#.....#.",
    "........#.....#.",
    "................",
]

BG = (0xEB, 0x63, 0x25)  # BGR — 파랑 #2563EB
FG = (0xFF, 0xFF, 0xFF)  # 흰색

W = H = 16


def build_ico() -> bytes:
    # XOR 비트맵: 1bpp, 행은 32비트 경계 패딩, 아래→위 순서
    xor = bytearray()
    for row in reversed(PIXELS):
        bits = 0
        for x, ch in enumerate(row):
            if ch == "#":
                bits |= 1 << (15 - x)  # MSB 우선, 팔레트 인덱스 1 = 전경
        xor += struct.pack(">H", bits) + b"\x00\x00"  # 2바이트 + 2바이트 패딩

    # AND 마스크: 전부 0 = 완전 불투명 (투명 픽셀 없음 → 합성 비용도 없음)
    and_mask = bytes(4 * H)

    palette = bytes(BG) + b"\x00" + bytes(FG) + b"\x00"  # RGBQUAD x 2

    # BITMAPINFOHEADER (높이는 XOR+AND 합산이라 2배)
    bih = struct.pack("<IiiHHIIiiII", 40, W, H * 2, 1, 1, 0,
                      len(xor) + len(and_mask), 0, 0, 0, 0)

    image = bih + palette + bytes(xor) + and_mask

    icondir = struct.pack("<HHH", 0, 1, 1)
    entry = struct.pack("<BBBBHHII", W, H, 2, 0, 1, 1, len(image), 22)
    return icondir + entry + image


if __name__ == "__main__":
    out = sys.argv[1] if len(sys.argv) > 1 else "res/nShiftSpace.ico"
    data = build_ico()
    with open(out, "wb") as f:
        f.write(data)
    print(f"{out}: {len(data)} bytes")
