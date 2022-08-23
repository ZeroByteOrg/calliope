#ifndef _screen_h_
#define _screen_h_

#include "system.h"
#include <conio.h>

#define SHOW_DIR(NAME) \
gotoxy(12,3);  \
cprintf("%-20s",(NAME)); \

#define draw_dirs() \
  print_list(&dirs);

#define draw_files() \
  print_list(&files);

#define draw_playlist() \
  print_list(&playlist); \
  print_list(&play_paths);

typedef struct window_s {
  char x1, x2, y1, y2;
} window;

extern void screen_init();
extern void print_list(itemlist* list);
extern void print_loading(char loading);
extern void draw_path();

#endif
