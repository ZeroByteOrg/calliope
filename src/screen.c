#include "screen.h"
#include <cbm.h>
#include <conio.h>
#include <stdio.h> // sprintf()


panel* panels[MAX_PANELS];
panel* activePanel;
uint8_t numPanels;

void print_list(panel* p);

char blankline[] = "%80s"; // token for cprintf() to use

void screen_init() {
  char i=0;
  clrscr();
  while(i<MAX_PANELS) panels[i++]=NULL;
  numPanels = 0;
  activePanel = NULL;
}

void screen_update() {
  char i, a;
  for (i=0;i<numPanels;i++) {
    //if (panels[i]->dirty) panel_draw(panels[i]);
    panel_draw(panels[i]);
    if(panels[i]==activePanel) a=i;
  }
  gotoxy (20,26);
  cprintf(
    "p:%u  items:%2d  slected:%2d  row:%2d col:%2d",
    a,
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
  gotoxy (35,2);
  if (isloading) {
    revers(1);
    cprintf("loading");
    revers(0);
  }
  else
    cprintf("       ");
}
