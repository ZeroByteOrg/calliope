#ifndef _system_h_
#define _system_h_

#define NUM_QUALITY 6
#define NUM_SONGS 9

#define ZSMBANK 1
#define ZCMBANK 0x18

#define CLEAR_INPUT while(kbhit()) cgetc();

#define LIST_MAX_SIZE 128
#define LIST_ITEM_SIZE 18

#define MAIN_Y 5
#define MAIN_END 27

#define PLAYLIST_X 0
#define PLAYLIST_Y (MAIN_Y)
#define PLAYLIST_LEN ((MAIN_END)-(PLAYLIST_Y) + 1)

#define DIRLIST_X 22
#define DIRLIST_Y (MAIN_Y)
#define DIRLIST_LEN ((MAIN_END)-(DIRLIST_Y) + 1)

#define FILELIST_X 42
#define FILELIST_Y (MAIN_Y)
#define FILELIST_LEN ((MAIN_END)-(FILELIST_Y) + 1)

#define LAZY_LFN 14 // LFN the lazy loader should use

typedef struct item_s {
  char* name[LIST_MAX_SIZE];
  char count;
  char active;
  char scroll;
  char redraw;
} itemlist;

#define FILENAME_LEN 32
#define MAX_PATH_LEN 128

struct workdir_s {
  signed char depth;
  char path[MAX_PATH_LEN];
  char root[3]; // either ./  or /  depending on whehter root is REAL root dir.
};
extern struct workdir_s workdir;

enum tab_e {
  TAB_DIRS,
  TAB_FILES,
  TAB_PLAYLIST,
  TAB_COUNT
};

struct tab_s {
  itemlist* list[TAB_COUNT];
  signed char selected;
  signed char last;
};

extern itemlist files, dirs, playlist, play_paths;
extern struct tab_s tabs;

#endif
