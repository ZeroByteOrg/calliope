#include "input.h"
#include "screen.h"
#include "lists.h"
#include <cbm.h>
#include <conio.h>

// no tab key on CBM machines, yet they didn't define this in CX16.H!?!?!??
#define CH_TAB 9

// define in assembly:
extern void __fastcall__ cx16_k_kbdbuf_put(const char key);

// returns boolean true/false for "joystick presemt". If present, then
// value is stored into *state.
extern char __fastcall__ cx16_k_joystick_get(const char id, unsigned int* state);

// Function declarations:
char poll_joystick(char id);


joystick joy1 = { 0,0,0,0 };

void input_init() {
}

//updates the input sources
void input() {
  return; // this routine is for later after more code refactors.

  //WIP:
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

char poll_joystick(char id) {
  if (!cx16_k_joystick_get(id,&joy1.current)) return 0;
  joy1.current  = ~joy1.current; // flip bits to use 1=pressed logic.
  joy1.pressed  = ~joy1.last & joy1.current;
  joy1.released =  joy1.last & ~joy1.current;
  joy1.last     =  joy1.current;
  return 1;
}

void joystick2kbd(char id) {
  if (!poll_joystick(id)) return;
  if (joy1.pressed & SNES_UP) cx16_k_kbdbuf_put(CH_CURS_UP);
  if (joy1.pressed & SNES_DOWN) cx16_k_kbdbuf_put(CH_CURS_DOWN);
  if (joy1.pressed & SNES_LEFT) cx16_k_kbdbuf_put(CH_CURS_LEFT);
  if (joy1.pressed & SNES_RIGHT) cx16_k_kbdbuf_put(CH_CURS_RIGHT);
  if (joy1.pressed & (SNES_A|SNES_X)) cx16_k_kbdbuf_put(' ');
  if (joy1.pressed & (SNES_B|SNES_Y)) cx16_k_kbdbuf_put(CH_ENTER);
  if (joy1.pressed & (SNES_L|SNES_R|SNES_SEL)) cx16_k_kbdbuf_put(CH_TAB);
}
