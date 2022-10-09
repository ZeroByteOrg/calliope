#include "screen.h"
#include "files.h" // for the lazy loader pointer variables ll_addr/ll_bank
#include <cbm.h>
#include <conio.h>
#include <stdio.h> // sprintf()

//#include "music.h"


panel* panels[MAX_PANELS];
panel* activePanel;
uint8_t numPanels;

uint8_t leds_active=0;

void print_blank_row();
uint8_t print_list(itemlist* list, uint8_t start, uint8_t maxrows, uint8_t cols, uint8_t numfolder);
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
  char i;
  leds_update(music_playing);
  for (i=0;i<numPanels;i++) {
    if (panels[i]->dirty) panel_draw(panels[i]);
    while(!kbhit()) {}
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
  VERA.control |= 2;
  VERA.display.hstop = 640>>2;
  VERA.control ^= 2;
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
  p->row=0;    // DELME? I don't think I'll be using these.....
  p->col=0;
  p->scroll=0;
  if (numPanels < MAX_PANELS) {
    panels[numPanels]=p;
    if(numPanels==0) {
      panel_activate(p);
    }
    ++numPanels;
  }
  panel_select(p,0);
  p->prevselect=0;
  p->dirty=DIRTY_ALL;
  p->numdir=0;
  p->list=l;
}

void panel_clear(panel* p) {
  uint8_t i;

  gotoxy(p->x,p->y);
  sprintf(&blankline[1],"%02us",p->w); // set the %s token in blankline as width.
  for(i=p->h;i>=1;i--)
    print_blank_row(p->w);
  p->dirty=DIRTY_CLEAR;
}

void panel_draw(panel* p) {
  uint8_t i, rows, mod;
  char foldermark;

  rows=p->list->count/p->numCols;
  mod=p->list->count%p->numCols;
  if (mod) ++rows;
  if (p->dirty & DIRTY_SCROLL) {
    gotoxy(p->x,p->y);
    i = print_list(p->list, p->scroll, p->h, p->numCols, p->numdir);
    for(i=p->h+p->y-i;i>=1;i--)
      print_blank_row(p->w);
    p->dirty |= DIRTY_SELECT;
  }
  else if (p->dirty & DIRTY_SELECT) {
    // clear the highlight bar if no scrolling
    foldermark=(p->prevselect<p->numdir)?'Z':' ';
    gotoxy(
      p->x+p->prevselect/rows*(ITEMSIZE+2)+1,
      p->y+p->prevselect%rows-p->scroll
    );
    cprintf("%c%-" STR(ITEMSIZE) "s",foldermark,p->list->name[p->prevselect]);
  }
  if (p->dirty & DIRTY_SELECT) {
    // draw the highlight bar
    foldermark=(p->selection<p->numdir)?'Z':' ';
    gotoxy(
      p->x+p->selection/rows*(ITEMSIZE+2)+1,
      p->y+p->selection%rows-p->scroll
    );
    revers(1);
    cprintf("%c%-" STR(ITEMSIZE) "s",foldermark,p->list->name[p->selection]);
    revers(0);
    p->prevselect=p->selection;
  }
  p->dirty=DIRTY_CLEAR;
}

#if(0)
// not using this code for now.
void viewer_draw(panel* p) {
  uint8_t i, count, rows, dir_rows;
  gotoxy(p->x,p->y);
  dir_rows=p->numdir/p->numCols + 1;
  if (p->numdir%p->numCols==0 && dir_rows>0) --dir_rows;
  if (dir_rows-1 >= p->scroll) {
    count=p->list->count;
    p->list->count=p->numdir;
  }
}
#endif

void print_blank_row(uint8_t w) {
  uint8_t x = wherex();
  while (w-->0) cprintf(" ");
  cprintf("\n");
  gotox(x);
}

uint8_t print_list(itemlist* list, uint8_t start, uint8_t maxrows, uint8_t cols, uint8_t numfolder) {
  uint8_t i,j,x,step;
  uint16_t n;
  char** name;

  if (maxrows==0) return wherey();
  name=list->name;
  step=list->count/cols;
  if (list->count%cols) ++step;
  x=wherex();
  if (step-start < maxrows) maxrows=step-start;
  for (i=0;i<maxrows;i++) {
    n=i+start;
    for (j=0;j<cols;j++) {
      if (n<numfolder) cprintf(" Z");
      else cprintf ("  ");
      if(n<list->count) {
        cprintf("%-" STR(ITEMSIZE) "s",name[n]);
      }
      else
        cprintf("%" STR(ITEMSIZE) "s"," ");
      n += step;
    }
    cprintf("\n");
    gotox(x);
  }
  return wherey();
}

