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

void draw_dirs();
void draw_files();
void draw_playlist();

char itemstr[10];

char* blank = NULL;
itemlist* lastselect = NULL;

void print_list(itemlist* list, char start, char count, char active) {
  char i, x;

  x=wherex();
  for (i=start ; i<start+count ; i++) {
    if (i<list->count && list->count > 0) {
      revers(i == active);
      cprintf(itemstr,list->name[i]);
      revers(0);
    }
    else {
      cprintf(itemstr,blank);
    }
    gotox(x);
  }
  gotoxy(10,29);
  cprintf("scroll:%02u  active=%02u",list->scroll,list->active);
}

void draw_path() {
  gotoxy(0,2);
  cprintf("directory: %s%s",workdir.root,workdir.path);
  do {
    cprintf(" ");
    if(wherex()==78) break;
  } while(1);
}

void draw_dirs() {
  dirs.redraw=0;
  if(dirs.scroll >= dirs.active) dirs.scroll = dirs.active;
  if(dirs.scroll+DIRLIST_LEN <= dirs.active) dirs.scroll=dirs.active-DIRLIST_LEN+1;
  gotoxy(DIRLIST_X,DIRLIST_Y);
  print_list(&dirs, dirs.scroll, DIRLIST_LEN, tabs.selected==TAB_DIRS ? dirs.active:0xff);
}

void draw_files() {
  files.redraw=0;
  if(files.scroll >= files.active) files.scroll = files.active;
  if(files.scroll+DIRLIST_LEN <= files.active) files.scroll=files.active-DIRLIST_LEN+1;
  gotoxy(FILELIST_X,FILELIST_Y);
  print_list(&files, files.scroll, FILELIST_LEN, tabs.selected==TAB_FILES ? files.active:0xff);
}

void draw_playlist() {
  playlist.redraw=0;
  gotoxy(PLAYLIST_X,PLAYLIST_Y);
  print_list(&playlist, playlist.scroll, PLAYLIST_LEN, tabs.selected==TAB_PLAYLIST ? playlist.active:0xff);

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

void screen_update() {
  if (tabs.selected != tabs.last) {
    switch (tabs.last) {
      case TAB_PLAYLIST:
        draw_playlist();
        break;
      case TAB_FILES:
        draw_files();
        break;
      case TAB_DIRS:
        draw_dirs();
        break;
    }
    tabs.last = tabs.selected;
  }
  switch (tabs.selected) {
    case TAB_PLAYLIST:
      draw_playlist();
      break;
    case TAB_FILES:
      draw_files();
      break;
    case TAB_DIRS:
      draw_dirs();
      break;
  }
  if (playlist.redraw) draw_playlist();
  if (files.redraw) draw_files();
  if (dirs.redraw) draw_dirs();
}

void screen_init() {
  unsigned char i;
  videomode(VIDEOMODE_80x30);
  VERA.control |= 0x02; // DCSEL=1
  VERA.display.hstart=8>>2;
  VERA.display.hstop=(640-8)>>2;
  VERA.control &= ~0x02; // DCSEL=0
  clrscr();
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
  for (i=0;i<TAB_COUNT;i++)
    tabs.list[i]->redraw=1;
  screen_update();
}
