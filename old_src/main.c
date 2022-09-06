#include "zsmplayer.h"
#include "files.h"
#include "screen.h"
#include "playlist.h"
#include <conio.h>
#include <cbm.h>
#include <string.h>

#define NUM_QUALITY 6
#define NUM_SOUNDS 14
#define NUM_SONGS 9

#define ZSMBANK 1

#define CLEAR_INPUT while(kbhit()) cgetc();

// these are exported by irq.asm
extern void install_irq();
extern void remove_irq();

// function prototypes
void music_looped(uint8_t playing, uint8_t remaining);
void load_song(const char* filename);
void select_folder(const char* folder);
void changelist(signed char forward);
char init();

extern char ll_bank;
extern char* ll_addr;

char current_song;
char music_playing;
char music_loading;
itemlist dirs, files, playlist, play_paths;
itemlist *selected;

struct tab_s tabs;

const char fkey_map[8] = { 0, 2, 3, 5, 1, 9, 4, 0};
                      //  F1 F3 F5 F7 F2 F4 F6 F8
                      //  85 86 87 88 89 8A 8B 8C

const itemlist* pannels[3] = { &dirs, &files, &playlist };

void music_looped(uint8_t playing, uint8_t remaining) {
  if (!playing) {
    music_playing = 0;
  }
}

void load_song(const char* filename) {
  music_loading = 1;
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
  draw_path();
  screen_update();
}

void changelist(signed char forward) {
  if (forward==0)
    forward=-1;
  else
    forward=1;
  tabs.last = tabs.selected;
  do {
    tabs.selected += forward;
    if (tabs.selected < 0) tabs.selected = TAB_COUNT-1;
    if (tabs.selected >= TAB_COUNT) tabs.selected=0;
    if (tabs.list[tabs.selected]->count > 0) break;
  } while(tabs.last != tabs.selected);
  if (tabs.selected != tabs.last) screen_update();
}

char init() {
  cbm_k_bsout(CH_FONT_UPPER);
  workdir.depth = 0;
  workdir.path[0] = 0;
  if (get_dir_list(&dirs))
    strcpy(workdir.root,"/");
  else
    strcpy(workdir.root,"./");
  get_zsm_list(&files);
  if (files.count==0 && dirs.count==0) return 0;
  tabs.list[TAB_DIRS]=&dirs;
  tabs.list[TAB_FILES]=&files;
  tabs.list[TAB_PLAYLIST]=&playlist;
  tabs.selected=TAB_PLAYLIST;
  zsm_init();
  zsm_setcallback(&music_looped);
  install_irq();
  VERA.irq_raster = 0;    // trigger line IRQ at line 0 for "perf meter"
  VERA.irq_enable = 0x03; // enable VSYNC and LINE IRQs
  current_song=0;
  music_playing=0;
  music_loading=0;
  playlist.count=0;
  playlist.scroll=0;
  playlist.active=0;
  play_paths.count=0;
  play_paths.scroll=0;
  play_paths.active=0;
  screen_init();
  changelist(1);
  gotoxy(0,29);
  cprintf("%02d files",files.count);
  return 1;
}


void main() {
  char key;
  char run=1;
  if (!init()) {
    clrscr();
    cprintf ("sorry - no zsm files or folders found.\n\r");
    return;
  }
  CLEAR_INPUT;
  while(run) {
    if(kbhit()) {
      key=cgetc();
      gotoxy(76,0);
      cprintf("%3u",key);
      if ((key >= CH_F1) && (key < CH_F8)) {
        key = fkey_map[key-CH_F1];
        // not doing quality - leaving here as example of F-key handling...
        key=CH_ENTER;
      }
      if ((key >= '1') && (key < '1'+NUM_SONGS)) {
        key -= '1';
        // do stuff with number keys....
      }
      selected = tabs.list[tabs.selected];
      switch (key) {
      case CH_CURS_RIGHT:
      case 9: // TAB
        changelist(1);
        break;
      case CH_CURS_LEFT:
      case CH_SHIFT_TAB:
        changelist(0);
        break;
      case CH_CURS_DOWN:
        if (selected->active < selected->count-1) {
          ++selected->active;
          selected->redraw = 1;
          screen_update();
        }
        break;
      case CH_CURS_UP:
        if (selected->active>0 && selected->count>0) {
          --selected->active;
          selected->redraw=1;
          screen_update();
        }
        break;
      case CH_STOP:
        run=0;
        break;
      case ' ':
        zsm_stopmusic();
        music_playing = 0;
        break;
      case CH_ENTER:
        if (tabs.selected==TAB_DIRS)
          select_folder(dirs.name[dirs.active]);
        else if (tabs.selected==TAB_FILES) {
          zsm_stopmusic();
          if (init_lazy_load(workdir.path,files.name[files.active],1,(void*)0xa000)) {
            lazy_load();
            print_loading(1);
            __asm__ ("sei");
            zsm_startmusic(1,0xa000);
            *(char*)0x0025 = 4;
            __asm__ ("cli");
            playlist_add(workdir.path, files.name[files.active]);
            playlist.redraw=1;
          }
          else music_playing = 0;
          screen_update();
        }
        key=0;
        break;
      case 141: // shift+enter
        if (tabs.selected==TAB_FILES) {
          zsm_stopmusic();
          print_loading(1);
          files.redraw=1;
          tabs.selected=TAB_COUNT; // "null"
          screen_update();
          if (load(workdir.path,files.name[files.active],1,(void*)0xa000)) {
            zsm_startmusic(1,0xa000);
          }
          tabs.selected=TAB_FILES;
          files.redraw=1;
          print_loading(0);
          screen_update();
        }
        break;
      default:
        break;
      }

    }
    gotoxy(0,28);
    cprintf("$%02x:%04x $%02x:%04x",ll_bank,(unsigned int)ll_addr,*(char*)0x0024,*(unsigned int*)0x0022);
    if(lazy_load()<1) {
      cbm_close(LAZY_LFN);
      print_loading(0);
    }
  }
  VERA.irq_enable = 0x01; // disable raster line IRQs
  remove_irq();
  zsm_stopmusic();
  go_root();
  gotoxy(0,29);
}
