#ifndef _screen_h_
#define _screen_h_

#include "lists.h"
#include "skin.h"
#include <stdint.h>

#define ITEMSIZE 16
#define STR_(X) #X
#define STR(X) STR_(X)

#define MAX_PANELS 4

//Screen Layout defines
#define SCR_PALBASE 16
#define SCR_LOAD_ADDR_X 7
#define SCR_LOAD_ADDR_Y 1
#define SCR_PLAY_ADDR_X 7
#define SCR_PLAY_ADDR_Y 3
#define SCR_VIEWER_X 21
#define SCR_VIEWER_Y 8
#define SCR_VIEWER_W ((3*ITEMSIZE + 3) + 6)
#define SCR_VIEWER_H 10
#define SCR_NAV_X 1
#define SCR_NAV_Y 6
#define SCR_NAV_W (ITEMSIZE)
#define SCR_NAV_H (SCR_VIEWER_H)
#define SCR_PATH_X 21
#define SCR_PATH_Y 6
#define SCR_PATH_X1 76
#define SCR_MSG_X 1
#define SCR_MSG_Y 28
#define SCR_MSG_X1 76
#define SCR_LEDYM_X 528
#define SCR_LEDYM_Y 8
#define SCR_LEDPSG_X 528
#define SCR_LEDPSG_Y 24
//#define SCR_LED_VRAMBASE 0x14000
#define SCR_LED_PALETTE 2

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

enum dirty_flags {
  DIRTY_CLEAR = 0,
  DIRTY_SCROLL = (1<<7),
  DIRTY_SELECT = (1<<6),
  DIRTY_PLAYING = (1<<5),
  DIRTY_ALL = 0xFF
};

enum textcolors {
  TEXT_GREEN=6
};

typedef struct {
  uint8_t x, y, w, h;
  uint8_t numCols;  // the panel is configured to display up to this many...
  uint8_t scroll;   
  uint8_t row, col; // DELME? I don't think I'll be using these....
  uint8_t selection;
  uint8_t prevselect;
  uint8_t active;
  uint8_t dirty;
  uint8_t numdir;
  itemlist* list;
} panel;



extern panel* activePanel;

extern void screen_init();
extern void screen_update(uint8_t music_playing);
extern void screen_close();

extern void panel_init(panel* p, itemlist* l, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
extern void panel_set_list(panel* p, itemlist* l);
extern void panel_clear(panel* p);
extern void panel_draw(panel* p);
extern void panel_select(panel* p, uint8_t item);
extern void panel_activate(panel* p);

extern void leds_enable(uint8_t leds_on);
extern void leds_forceupdate(uint8_t leds_on);

extern void print_loading(char isloading);
extern void __fastcall__ print_addresses();
extern void print_path();
extern void print_msg(char* msg);
extern void clear_msg();

#endif
