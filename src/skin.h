#ifndef _skin_h_
#define _skin_h_

// move these to a layout definition .h file of some sort....
#define VRAM_BG      0x00000L
#define VRAM_LEDS    0x14000L
#define VRAM_FONT    0x13000L
#define VRAM_PAL     0x1FA20L

#define SK_ADDR_BG    (char*)0xA000
#define SK_BANK_BG    1
#define SK_BYTES_BG   76800L

#define SK_ADDR_LEDS  (char*)0xAC00
#define SK_BANK_LEDS  10
#define SK_BYTES_LEDS 512L

#define SK_ADDR_FONT  (char*)0xAE00
#define SK_BANK_FONT  10
#define SK_BYTES_FONT 2048L

#define SK_ADDR_PAL   (char*)0xB600
#define SK_BANK_PAL   10
#define SK_BYTES_PAL  160L

#define SK_TEXT_NORMAL 4
#define SK_TEXT_WARN   5


extern void load_skin(const char* skinfile);

#endif
