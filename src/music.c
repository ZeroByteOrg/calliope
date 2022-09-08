#include <stdint.h>

#include "music.h"
#include "zsmplayer.h"
#include "screen.h"
#include "files.h"

// This is a kludge. If Zsound doesn't end up using this address for delay
// then the code will be broken. Doing this for expediency in not needing
// to expose it in the API
#define ZSMDELAY (*(char*)0x0025)

char music_playing;
char music_ended;
itemlist playlist;

void music_looped(uint8_t playing, uint8_t remaining);


void player_init() {
  zsm_init();
  ym_init();
  list_clear(&playlist);
  music_playing=0;
  music_ended=0;
  zsm_setcallback(&music_looped);
}

void music_play() {
  if (!music_playing) return;
  zsm_play();
  if(music_ended) {
    music_playing = 0;
    // un-highlight the song name, for instance?
  }
}

char music_start(char* path, char* filename) {
  if (music_playing) {
    zsm_stopmusic();
  }
  music_playing=0;
  music_ended = 0;
  ym_init();   // patch for a bug in Zsound on real HW when switching songs.
  if (start_lazy_load(path,filename,1,(void*)0xa000)) {
    lazy_load();
    __asm__ ("sei");
    zsm_startmusic(1,0xa000);
    ZSMDELAY = 8;
    __asm__ ("cli");
    //playlist_add(workdir.path, files.name[files.active]);
    //playlist.redraw=1;
    music_playing = 1;
  }
  return music_playing;
}
void music_start_opening() {
  ym_init();
  music_playing=1;
  zsm_startmusic(1,0xa000);
}

void music_stop() {
  zsm_stopmusic();
  music_playing=0;
  music_ended=0;
}

void music_looped(uint8_t playing, uint8_t remaining) {
  if (!playing) {
    music_ended = 1;
  }
}
