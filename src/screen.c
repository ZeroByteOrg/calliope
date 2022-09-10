#include "screen.h"
#include "files.h" // for the lazy loader pointer variables ll_addr/ll_bank
#include <cbm.h>
#include <conio.h>
#include <stdio.h> // sprintf()


panel* panels[MAX_PANELS];
panel* activePanel;
uint8_t numPanels;
uint8_t leds_active;

void print_list(panel* p);
void print_panel_debug();
void leds_init();
void leds_update();
void clear_sprites();

char msg_cleared=1;
char blankline[] = "%80s"; // token for cprintf() to use

void screen_init() {
  char i=0;
  clrscr();
  while(i<MAX_PANELS) panels[i++]=NULL;
  numPanels = 0;
  activePanel = NULL;
  print_loading(0);
  gotoxy(SCR_PLAY_ADDR_X-5,SCR_PLAY_ADDR_Y);
  cprintf("play ");
  leds_init();
}


void screen_update(uint8_t music_playing) {
  char i, a;
  leds_update(music_playing);
  for (i=0;i<numPanels;i++) {
    //if (panels[i]->dirty) panel_draw(panels[i]);
    panel_draw(panels[i]);
    if(panels[i]==activePanel) a=i;
  }
}

void screen_close() {
  clear_sprites();
  VERA.layer1.config=0x60;
  VERA.layer1.mapbase=0xd8;
  VERA.layer1.tilebase=0xf8;
  VERA.layer0.config=0;
  VERA.layer0.mapbase=0;
  VERA.layer0.tilebase=0;
  bgcolor(COLOR_BLUE);
  textcolor(COLOR_WHITE);
  videomode(VIDEOMODE_80x60);
  clrscr();
  cprintf("may all of your days be musical!\n\n\r");
}

void print_panel_debug() {
  gotoxy (20,26);
  cprintf(
    "items:%2d  slected:%2d  row:%2d col:%2d",
    activePanel->list->count,
    activePanel->selection,
    activePanel->row,
    activePanel->col
  );
}

void panel_init(panel* p, itemlist* l, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  p->x=x;
  p->y=y;
  p->w=w;
  p->h=h;
  p->numCols=1;
  p->scroll=0;
  p->active=0;
  p->list=l;
  p->row=0;
  p->col=0;
  p->scroll=0;
  if (numPanels < MAX_PANELS) {
    panels[numPanels]=p;
    if(numPanels==0) {
      panel_activate(p);
    }
    ++numPanels;
  }
  panel_select(p,SEL_FIRST);
  p->dirty=1;
}

void panel_clear(panel* p) {
  uint8_t i;
  uint8_t x=p->x;
  uint8_t y=p->y;

  gotoxy(x,y);
  sprintf(&blankline[1],"%02us",p->w); // set the %s token in blankline as width.
  for(i=p->h;i>=1;i--) {
    cprintf(blankline," \n");
    gotox(x);
  }
  p->dirty=0;
}

void panel_draw(panel* p) {
  uint8_t x=p->x;
  uint8_t y=p->y;
  gotoxy(x,y);
  print_list(p);
  p->dirty=0;
}

void print_list(panel* p) {
  uint8_t i,j,step;
  uint16_t n;
  char** name;

  name=p->list->name;
  step=p->list->count/p->numCols + 1;
  gotoxy(p->x, p->y);
  for (i=0;i<p->h;i++) {
    n=p->scroll+i;
    for (j=0; j<p->numCols ; j++) {
      cprintf(" ");
      if(n<p->list->count && i+p->scroll<step) {
        if (n==p->selection && p->active) revers(1);
        cprintf("%-" STR(ITEMSIZE) "s",name[n]);
        revers(0);
      }
      else
        cprintf("%" STR(ITEMSIZE) "s"," ");
      n += step;
    }
    cprintf("\n");
    gotox(p->x);
  }
}

void panel_select(panel* p, uint8_t item) {
  uint8_t step, mod, scroll;

  if (item >= p->list->count) return;   // do nothing if unused index
  if (item == p->selection) return;     // do nothing if same selected item

  step = p->list->count/p->numCols + 1; // how much to +/- for moving L/R
  mod  = item % (step);                 // "row" of selected item
  scroll = p->scroll;
  p->selection=item;
  if (mod < scroll)
    scroll = mod;
  else if (mod >= p->h + scroll - 1)
    scroll = mod - p->h + 1;

  if (scroll != p->scroll) {
    p->scroll=scroll;
    p->dirty=1;
  }
  else {
    // TODO: make code that just updates the old/new selection display
    p->dirty=1;
  }
}

// placeholders - better functionality would be to just re-draw the selected item...
void panel_activate(panel* p) {
  activePanel->active=0;
  activePanel->dirty=1;
  p->active=1;
  p->dirty=1;
  activePanel=p;
}

