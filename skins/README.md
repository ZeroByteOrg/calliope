## Calliope Skins

Calliope supports skin (.SK) files. A skin is comprised of a background image,
a font, sprites for the animated LEDs, and the palettes from those images.

**bg.png:**

This is the background graphics. It should be 640x420 pixels resolution, and
be 4bpp indexed color. The palette should be limited to exactly 16 colors.
Color index 0 from the background image will be written into the system's
palette entry zero.

**font.png:**

This should be a set of 8x8 characters which will be packed into 2bpp format.
The palette of the image should have 32 colors defined. Colors 0-3 will be
the primary color palette for text, and colors $10-$1F will be used for
a highlight color. Currently, only the first 4 colors are used but in the
future, the remaining groups of colors will be used for palette animations
to make an item flash colors or fade between dark and bright versions of
the base color.

**leds.png:**

This should be 16 versions of an individual LED (8x8 resolution, 4bpp color)
ranging from off state (0) to maximum brightness (15). The palette should
contain 32 colors. The first 16 are the "on" color, and the remaining 16 are
the "off/inactive" color.

**intro.zsm**

The Main project's Makefile will copy this file into the build directory as
OPENING.BIN and play it when the program first launches. The file is named
this in order to hide it from the UI during operation.

Future updates will most likely see OPENING.BIN shift to being embedded within
the SK file itself.


