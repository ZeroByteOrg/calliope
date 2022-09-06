#include "input.h"
#include "screen.h"
#include "lists.h"
#include <cbm.h>
#include <conio.h>

void handle_input() {
  if (kbhit()) switch(cgetc()) {
    case CH_ENTER:
      break;
//    case CH_TAB:
//      break;
    case CH_SHIFT_TAB:
      break;

    case CH_CURS_UP:

    case CH_CURS_DOWN:
    case CH_CURS_LEFT:
    case CH_CURS_RIGHT:
    case CH_HOME:
      break;
  }
}
