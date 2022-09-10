#ifndef _music_h_
#define _music_h_

#include "lists.h"

extern itemlist playlist;
extern char music_playing;
extern char music_ended;

extern void player_init();
extern void music_play();

extern void music_start_opening();

extern char music_start(char* path, char* filename);
extern void music_stop();

extern void __fastcall__ ym_init();
extern void __fastcall__ psg_init();

#endif
