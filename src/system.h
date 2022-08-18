#ifndef _system_h_
#define _system_h_

#define NUM_QUALITY 6
#define NUM_SONGS 9

#define ZSMBANK 1
#define ZCMBANK 0x18

#define CLEAR_INPUT while(kbhit()) cgetc();

#define LIST_MAX_SIZE 128
#define LIST_ITEM_SIZE 18

typedef struct item_s {
  char* name[LIST_MAX_SIZE];
  char count;
  char active;
} itemlist;


#endif
