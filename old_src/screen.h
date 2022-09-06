#ifndef _screen_h_
#define _screen_h_

#include "system.h"
#include <conio.h>


extern void screen_init();
extern void print_list(itemlist* list, char start, char count, char active);
extern void print_loading(char loading);
extern void draw_path();
extern void screen_update();

#endif