void panel_selection_move(panel* p, char change) {
  uint8_t step,mod,newselect;

  step = p->list->count/p->numCols + 1; // how much to +/- for moving L/R
  mod  = p->selection % (step);         // "row" of selection
  newselect = p->selection;             // default to current selection.
  switch (change) {
    case SEL_UP:
      if (mod != 0) --newselect;
      break;
    case SEL_DOWN:
      if (mod+1 < p->list->count && mod+1 < step) ++newselect;
      break;
    case SEL_LEFT:
      if (newselect - step >= 0) newselect -= step;
      break;
    case SEL_RIGHT:
      if (newselect + step < p->list->count) newselect += step;
      break;
    case SEL_TOP:
      newselect -= mod; // move to top of column
      break;
    case SEL_BOTTOM:
      newselect = newselect - mod + step - 1; // move to bottom of column
      if (newselect >= p->list->count) newselect = p->list->count-1;
      break;
    case SEL_PGUP:
      if (mod - p->h >= 0)
        newselect = newselect - p->h;
      else
        newselect -= mod;
      break;
    case SEL_PGDN:
      if (mod+p->h < step)
        newselect += p->h;
      else
        newselect = step-1;
      if (newselect >= p->list->count) newselect = p->list->count-1;
      p->scroll += p->h + 1;
      if (p->scroll >= step-1) p->scroll=step-1;
      break;
    case SEL_FIRST:
      newselect = 0;
      break;
    case SEL_LAST:
      newselect = p->list->count-1;
      break;
    default:
      return;  // invalid direction sent? Use the enum, Luke!!!
  }
  panel_select(p, newselect);
}

void panel_set_list(panel* p, itemlist* l) {
  p->list = l;
  panel_select(p, 0);
  p->dirty = 1;
}

void print_loading(char isloading) {
  gotoxy (SCR_LOAD_ADDR_X-5,SCR_LOAD_ADDR_Y);
  revers(isloading==1);
  cprintf("load");
  revers(0);
}

void __fastcall__ print_addresses() {
  gotoxy(SCR_LOAD_ADDR_X, SCR_LOAD_ADDR_Y);
  cprintf("%02x:%04x",ll_bank,(uint16_t)ll_addr);
  gotoxy(SCR_PLAY_ADDR_X, SCR_PLAY_ADDR_Y);
  cprintf("%02x:%04x",*(uint8_t*)0x0024,*(uint16_t*)0x0022);
}

void print_path() {
  gotoxy(SCR_PATH_X,SCR_PATH_Y);
  cprintf("%s%s",workdir.root,workdir.path);
  do {
    cprintf(" ");
    if(wherex()==SCR_PATH_X1) break;
  } while(1);
}

void print_msg(char* msg) {
  gotoxy(SCR_MSG_X,SCR_MSG_Y);
  cprintf("%s",msg);
  do {
    cprintf(" ");
    if(wherex()==SCR_MSG_X1) break;
  } while(1);
  msg_cleared=0;
}

void clear_msg() {
  if (!msg_cleared) {
    msg_cleared=1;
    print_msg("");
  }
}

void clear_sprites() {
  uint16_t i;

  VERA.control=0;
  VERA.address=0xFC00;
  VERA.address_hi=1|VERA_INC_1;
  for (i=0 ; i<(0x1FFFF-0x1FC00) ; i++) VERA.data0=0; // clear the sprite regs
  VERA.display.video &= ~(1<<6); // disable sprites
}

void leds_init() {
  uint16_t i;
  uint16_t  x,y;

  clear_sprites();
  VERA.address=0xFC00;
  VERA.address_hi=1|VERA_INC_1;
  for (i=0 ; i<(0x1FFFF-0x1FC00) ; i++) VERA.data0=0; // clear the sprite regs
  VERA.address=0xFC00; // return data0 to sprite 1 register.

  x=SCR_LEDYM_X;
  y=SCR_LEDYM_Y;
  for(i=0; i<24 ; i++) {
    if (i==8) {
      x=SCR_LEDPSG_X;
      y=SCR_LEDPSG_Y;
    }
    if (i==16) {
      x=SCR_LEDPSG_X;
      y=(SCR_LEDPSG_Y+8);
    }
    VERA.data0=((SCR_LED_VRAMBASE>>5) & 0xFF);
    VERA.data0=((SCR_LED_VRAMBASE>>13) & 0xFF);
    VERA.data0=x & 0xFF;
    VERA.data0=(x>>8) & 0x03;
    VERA.data0=y & 0xFF;
    VERA.data0=(y>>8) & 0x03;
    VERA.data0=0x0C;
    VERA.data0=SCR_LED_PALETTE;
    x+=8;
  }
  VERA.display.video |= (1<<6); // enable sprites
  leds_active = 0;
}


void leds_update(uint8_t playing) {
  #define ZSM_MASK (*(char*)0xA009)
  uint8_t i,c;
  if (leds_active) {
    if (!playing) {
      leds_active=0; // make sure the IRQ won't update them during the clear
      VERA.control=0;
      VERA.address=0xFC00;
      VERA.address_hi=1|VERA_INC_8;
      for(i=0;i<24;i++) VERA.data0 = SCR_LED_VRAMBASE & 0XFF;
    }
  }
  else {
    if (playing)
      RAM_BANK=1;
      VERA.control=0;
      VERA.address=0xFC00;
      VERA.address_hi=1|VERA_INC_8;
      for(i=0;i<8;i++)
        VERA.data0 = ((ZSM_MASK >> i) & 0x01) ? 0x0f : 0;
      leds_active=1;
  }
}
