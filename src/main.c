#include "zsmplayer.h"
#include "files.h"
#include "screen.h"
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
void init();

extern char ll_bank;
extern char* ll_addr;

char current_song;
char music_playing;
char music_loading;
itemlist dirs, files, playlist, play_paths;
itemlist *selected;

const char fkey_map[8] = { 0, 2, 3, 5, 1, 9, 4, 0};
                      //  F1 F3 F5 F7 F2 F4 F6 F8
                      //  85 86 87 88 89 8A 8B 8C

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
  draw_files();
  draw_dirs();
}

void init() {
  workdir.depth = 0;
  workdir.path[0] = 0;
  zsm_init();
  zsm_setcallback(&music_looped);
  install_irq();
  current_song=0;
  music_playing=0;
  music_loading=0;
  dirs.x = 1;
  dirs.y = LIST_Y;
  files.x = 22;
  files.y = LIST_Y;
  if (get_dir_list(&dirs))
    strcpy(workdir.root,"/");
  else
    strcpy(workdir.root,"./");
  get_zsm_list(&files);
  selected = &dirs;
  screen_init();
  print_list(&dirs,12);
  print_list(&files,12);
  gotoxy(0,29);
  cprintf("%02d files",files.count);
}

void main() {
  char key;
  char run=1;
  init();
  CLEAR_INPUT;
  while(run) {
    if(kbhit()) {
      key=cgetc();
      gotoxy(78,0);
      cprintf("%u",key);
      if ((key >= CH_F1) && (key < CH_F8)) {
        key = fkey_map[key-CH_F1];
        // not doing quality - leaving here as example of F-key handling...
        key=CH_ENTER;
      }
      if ((key >= '1') && (key < '1'+NUM_SONGS)) {
        key -= '1';
        // do stuff with number keys....
      }
      switch (key) {
      case 9: // TAB
        if (selected==&files)
          selected=&dirs;
        else if (selected==&dirs)
          selected=&files;
        draw_files();
        draw_dirs();
        break;
      case CH_CURS_DOWN:
        if (selected->active < selected->count-1) {
          ++selected->active;
          print_list(selected,12);
        }
        break;
      case CH_CURS_UP:
        if (selected->active>0 && selected->count>0) {
          --selected->active;
          print_list(selected,12);
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
        if (selected==&dirs)
          select_folder(dirs.name[dirs.active]);
        else if (selected==&files) {
          zsm_stopmusic();
          if (init_lazy_load(workdir.path,files.name[files.active],1,(void*)0xa000)) {
            lazy_load();
            print_loading(1);
            __asm__ ("sei");
            zsm_startmusic(1,0xa000);
            *(char*)0x0025 = 4;
            __asm__ ("cli");
          }
          else music_playing = 0;
        }
        key=0;
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
  remove_irq();
  zsm_stopmusic();
  gotoxy(0,29);
}
