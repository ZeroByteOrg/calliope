#include <stdint.h>

#include "music.h"
#include "zsmplayer.h"
#include "files.h"

// This is a kludge. If Zsound doesn't end up using this address for delay
// then the code will be broken. Doing this for expediency in not needing
// to expose it in the API
#define ZSMDELAY (*(char*)0x0025)

itemlist playlist;

uint8_t music_playing = 0;
uint8_t music_ended   = 0;
uint8_t loops_done    = 0;
uint8_t loops_left    = 0;

void music_looped(uint8_t playing, uint8_t remaining);


void player_init() {
  zsm_init();
  ym_init();
  psg_init();
  list_clear(&playlist);  // Move this out of player?
  zsm_setcallback(&music_looped);
}

void player_update() {
  if (!music_playing) return;
  //zsm_play();
  if(music_ended) {
    music_playing=0;
    music_ended=0;
    // un-highlight the song name, for instance?
  }
}

char music_start(const char* path, const char* filename) {
  if (music_playing) {
    zsm_stopmusic();
  }
  music_playing=0;
  music_ended=0;
  loops_done=0;
  loops_left=0;
  ym_init();   // patch for a bug in Zsound on real HW when switching songs.
  if (start_lazy_load(path,filename,1,(void*)0xa000)) {
    lazy_load(); // get the first ~512 bytes into memory for zsm_startmusic
                 // to have access to the ZSM header...
    __asm__ ("sei");
    zsm_startmusic(1,0xa000);
    ZSMDELAY = 8;       // give the lazy loader a head start
    __asm__ ("cli");
    music_playing = 1;
  }
  return music_playing;
}

// quick hack to get the OPENING.BIN song to play w/o needing to pass a
// filename and path. Maybe convert this to a general-purpose
// "play-whats-loaded" API call?
void music_start_opening() {
  ym_init();
  music_playing=1;
  music_ended=0;
  loops_done=0;
  loops_left=0; // 0=infinite... TODO: user-defined max-loops control.
  zsm_startmusic(1,0xa000);
}

void music_stop() {
  zsm_stopmusic();
  music_playing=0;
  music_ended=0;
  loops_left=0;
}

// This is the loop/end notification callback handler.
void music_looped(uint8_t playing, uint8_t remaining) {
  ++loops_done;
  if (!playing) {
    music_ended = 1;
  }
  loops_left=remaining;
}

uint8_t music_get_ym_chanmask() {
  // stub. this is how things _should_ get the
  // channels-in-use mask.
  return 0;
}