void panel_select(panel* p, uint8_t item) {
  uint8_t step, mod;
  int scroll, maxscroll;

  if (item >= p->list->count) return;   // do nothing if unused index
  if (item == p->selection) return;     // do nothing if same selected item

  p->dirty |= DIRTY_SELECT;
  step = p->list->count/p->numCols + 1; // how much to +/- for moving L/R
  mod  = item % (step);                 // "row" of selected item
  scroll = p->scroll;
  maxscroll = step - p->h;              // amount to put last row at bottom
  if (maxscroll < 0) maxscroll=0;       // if the list is long enough.
  p->selection=item;
  // scroll up or down as needed to keep the selection at least 2 rows away
  // from the top/bottom of the panel...
  if (mod < scroll + 2)
    scroll = mod - 2;
  else if (mod - scroll >= p->h-3)
    //scroll = mod + 2 - p->h - 1;
    scroll = mod + 3 - p->h;
  // constrain scrolling such that there are never blank rows at the top
  // and scrolling down stops whenever the last row is visible at the end
  // of the panel
  if (scroll >= maxscroll) scroll = maxscroll;
  if (scroll < 0) scroll=0;
  if (scroll != p->scroll) {
    p->scroll=scroll;
    p->dirty |= DIRTY_SCROLL;
  }
}

// placeholders - better functionality would be to just re-draw the selected item...
void panel_activate(panel* p) {
  // deactivate the current panel and set dirty flag to erase the selection bar
  activePanel->active=0;
  activePanel->dirty|=DIRTY_SELECT;

  p->active=1;
  p->dirty|=DIRTY_SELECT;
  activePanel=p;
}

void panel_selection_move(panel* p, char change) {
  uint8_t step, row, newselect;

  step = p->list->count/p->numCols;     // how much to +/- for moving L/R
  if (p->list->count%p->numCols) ++step;
  row  = p->selection % (step);         // absolute row of selection
  newselect = p->selection;             // default to current selection.
  switch (change) {
    case SEL_UP:
      if (row != 0) --newselect;
      break;
    case SEL_DOWN:
      if (row+1 < p->list->count && row+1 < step) ++newselect;
      break;
    case SEL_LEFT:
      if (newselect - step >= 0) newselect -= step;
      break;
    case SEL_RIGHT:
      if (newselect + step < p->list->count) newselect += step;
      break;
    case SEL_TOP:
      newselect -= row; // move to top of column
      break;
    case SEL_BOTTOM:
      newselect = newselect - row + step - 1; // move to bottom of column
      if (newselect >= p->list->count) newselect = p->list->count-1;
      break;
    case SEL_PGUP:
      if (row - p->h >= 0)
        newselect = newselect - p->h;
      else
        newselect -= row;
      break;
    case SEL_PGDN:
      if (row+p->h < step)
        newselect += p->h;
      else
        newselect += step-1-row;
      if (newselect >= p->list->count) newselect = p->list->count-1;
      //p->scroll += p->h + 1;
      //if (p->scroll >= step-1) p->scroll=step-1;
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
  p->dirty = DIRTY_ALL;
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

void print_path() {  VERA.control |= 2;
  VERA.display.hstop = 640>>2;
  VERA.control ^= 2;

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
//    VERA.data0=((SCR_LED_VRAMBASE>>5) & 0xFF);
//    VERA.data0=((SCR_LED_VRAMBASE>>13) & 0xFF);
    VERA.data0=((VRAM_LEDS>>5) & 0xFF);
    VERA.data0=((VRAM_LEDS>>13) & 0xFF);
    VERA.data0=x & 0xFF;
    VERA.data0=(x>>8) & 0x03;
    VERA.data0=y & 0xFF;
    VERA.data0=(y>>8) & 0x03;
    VERA.data0=0x0C;
    VERA.data0=SCR_LED_PALETTE;
    x+=8;
  }
  VERA.display.video |= (1<<6); // enable sprites
}

void leds_enable(uint8_t leds_on) {
  uint8_t i;

  if (leds_on) {
    if (!leds_active) {
      // hack to access the YM channel use mask.
      #define ZSM_MASK (*(char*)0xA009)
      RAM_BANK=1;
      VERA.control=0;
      VERA.address=0xFC00;
      VERA.address_hi=1|VERA_INC_8;
      for(i=0;i<8;i++)
        VERA.data0 = ((ZSM_MASK >> i) & 0x01) ? 0x0f : 0;
    }
  }
  else {
    if (leds_active) {
      VERA.control=0;
      VERA.address=0xFC00;
      VERA.address_hi=1|VERA_INC_8;
      for(i=0;i<24;i++) VERA.data0 = VRAM_LEDS & 0XFF;
    }
  }
  leds_active = leds_on;
}

void leds_update(uint8_t playing) {
  static uint8_t laststate=0xff;

  if (playing != laststate) {
    laststate = playing;
    leds_enable(playing);
  }
}

void leds_forceupdate(uint8_t leds_on) {
  leds_enable(!leds_on);
  leds_enable(leds_on);
}
