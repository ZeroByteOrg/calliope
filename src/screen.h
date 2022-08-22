#ifndef _screen_h_
#define _screen_h_

#include "system.h"
#include <conio.h>

#define SHOW_DIR(NAME) \
gotoxy(12,3);  \
cprintf("%-20s",(NAME)); \

#define draw_dirs() \
  print_list(&dirs,12);

#define draw_files() \
  print_list(&files,12);

typedef struct window_s {
  char x1, x2, y1, y2;
} window;

extern void screen_init();
extern void print_list(itemlist* list, char count);
extern void draw_path();

#endif
