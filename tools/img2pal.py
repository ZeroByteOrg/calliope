#!/usr/bin/env python3

from PIL import Image
import argparse
import numpy as np

parser = argparse.ArgumentParser(description="Extract palette from indexed images or build one from RGB images.")
parser.add_argument("-p", "--prgheader", help="Put a 2-byte PRG header of 0xFFFF at the beginning of the output file", dest='putheader', default=False, action='store_true')
parser.add_argument("input", help="Input file.")
parser.add_argument("output", help="Output file.")
#leaving these relics from img2tileset.py as examples for myself later....
#parser.add_argument("-s", "--size", choices=["8x8", "8x16", "16x8", "16x16", "32x32", "320x240", "640x480", "640x240" ], default="8x8", help="Size of individual tiles. Defaults to 8x8.")
#parser.add_argument("-b", "--bpp", choices=["2","4","8"], default=4, help="bpp depth of input file. Defaults to 4bpp")


args = parser.parse_args()

#tile_x_size, tile_y_size = args.size.split("x")
#tile_x_size, tile_y_size = int(tile_x_size), int(tile_y_size)
#bpp = int(args.bpp)

if args.input == args.output:
    print("error: Input file can't be the same as the output")
    exit(1)

try:
    img = Image.open(args.input)
except:
    print("Failed to open input image.")
    exit(1)

palette = bytearray()

if img.mode == "P":
    print("Extracting palette from indexed image.")
    #p=np.array(img.palette())
    i=0
    for color in img.palette.getdata()[1]:
        color = color & 0xf0
        if(i % 3 == 0):
            red = color >> 4
        if(i % 3 == 1):
            gb = color
        if(i % 3 == 2):
            gb = gb | (color >> 4)
            palette.append(gb)
            palette.append(red)
        i += 1

else:
    i=0
    print("Creating palette from RGBA image.")
    pix = np.array(img)
    for y in range(0, img.size[1]):
        for x in range(0, img.size[0]):
            r,g,b = pix[y][x]
            r = (r & 0xf0) >> 4
            g = (g & 0xf0)
            b = (b & 0xf0) >> 4
            i += 1
            palette.append(g|b)
            palette.append(r)

with open(args.output, mode="wb") as output:
    if args.putheader:
        output.write( (0xffff).to_bytes(2, byteorder='little') );
    output.write(palette)
