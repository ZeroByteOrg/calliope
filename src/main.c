#include <stdio.h>
#include <conio.h>
#include <cbm.h>
#include <string.h>
#include "files.h"
#include "lists.h"
#include "screen.h"
#include "input.h"
#include "music.h"

panel viewer; // main area that shows file list or playlist
panel nav;    // folder selection list

itemlist dirs, files;

// function prototypes
char init();
void panel_selection_move(panel* p, char change);
void select_folder(const char* folder);

void main() {
  char key;
  cbm_k_chrout(CH_FONT_UPPER);
  if(!init()) {
    printf("no music files or folders found.\n");
    return;
  };
  screen_update();
  while(1) {
    waitvsync();
    music_play();
    if (ll_working)
      if (!lazy_load())
        print_loading(0);
    if(kbhit()) {
      key=cgetc();
      #define CH_PGUP         130
      #define CH_PGDN           2
      #define CH_TAB            9
      #define CH_END            4
      #define CH_SHIFT_END    132
      #define CH_SHIFT_HOME   147
      switch(key) {
        case ' ':
        case CH_STOP: // ESC in emu - not sure what the real code is...
          music_stop();
          break;
        case CH_CURS_UP:
          panel_selection_move(activePanel, SEL_UP);
          break;
        case CH_CURS_DOWN:
          panel_selection_move(activePanel, SEL_DOWN);
          break;
        case CH_CURS_LEFT:
          panel_selection_move(activePanel, SEL_LEFT);
          break;
        case CH_CURS_RIGHT:
          panel_selection_move(activePanel, SEL_RIGHT);
          break;
        case CH_HOME:
          panel_selection_move(activePanel, SEL_TOP);
          break;
        case CH_END:
          panel_selection_move(activePanel, SEL_BOTTOM);
          break;
        case CH_SHIFT_HOME:
          panel_selection_move(activePanel, SEL_FIRST);
          break;
        case CH_SHIFT_END:
          panel_selection_move(activePanel, SEL_LAST);
          break;
        case CH_PGUP:
          panel_selection_move(activePanel, SEL_PGUP);
          break;
        case CH_PGDN:
          panel_selection_move(activePanel, SEL_PGDN);
          break;
        case CH_TAB:
          if (activePanel==&viewer)
            panel_activate(&nav);
          else
            panel_activate(&viewer);
          break;
        case CH_ENTER:
          if (activePanel==&viewer) {
            music_start(workdir.path,viewer.list->name[viewer.selection]);
          }
          else if (activePanel==&nav) {
            select_folder(nav.list->name[nav.selection]);
          }

      }
      screen_update();
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

  screen_init();
  panel_init(&viewer, &files, 17, 17, 3*ITEMSIZE + 3, 20);
  panel_init(&nav, &dirs, 0,17,ITEMSIZE,20);
  panel_activate(&viewer);
  viewer.numCols=3;
  //install_irq();

  player_init();

  return 1;
}

void select_folder(const char* folder) {
  if (!chdir(folder)) return;
  if (strcmp("..",folder)==0) {
    // chdir up
    if (workdir.depth >= 2)
      strrchr(workdir.path,'/')[0]=0;
    else
      workdir.path[0]=0;
    --workdir.depth;
    if (workdir.depth < 0)
      workdir.depth=0;
  }
  else {
    if (workdir.depth > 0)
      strcat(workdir.path,"/");
    strcat(workdir.path,folder);
    ++workdir.depth;
  }
  get_dir_list(&dirs);
  get_zsm_list(&files);
/*
  GET BACK TO USING THIS CONTROL FLOW.....
  draw_path();
  screen_update();
*/
  panel_set_list(&nav,&dirs);
  panel_set_list(&viewer,&files);
}
