#include <stdio.h>
#include <conio.h>
#include <cbm.h>
#include "files.h"
#include "lists.h"
#include "screen.h"
#include "input.h"
#include "music.h"

panel viewer; // main area that shows file list or playlist
panel nav;    // folder selection list
panel* activepanel;

itemlist dirs, files;

// function prototypes
char init();
void panel_selection_move(panel* p, char change);

void main() {
  char key;
  cbm_k_chrout(CH_FONT_UPPER);
  if(!init()) {
    printf("no music files or folders found.\n");
    return;
  };
  while(1) {
    // szound goes here
    if(kbhit()) {
      key=cgetc();
      #define CH_PGUP       130
      #define CH_PGDN         2
      #define CH_TAB          9
      #define CH_SHIFT_HOME 147
      #define CH_END          4
      #define CH_SHIFT_END  132
      switch(key) {
        case CH_CURS_UP:
          panel_selection_move(activepanel, SEL_UP);
          break;
        case CH_CURS_DOWN:
          panel_selection_move(activepanel, SEL_DOWN);
          break;
        case CH_CURS_LEFT:
          panel_selection_move(activepanel, SEL_LEFT);
          break;
        case CH_CURS_RIGHT:
          panel_selection_move(activepanel, SEL_RIGHT);
          break;
        case CH_HOME:
          panel_selection_move(activepanel, SEL_TOP);
          break;
        case CH_END:
          panel_selection_move(activepanel, SEL_BOTTOM);
          break;
        case CH_SHIFT_HOME:
          panel_selection_move(activepanel, SEL_FIRST);
          break;
        case CH_SHIFT_END:
          panel_selection_move(activepanel, SEL_LAST);
          break;
        case CH_PGUP:
          panel_selection_move(activepanel, SEL_PGUP);
          break;
        case CH_PGDN:
          panel_selection_move(activepanel, SEL_PGDN);
          break;
        case CH_TAB:
          panel_deactivate(activepanel);
          if (activepanel==&viewer)
            activepanel=&nav;
          else
            activepanel=&viewer;
          panel_activate(activepanel);
      }
      gotoxy(77,0);
      cprintf("%03u",key);
    }
  }
}

char init() {
  cbm_k_bsout(CH_FONT_UPPER);
  workdir.depth = 0;
  workdir.path[0] = 0;
  if (get_dir_list(&dirs)) {
    workdir.root[0]='/';
    workdir.root[1]=0;
  }
  else {
    // strcpy(workdir.root,"./");
    workdir.root[0]='.';
    workdir.root[1]='/';
    workdir.root[2]=0;
  }
  get_zsm_list(&files);
  if (files.count==0 && dirs.count==0) return 0;

  panel_init(&viewer, &files, 17, 17, 3*ITEMSIZE + 3, 20);
  panel_init(&nav, &dirs, 0,17,ITEMSIZE,20);
  viewer.numCols=3;
  activepanel = &viewer;
  screen_init();
  panel_draw(&viewer);
  panel_draw(&nav);
  panel_activate(&viewer);
  //install_irq();

  player_init();

  return 1;
}
