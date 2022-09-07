#!/usr/bin/env python3

from PIL import Image
import argparse

parser = argparse.ArgumentParser(description="Convert indexed image formats to raw tile data.")
parser.add_argument("-s", "--size", choices=["8x8", "8x16", "16x8", "16x16", "32x32", "320x240", "640x480", "640x240" ], default="8x8", help="Size of individual tiles. Defaults to 8x8.")
parser.add_argument("-b", "--bpp", choices=["2","4","8"], default=4, help="bpp depth of input file. Defaults to 4bpp")
parser.add_argument("-n", "--noheader", help="Do not put a 2-byte PRG header at the beginning of the output file", dest='skipheader', default=False, action='store_true')
parser.add_argument("input", help="Input file.")
parser.add_argument("output", help="Output file.")


args = parser.parse_args()

tile_x_size, tile_y_size = args.size.split("x")
tile_x_size, tile_y_size = int(tile_x_size), int(tile_y_size)
bpp = int(args.bpp)

if args.input == args.output:
    print("error: Input file can't be the same as the output")
    exit(1)

try:
    img = Image.open(args.input)
except:
    print("Failed to open input image.")
    exit(1)

if img.mode != "P":
    print("Input image must be in an indexed format.")
    exit(1)

packbytes = (8//bpp)
with open(args.output, mode="wb") as output:
    if not args.skipheader:
        output.write( (0xffff).to_bytes(2, byteorder='little') );
    for y in range(0, img.size[1] // tile_y_size):
        y *= tile_y_size
        for x in range(0, img.size[0] // tile_x_size):
            x *= tile_x_size
            tile = img.crop((x, y, x + tile_x_size, y + tile_y_size))
            data=bytearray(tile.getdata())
            for i in range(0,tile_x_size * tile_y_size * bpp // 8):
                mybyte = 0x00;
                for j in range(0, packbytes):
                    srcbyte=data[(i*(8//bpp))+j] & (2**bpp-1)
                    mybyte=mybyte | (srcbyte << (bpp*(packbytes-j-1)))
                output.write( mybyte.to_bytes(15//8,'little') )

