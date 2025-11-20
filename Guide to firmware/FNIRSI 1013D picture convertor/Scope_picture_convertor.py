#!/usr/bin/env python3
import struct
import glob
import os

OUTPUT = "scope.bmp"

# -----------------------------------------------------
# ULOŽENÁ HLAVIČKA Z FUNKČNÉHO scope.bmp (presne 70 bajtov)
# -----------------------------------------------------
SCOPE_HEADER = bytes([
    0x42,0x4D,          # "BM"
    0x46,0xB8,0x0B,0x00,# file size 768070
    0x00,0x00,
    0x00,0x00,
    0x46,0x00,0x00,0x00,# pixel offset 70
    # DIB header (40 bytes)
    0x28,0x00,0x00,0x00,# header size 40
    0x20,0x03,0x00,0x00,# width = 800
    0x20,0xFE,0xFF,0xFF,# height = -480 (top-down)
    0x01,0x00,          # planes
    0x10,0x00,          # bpp = 16
    0x03,0x00,0x00,0x00,# BI_BITFIELDS
    0x00,0xB8,0x0B,0x00,# image size 768000
    0x00,0x00,0x00,0x00,# xppm
    0x00,0x00,0x00,0x00,# yppm
    0x00,0x00,0x00,0x00,# colors
    0x00,0x00,0x00,0x00,# important colors
    # RGB masks (3×4 B)
    0x00,0xF8,0x00,0x00,# red mask
    0xE0,0x07,0x00,0x00,# green mask
    0x1F,0x00,0x00,0x00 # blue mask
])

# -----------------------------------------------------
def convert_24_to_16(rgb_bytes, width=800, height=480):
    """Konverzia 24-bit BMP → RGB565 s otočením riadkov"""
    row_size = width * 3
    out = bytearray()
    for y in range(height):
        start = (height - 1 - y) * row_size
        row = rgb_bytes[start:start+row_size]
        for i in range(0, len(row), 3):
            b = row[i]
            g = row[i+1]
            r = row[i+2]
            rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
            out += struct.pack("<H", rgb565)
    return out

# -----------------------------------------------------
def convert_16bit(rgb_bytes, width=800, height=480):
    """Otočenie 16-bit BMP (RGB565)"""
    row_size = width * 2
    out = bytearray()
    for y in range(height):
        start = (height - 1 - y) * row_size
        row = rgb_bytes[start:start+row_size]
        out += row
    return out

# -----------------------------------------------------
def find_input_bmp():
    """Nájde prvý BMP súbor okrem scope.bmp"""
    files = glob.glob("*.bmp")
    for f in files:
        if f.lower() != OUTPUT.lower():
            return f
    return None

# -----------------------------------------------------
def main():
    INPUT = find_input_bmp()
    if not INPUT:
        print("Nenašiel sa žiadny BMP súbor na konverziu!")
        return

    print(f"Konvertujem súbor: {INPUT}")

    with open(INPUT, "rb") as f:
        bmp = f.read()

    pixel_offset = struct.unpack_from("<I", bmp, 10)[0]
    width = struct.unpack_from("<I", bmp, 18)[0]
    height_signed = struct.unpack_from("<i", bmp, 22)[0]
    bpp = struct.unpack_from("<H", bmp, 28)[0]
    height = abs(height_signed)

    if width != 800 or height != 480:
        raise RuntimeError("Neočakávané rozmery obrázka (musí byť 800x480)")

    pixel_data = bmp[pixel_offset:]

    if bpp == 24:
        rgb565 = convert_24_to_16(pixel_data, width, height)
    elif bpp == 16:
        rgb565 = convert_16bit(pixel_data, width, height)
    else:
        raise RuntimeError(f"Neznámy bpp {bpp}, podporujeme iba 16 alebo 24-bit BMP")

    if len(rgb565) != width*height*2:
        raise RuntimeError("Zlá dĺžka RGB565 dát")

    with open(OUTPUT, "wb") as f:
        f.write(SCOPE_HEADER)
        if len(SCOPE_HEADER) < 70:
            f.write(b'\x00' * (70 - len(SCOPE_HEADER)))
        f.write(rgb565)

    print("Hotovo! Vytvorený súbor:", OUTPUT)

# -----------------------------------------------------
if __name__ == "__main__":
    main()

