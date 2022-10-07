#include <stdio.h>
#include <conio.h>
#include <cbm.h>
#include <string.h>
#include "files.h"
#include "lists.h"
#include "screen.h"
#include "input.h"
#include "music.h"
#include "skin.h"

// these are exported by irq.asm
extern void install_irq();
extern void remove_irq();

#define MODE_LAZY 0
#define MODE_FULL 1

panel viewer; // main area that shows file list or playlist
panel nav;    // folder selection list

itemlist dirs, files;

char message[SCR_MSG_X1-SCR_MSG_X+1];

// function prototypes
char init();
void panel_selection_move(panel* p, char change);
void select_folder(const char* folder);
void select_zsm(const char* filename, char mode);
void generate_reversed();
void welcome();

char lyr_settings[14]={
  0x06,0x00,0x01,0x00,0x01,0x00,0x00,
  0x61,0xd8,0x98,0x00,0x00,0x00,0x00
};

void main() {
  char key;
  cbm_k_chrout(CH_FONT_UPPER);
  if(!init()) {
    printf("no music files or folders found.\n");
    return;
  };
  screen_update(0);
  welcome();
  panel_select(&viewer,0);
  screen_update(music_playing);
  while(1) {
    if (ll_working)
      if (lazy_load()<0)
        print_loading(0);
    print_addresses(); // TODO: move into IRQ, else into screen_update().
    screen_update(music_playing);
    joystick2kbd(1);  // hack to get the joystick working in time for VCF
    if(kbhit()) {
      key=cgetc();
      if (key=='q') break;
      #define CH_PGUP         130
      #define CH_PGDN           2
      #define CH_TAB            9
      #define CH_END            4
      #define CH_SHIFT_END    132
      #define CH_SHIFT_HOME   147
      #define CH_SHIFT_ENTER  141
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
/*
        case CH_TAB:
          if (activePanel==&viewer)
            panel_activate(&nav);
          else
            panel_activate(&viewer);
          break;
*/
        case CH_SHIFT_ENTER:
        case CH_ENTER:
          if (viewer.selection >= viewer.numdir) {
            select_zsm(viewer.list->name[viewer.selection],(key==CH_SHIFT_ENTER));
          }
          else {
            select_folder(viewer.list->name[viewer.selection]);
          }
          break;
      }
    }
    if (music_ended || !music_playing) {
      clear_msg();
      music_ended=0;
      music_playing=0;
    }
  }
  music_stop();
  stop_lazy_load();
  ym_init();
  psg_init();
  VERA.irq_enable &= ~2; // disable line IRQs
  remove_irq();
  VERA.control |= 2;
  VERA.display.hstop = 640>>2;
  VERA.control ^= 2;
  go_root();
  cbm_k_clrch();
  screen_close();
}

char init() {
  uint8_t i, n;
//  char* addr;

  cbm_k_bsout(CH_FONT_UPPER);
  workdir.depth = 0;
  workdir.path[0] = 0;
  if (get_dir_list(&files, GET_CLEAR)) {
    workdir.root[0]='/';
    workdir.root[1]=0;
  }
  else {
    // strcpy(workdir.root,"./");
    workdir.root[0]='.';
    workdir.root[1]='/';
    workdir.root[2]=0;
  }
  n=files.count;
  get_zsm_list(&files, GET_APPEND);
  if (files.count==0) return 0;
  dirs.count = 0;
  player_init();

  videomode(VIDEOMODE_80x30);
  VERA.control = 0; //~0x02; // ensure we are using DCSEL=0
  VERA.display.border = TEXT_GREEN-1; // make the "CPU utilization" bar green.
  textcolor(0);
  bgcolor(SK_TEXT_NORMAL); // yes, background color! (2bpp mode)
  clrscr();
  gotoxy(0,0);
  cprintf("loading assets...");
  load_skin("default.sk");
  RAM_BANK=1;
  bload("opening.bin",(void*)0xA000);
  // configure VERA display settings
  for(i=0;i<14;i++) {
     ((char*)0x9f2d)[i] = lyr_settings[i];
  }
  VERA.display.video = VERA.display.video & 0x0f | 0x30;
  screen_init();
  panel_init(&viewer, &files, SCR_VIEWER_X,SCR_VIEWER_Y,SCR_VIEWER_W,SCR_VIEWER_H);
  panel_init(&nav, &dirs, SCR_NAV_X,SCR_NAV_Y,SCR_NAV_W,SCR_NAV_H);
  viewer.numCols=3;
  viewer.numdir=n;
  panel_activate(&viewer);
  panel_select(&viewer, 0);

  VERA.control |= 2;
  VERA.display.hstop = 632>>2;
  VERA.control ^= 2;
  install_irq();
  VERA.irq_raster = 0;
  VERA.irq_enable = VERA.irq_enable | 2 & 0x7F;
  music_start_opening(1,0xa000);

  return 1;
}

void welcome(){
  char x;
  panel_clear(&viewer);
  gotoxy(viewer.x,viewer.y);
  x=wherex();
  cprintf("welcome to calliope! --powered by zsound.\n\n");
  gotox(x);
  cprintf("controls:\n");
  gotox(x);
  cprintf("use the cursor keys and home/end/pg-up/pg-down\n");
  gotox(x);
  cprintf("to navigate the file and directory browser.\n\n");
  gotox(x);
  cprintf("use the tab key to switch between the file\n");
  gotox(x);
  cprintf("and directory browser panels.\n\n");
  gotox(x);
  cprintf("spacebar: stop music playback\n\n");
  gotox(x);
  cprintf("enter: start playing slected song\n");
  gotox(x);
  cprintf("       open selected directory\n\n");
  gotox(x);
  cprintf("q: quit\n");
  print_msg("press any key to begin");
  while(kbhit()) cgetc();
  while(!kbhit()) {joystick2kbd(1);}
  while(kbhit()) cgetc();
  panel_clear(&viewer);
  panel_select(&viewer, SEL_FIRST);
  if (files.count >= 1) panel_activate(&viewer);
  clear_msg();
  viewer.dirty = DIRTY_ALL;
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
  get_dir_list(&files, GET_CLEAR);
  viewer.numdir=files.count;
  get_zsm_list(&files, GET_APPEND);
  print_path();
  panel_set_list(&nav,&dirs);
  panel_set_list(&viewer,&files);
  if(files.count > 0 && dirs.count < 2)
    panel_activate(&viewer);
}

void select_zsm(const char* filename, char mode) {

  if (mode==MODE_FULL) {
    music_stop();
    print_loading(1);
    leds_forceupdate(0);
    load(workdir.path,filename,1,(void*)0xa000);
    print_loading(0);
  }
  music_start(workdir.path,filename);
  sprintf(message,"now playing: %s%s/%s", workdir.root,workdir.path,filename);
  print_msg(message);
  leds_forceupdate(music_playing);
}
