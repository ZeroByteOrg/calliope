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

unsigned long songsize[NUM_SONGS] = {
  187886,113061,96712,69166,101333,146480,138774,1706,910
};

const char* dir[NUM_QUALITY] = {
  "zcmorig", "zcm22k16", "zcm8k16", "zcm22k8", "zcm16k8", "zcm8k8"
//  "zcm8k16", "zcm22k8"
};
const char* qual[NUM_QUALITY] = {
  "48khz 16bit", "22khz 16bit", "8khz 16bit", "22khz 8bit", "16khz 8bit",
  "8khz 8bit"
//  "8khz 16bit", "22khz 8bit"
};

const char* qualshort[NUM_QUALITY] = {
  "48khz", "22khz", " 8khz", "22khz", "16khz", " 8khz"
};

const char* zsmname[NUM_SONGS] = {
  "main.zsm", "levela.zsm", "levelb.zsm", "levelc.zsm",
  "leveld.zsm", "levele.zsm", "levelf.zsm", "win.zsm",
  "lose.zsm"
};

const char* songname[NUM_SONGS] = {
  "main theme", "level a", "level b", "level c", "level d",
  "level e", "level f", "you win", "you lose"
};

char* blank = NULL;

void print_list(itemlist* list, char start, char count) {
  char i, x;
  x = wherex();
  for (i=start ; i<start+count ; i++) {
    if (i<list->count) {
      revers(i==list->active);
      cprintf(itemstr,list->name[i]);
      revers(0);
    }
    else {
      cprintf(itemstr,blank);
    }
    gotox(x);
  }
}

void draw_files() {
  #define MM 12
  gotoxy(2,6);
  if (files.active < files.scroll)
    files.scroll = files.active;
  if (files.active - files.scroll >= MM)
    files.scroll = files.active-MM+1;
  print_list(&files,files.scroll,MM);
  gotoxy(10,29);
  cprintf("scroll:%02u  active=%02u",files.scroll,files.active);
}

void print_songlist() {
  #define SONGX 42
  #define SONGY 10
  #define SONGNAME "%-12s"

  char i;

  for (i=0 ; i<NUM_SONGS ; i++) {
    gotoxy(SONGX,SONGY+i);
    cprintf("%u: ",i+1);
    if (i==current_song) {
      revers(1);
      if (music_playing)
        textcolor(COLOR_LIGHTGREEN);
      if (music_loading)
        textcolor(COLOR_ORANGE);
      cprintf(SONGNAME,songname[i]);
      cprintf(" %6lu bytes",songsize[i]);
      revers(0);
      if (music_loading)
        cprintf(" loading");
      else
        cprintf("        ");
      textcolor(COLOR_WHITE);
    }
    else {
      cprintf(SONGNAME,songname[i]);
      cprintf(" %6lu bytes",songsize[i]);
    }
    revers(0);
    textcolor(COLOR_WHITE);
  }
  gotoxy(SONGX,wherey()+2);
  cprintf("press backspace to toggle music");
}

void screen_init() {
  unsigned char i;
  videomode(VIDEOMODE_80x30);
  clrscr();
  cbm_k_bsout(CH_FONT_UPPER);
  gotoxy (0,0);
  cprintf("calliope music player");
  gotoxy (0,1);
  for (i=0 ; i<80 ; i++) cputc('-');
  if (blank == NULL) {
    blank = malloc(LIST_ITEM_SIZE+1);
    memset(blank,' ',LIST_ITEM_SIZE);
    blank[LIST_ITEM_SIZE]=0;
  }
  sprintf(itemstr," %%-%us \n",LIST_ITEM_SIZE);
  itemstr[8]=0;
  gotoxy(0,3);
  cprintf("directory: /");
}
