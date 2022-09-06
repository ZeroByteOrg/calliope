#include <stdint.h>

#include "zsmplayer.h"
#include "files.h"
#include "lists.h"

char music_playing;
itemlist playlist;

void player_init() {
  zsm_init();
  list_clear(&playlist);
}

void music_looped(uint8_t playing, uint8_t remaining) {
  if (!playing) {
    music_playing = 0;
  }
}
