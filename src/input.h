#ifndef _input_h_
#define _input_h_
#include <stdint.h>

#define SNES_UP     (1 << 3)
#define SNES_DOWN   (1 << 2)
#define SNES_LEFT   (1 << 1)
#define SNES_RIGHT  (1 << 0)
#define SNES_A      (1 << 15)
#define SNES_B      (1 << 7)
#define SNES_X      (1 << 14)
#define SNES_Y      (1 << 6)
#define SNES_L      (1 << 13)
#define SNES_R      (1 << 12)
#define SNES_SEL    (1 << 5)
#define SNES_START  (1 << 4)

typedef struct{
  uint16_t current;
  uint16_t last;
  uint16_t pressed;
  uint16_t released;
}joystick;

extern void input_init();
extern void joystick2kbd(char id);

// needs to return something, like a command enum?
extern void input();


#endif
