#ifndef _screen_h_
#define _screen_h_

#include "lists.h"
#include <stdint.h>

#define ITEMSIZE 16
#define STR_(X) #X
#define STR(X) STR_(X)

#define MAX_PANELS 4

enum selectionmoves {
  SEL_UP,
  SEL_DOWN,
  SEL_LEFT,
  SEL_RIGHT,
  SEL_TOP,
  SEL_BOTTOM,
  SEL_PGUP,
  SEL_PGDN,
  SEL_FIRST,
  SEL_LAST
};

enum textcolors {
  TEXT_GREEN=6
};

typedef struct {
  uint8_t x, y, w, h;
  uint8_t numCols;
  uint8_t scroll;
  uint8_t row, col;
  itemlist* list;
  uint8_t selection;
  uint8_t active;
  uint8_t dirty;
} panel;

extern panel* activePanel;

extern void screen_init();
extern void screen_update();

extern void panel_init(panel* p, itemlist* l, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
extern void panel_set_list(panel* p, itemlist* l);
extern void panel_clear(panel* p);
extern void panel_draw(panel* p);
extern void panel_select(panel* p, uint8_t item);
extern void panel_activate(panel* p);

extern void print_loading(char isloading);

#endif
