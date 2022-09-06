// Test program for new list printing / data structures.

#include <stdio.h>
#include <conio.h>
#include <cbm.h>
#include "zsound/zsmplayer.h"

extern void __fastcall__ init();

const char* name[4]= { "main.zsm", "levela.zsm", "lose.zsm", "win.zsm" };

struct {
  char bank[4];
  char* addr[4];
  char current;
} zsm;

//extern void __fastcall__ vsync();

char* bload(const char* filename,const void* address) {
  // loads with current HiRAM bank = the one we want.
  // Even if it's not, it's the one we're gonna get! :)
	cbm_k_setnam(filename);
	cbm_k_setlfs(0,8,2);
	return (char*)cbm_k_load(0,(uint16_t)address);
}

void nextsong() {
  if (zsm.current >= 3) zsm.current = 2;
  ++zsm.current;
  zsm.current %= 3;
  printf ("playing %s\n",name[zsm.current]);
  zsm_startmusic(zsm.bank[zsm.current], (uint16_t)zsm.addr[zsm.current]);
}

void playsong(char id) {
  zsm.current = id;
  printf ("playing %s\n",name[zsm.current]);
  zsm_startmusic(zsm.bank[zsm.current], (uint16_t)zsm.addr[zsm.current]);
}

#define NOPP \
asm("NOP");  \
asm("NOP");  \
asm("NOP");  \
asm("NOP");  \
asm("NOP");
// removed 7

void writeYM(uint8_t reg, uint8_t val) {
  while (YM2151.status & 0x80) {}
  NOPP
  YM2151.reg = reg;
  NOPP
  YM2151.data = val;
}

void oldinit() {
  int i;
  for (i=0xe0;i<0x100;i++) writeYM(i,0x0f);
  for (i=0;i<8;i++) writeYM(0x08,i);
  writeYM(1,2);
  writeYM(0x18,0);
  writeYM(1,0);
  writeYM(0x19,0x80);
  for (i=0x0f;i<0x100;i++) writeYM(i,0);
}

void main() {
  char playing=0;
  char key;
  zsm_init();
  zsm.current=2;
  zsm.bank[0]=1;
  zsm.addr[0]=(char*)0xa000;
  RAM_BANK=1;
  printf("loading %s\n",name[0]);
  zsm.addr[1]=bload(name[0],(char*)0xa000);
  zsm.bank[1]=RAM_BANK;
  printf("loading %s\n",name[1]);
  zsm.addr[2]=bload(name[1],zsm.addr[1]);
  zsm.bank[2]=RAM_BANK;
  printf("loading %s\n",name[2]);
  zsm.addr[3]=bload(name[2],zsm.addr[2]);
  zsm.bank[3]=RAM_BANK;
  printf("loading %s\n",name[3]);
  bload(name[3],zsm.addr[3]);
  printf("done\n");
  printf("press spacebar to advance the music to the next tune.\n");
  printf("press 1 2 or 3 to play a specific song.");
  printf("press q to quit\n");
  while (kbhit()) cgetc(); // clear keyboard buffer
  while(1) {
    waitvsync();
    zsm_play();
    if (kbhit()) {
      key=cgetc();
      if (key=='q') break;
      if (key==' ') {
        zsm_stopmusic();
        nextsong();
        playing=1;
      }
      if (key >= '1' && key < '5') {
        key -= '1';
        zsm_stopmusic();
        playsong(key);
        playing=1;
      }
      if (key==20) {
        if(playing) {
          playing=0;
          zsm_stopmusic();
        }
        else {
          playing=1;
          playsong(zsm.current);
        }
      }
      if (key=='i') {
        if (playing) zsm_stopmusic();
        init();
        playing=0;
      }
    }
  }
  zsm_stopmusic();
}
