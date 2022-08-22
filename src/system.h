#ifndef _system_h_
#define _system_h_

#define NUM_QUALITY 6
#define NUM_SONGS 9

#define ZSMBANK 1
#define ZCMBANK 0x18

#define CLEAR_INPUT while(kbhit()) cgetc();

#define LIST_MAX_SIZE 128
#define LIST_ITEM_SIZE 18
#define LIST_Y 6

typedef struct item_s {
  char* name[LIST_MAX_SIZE];
  char count;
  char active;
  char scroll;
  char x;
  char y;
} itemlist;

#define FILENAME_LEN 32
#define MAX_PATH_LEN 128

struct workdir_s {
  signed char depth;
  char path[MAX_PATH_LEN];
  char root[3]; // either ./  or /  depending on whehter root is REAL root dir.
};

extern struct workdir_s workdir;

extern itemlist files, dirs, playlist;
extern itemlist *selected;

#endif
