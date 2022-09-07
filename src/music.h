#ifndef _music_h_
#define _music_h_

#include "lists.h"

extern itemlist playlist;

extern void player_init();
extern void music_play();

extern char music_start(char* path, char* filename);
extern void music_stop();

extern void __fastcall__ ym_init();

#endif
