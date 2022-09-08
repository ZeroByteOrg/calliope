#include <stdio.h>
#include <conio.h>
#include <cbm.h>
#include <string.h>
#include "files.h"
#include "lists.h"
#include "screen.h"
#include "input.h"
#include "music.h"

// move these to a layout definition .h file of some sort....
#define VRAM_FONT 0x3000

// these are exported by irq.asm
extern void install_irq();
extern void remove_irq();

panel viewer; // main area that shows file list or playlist
panel nav;    // folder selection list

itemlist dirs, files;

// function prototypes
char init();
void panel_selection_move(panel* p, char change);
void select_folder(const char* folder);
void generate_reversed();

char lyr_settings[14]={
  0x06,0x00,0x01,0x00,0x01,0x00,0x00,
  0x61,0xd8,0x98,0x00,0x00,0x00,0x00
};

char txt_palette[6]={
  0x00, 0x00, 0x0, 0x94, 0x11, 0xa4
};

void main() {
  char key;
  cbm_k_chrout(CH_FONT_UPPER);
  if(!init()) {
    printf("no music files or folders found.\n");
    return;
  };
  screen_update();
  while(1) {
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
        case CH_TAB:
          if (activePanel==&viewer)
            panel_activate(&nav);
          else
            panel_activate(&viewer);
          break;
        case CH_ENTER:
          if (activePanel==&viewer) {
            music_stop();
            print_loading(1);
            load(workdir.path,viewer.list->name[viewer.selection],1,(void*)0xa000);
            print_loading(0);
          }
          // no break - fall through into regular ENTER
        case CH_SHIFT_ENTER:
          if (activePanel==&viewer) {
            music_start(workdir.path,viewer.list->name[viewer.selection]);
          }
          else if (activePanel==&nav) {
            select_folder(nav.list->name[nav.selection]);
          }
      }
      screen_update();
      gotoxy(74,28);
      cprintf("%03u",key);
    }
    if (ll_working)
      if (lazy_load()<0)
        print_loading(0);
    print_addresses();
    #define ZSMBANK (*(char*)0x0024)
    #define ZSMADR  (*(unsigned int*)0x0022)
    if ( (ll_bank>ZSMBANK)||((unsigned int)ll_addr == ZSMADR && ll_bank >= ZSMBANK) ) {
      gotoxy(0,32);
      cprintf("too lazy");
    }

  }
  music_stop();
  ym_init();
  VERA.irq_enable ^= 2; // disable line IRQs
  VERA.control |= 2;
  VERA.display.hstop = 640>>2;
  VERA.control ^= 2;
  remove_irq();
  stop_lazy_load();
  go_root();
}

char init() {
  uint8_t i;

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
/*
  VERA.address = 0xfa00 + (16*2 * 6); // choose palette row 6
  VERA.address_hi = 1 | VERA_INC_1;
  for (i=0; i<6 ; i++)
    VERA.data0 = txt_palette[i];
*/
  videomode(VIDEOMODE_80x30);
  textcolor(0);
  bgcolor(TEXT_GREEN); // yes, background color! (2bpp mode)
  clrscr();
  gotoxy(0,0);
  cprintf("loading assets...");
  load_asset("2bppaltfont.bin",1,(void*)0x3000);
  load_asset("bg.bin",0,0);
  cprintf("\n\rgenerating reversed font....");
  generate_reversed();
  for(i=0;i<14;i++) {
     ((char*)0x9f2d)[i] = lyr_settings[i];
  }
  VERA.display.video = 0x31;
  screen_init();
  panel_init(&viewer, &files, SCR_VIEWER_X,SCR_VIEWER_Y,SCR_VIEWER_W,SCR_VIEWER_H);
  panel_init(&nav, &dirs, SCR_NAV_X,SCR_NAV_Y,SCR_NAV_W,SCR_NAV_H);
  viewer.numCols=3;
  if (files.count) panel_activate(&viewer);
  else panel_activate(&nav);

  player_init();

  VERA.control |= 2;
  VERA.display.hstop = 632>>2;
  VERA.control ^= 2;
  install_irq();
  VERA.irq_raster = 0;
  VERA.irq_enable |= 2;


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

void generate_reversed() {
  int i;
  VERA.control = 0;
  VERA.address = VRAM_FONT;
  VERA.address_hi = 1 | VERA_INC_1;
  VERA.control = 1;
  VERA.address = VRAM_FONT+0x800;
  VERA.address_hi = 1 | VERA_INC_1;
  for (i=0;i<0x800;i++) {
    VERA.data1 = VERA.data0 ^ 0xff;
  }
  VERA.control = 0;
}
