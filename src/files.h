#ifndef _files_h_
#define _files_h_

#include "lists.h"

#define FILENAME_LEN 32
#define MAX_PATH_LEN 128
#define LAZY_LFN 14 // LFN the lazy loader should use

struct workdir_s {
  signed char depth;
  char path[MAX_PATH_LEN];
  char root[3]; // either ./  or /  depending on whehter root is REAL root dir.
};
extern struct workdir_s workdir;

extern void go_root();
extern signed char chdir(const char* dirname);
extern char* bload(const char* filename,const void* address);
extern void get_zsm_list(itemlist* list);
extern signed char get_dir_list(itemlist* list); // returns 1 if FS root dir

extern int lazy_load();
extern char init_lazy_load(const char* path, const char* filename, char bank, void* addr);
extern char load(const char* path, const char* filename, char bank, void* addr);

#endif
