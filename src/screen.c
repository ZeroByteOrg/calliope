#include "screen.h"
#include "system.h"
#include <conio.h>
#include <cbm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


extern char current_song;
extern char music_playing;
extern char music_loading;

char itemstr[10];

char* blank = NULL;

void print_list(itemlist* list) {
  char i;
  char count = list->len;
  char start=0;
  gotoxy(list->x,list->y);
  if (list->count >= 1) {
    if (list->active < list->scroll)
      list->scroll = list->active;
    if (list->active - list->scroll >= count)
      list->scroll = list->active-count+1;
    start = list->scroll;
  }

  for (i=start ; i<start+count ; i++) {
    if (i<list->count && list->count > 0) {
      if(selected == list)
        revers(i==list->active);
      cprintf(itemstr,list->name[i]);
      revers(0);
    }
    else {
      cprintf(itemstr,blank);
    }
    gotox(list->x);
  }
  gotoxy(10,29);
  cprintf("scroll:%02u  active=%02u",selected->scroll,selected->active);
}

void draw_path() {
  gotoxy(0,2);
  cprintf("directory: %s%s",workdir.root,workdir.path);
  do {
    cprintf(" ");
    if(wherex()==78) break;
  } while(1);
}

void print_loading(char loading) {
  gotoxy(35,0);
  if (loading) {
    revers(1);
    cprintf("loading");
    revers(0);
  }
  else
    cprintf("       ");
}

void screen_init() {
  unsigned char i;
  videomode(VIDEOMODE_80x30);
  VERA.control |= 0x02; // DCSEL=1
  VERA.display.hstart=8>>2;
  VERA.display.hstop=(640-8)>>2;
  VERA.control &= ~0x02; // DCSEL=0
  clrscr();
  cbm_k_bsout(CH_FONT_UPPER);
  gotoxy (0,0);
  cprintf("calliope music player");
  gotoxy (0,1);
  for (i=0 ; i<78 ; i++) cputc('-');
  if (blank == NULL) {
    blank = malloc(LIST_ITEM_SIZE+1);
    memset(blank,' ',LIST_ITEM_SIZE);
    blank[LIST_ITEM_SIZE]=0;
  }
  sprintf(itemstr," %%-%us \n",LIST_ITEM_SIZE);
  itemstr[8]=0;
  draw_path();
}
