#include "skin.h"
#include "files.h"
#include "music.h" // for stopmusic()
#include <cx16.h>
#include <stdio.h>

void generate_reversed_font();
void vram_copy(const char dbank, char* data, const char vbank, const char* vaddr, signed long bytes);
void clear_vram();

void load_skin(const char* skinfile) {
  music_stop(); // we're going to use HiRam to load assets into...
  clear_loaded_song();
  clear_vram();

  RAM_BANK=1;
  bload(skinfile,(void*)0xA000);
  vram_copy(SK_BANK_BG,SK_ADDR_BG,(VRAM_BG>>16),(char*)(VRAM_BG&0xFFFF),SK_BYTES_BG);
  vram_copy(SK_BANK_LEDS,SK_ADDR_LEDS,(VRAM_LEDS>>16),(char*)(VRAM_LEDS&0xFFFF),SK_BYTES_LEDS);
  vram_copy(SK_BANK_FONT,SK_ADDR_FONT,(VRAM_FONT>>16),(char*)(VRAM_FONT&0xFFFF),SK_BYTES_FONT);
  vram_copy(SK_BANK_PAL,SK_ADDR_PAL,(VRAM_PAL>>16),(char*)(VRAM_PAL&0xFFFF),SK_BYTES_PAL);
  generate_reversed_font();
  VERA.address=0xfa00;
  VERA.address_hi=1|VERA_INC_1;
  // copy first byte of palette into system color 0.
  RAM_BANK=SK_BANK_PAL;
  VERA.data0=(SK_ADDR_PAL)[0];
  VERA.data0=(SK_ADDR_PAL)[1];
}

void generate_reversed_font() {
  int i;
  VERA.control = 0;
  VERA.address = VRAM_FONT&0xFFFF;
  VERA.address_hi = (VRAM_FONT>>16) | VERA_INC_1;
  VERA.control = 1;
  VERA.address = (VRAM_FONT+0x800)&0xFFFF;
  VERA.address_hi = (VRAM_FONT>>16) | VERA_INC_1;
  for (i=0;i<SK_BYTES_FONT;i++) {
    VERA.data1 = VERA.data0 ^ 0xff;
  }
  VERA.control = 0;
}

void clear_vram() {
  unsigned long i;
  VERA.address=0;
  VERA.address_hi=VERA_INC_1;

  for (i=0 ; i<0x1AFFF ; i++) VERA.data0=0;
}

void vram_copy(const char dbank, char* data, const char vbank, const char* vaddr, signed long bytes) {

  printf("\n\rvram copy %02x:%04x -> %1x%04x (%05Lx bytes)",
    dbank, (unsigned int)data, vbank, (unsigned int)vaddr, bytes
  );

  VERA.address=(unsigned int)vaddr;
  VERA.address_hi=((vbank & 0x01) | VERA_INC_1);

  RAM_BANK = dbank;
  while (--bytes >= 0) {
    VERA.data0 = *data;
    if (++data >= (char*)0xC000) {
      data=(char*)0xA000;
      ++RAM_BANK;
    }
  }
}
