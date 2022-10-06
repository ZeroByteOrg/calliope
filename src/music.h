#ifndef _music_h_
#define _music_h_

#include "lists.h"

extern itemlist playlist;

extern uint8_t music_playing,
               music_ended,
               loops_done,
               loops_left;

extern void player_init();
extern void player_update();
extern uint8_t music_get_ym_chanmask();

extern void music_start_opening();

extern char music_start(const char* path, const char* filename);
extern void music_stop();

extern void __fastcall__ ym_init();
extern void __fastcall__ psg_init();

#endif
