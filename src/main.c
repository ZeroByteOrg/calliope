#include "zsmplayer.h"
#include "files.h"
#include "screen.h"
#include <conio.h>
#include <cbm.h>

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
void load_song(char* filename);
void init();

char current_song;
char music_playing;
char music_loading;
itemlist files;

const char fkey_map[8] = { 0, 2, 3, 5, 1, 9, 4, 0};
                      //  F1 F3 F5 F7 F2 F4 F6 F8
                      //  85 86 87 88 89 8A 8B 8C

void music_looped(uint8_t playing, uint8_t remaining) {
  if (!playing) {
    music_playing = 0;
    print_songlist();
  }
}

void load_song(char* filename) {
  music_loading = 1;
  chdir("/zsm");
  SHOW_DIR("zsm");
  if (music_playing)
    zsm_stopmusic();
  print_songlist();
  RAM_BANK = ZSMBANK;
  bload(filename,(char*)0xa000);
  music_loading = 0;
  if (music_playing)
    zsm_startmusic(ZSMBANK,0xa000);
  print_songlist();
}

void init() {
  screen_init();
  zsm_init();
  zsm_setcallback(&music_looped);
  load_song(0);
  print_songlist();
  install_irq();
  current_song=0;
  music_playing=0;
  music_loading=0;

  files.name[0]="foo";
  files.name[1]="bar";
  files.name[2]="bat";
  files.count=3;
  files.active=0xff;
  gotoxy(2,6);
  print_list(&files,0,3);
}

void main() {
  char key;
  char run=1;
  init();
  CLEAR_INPUT;
  while(run) {
    if(kbhit()) {
      key=cgetc();
//      gotoxy (78,0);
//      cprintf("%02x",key);
      if ((key >= CH_F1) && (key < CH_F8)) {
        key = fkey_map[key-CH_F1];
        // not doing quality - leaving here as example of F-key handling...
        key=CH_ENTER;
      }
#if(0)
      if ((key >= 'a') && (key < 'a'+NUM_SOUNDS)) {
        key -= 'a';
        if (key!=current_sound) {
          current_sound = key;
          print_soundlist();
        }
        key=CH_ENTER;
      }
#endif
      if ((key >= '1') && (key < '1'+NUM_SONGS)) {
        key -= '1';
        if (key != current_song) {
          current_song = key;
          music_playing = 1;
//          key=music_playing; // use key as tmp to hold true state...
//          music_playing=0;
// //          print_songlist();
//          music_playing=key;
          load_song(current_song);
          if (music_playing) print_songlist();
          key = 0;
          CLEAR_INPUT;
        }
        else
          key = CH_DEL; // just toggle
      }
      switch (key) {
      case CH_DEL:
        if (!music_playing) {
          zsm_startmusic(ZSMBANK,0xA000);
          music_playing=1;
        }
        else {
          zsm_stopmusic();
          music_playing=0;
        }
        print_songlist();
        break;
      case CH_STOP:
        run=0;
        break;
      case ' ':
        zsm_stopmusic();
        music_playing = 0;
        print_songlist();
        break;
      case CH_ENTER:
      default:
        break;
      }
    }
  }
  remove_irq();
  zsm_stopmusic();
  gotoxy(0,29);
}
