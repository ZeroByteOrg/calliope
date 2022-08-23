#ifndef _files_h_
#define _files_h_

#include "system.h"

extern void go_root();
extern signed char chdir(const char* dirname);
extern char* bload(const char* filename,const void* address);
extern void get_zsm_list(itemlist* list);
extern signed char get_dir_list(itemlist* list); // returns 1 if FS root dir

extern int lazy_load();
extern char init_lazy_load(const char* path, const char* filename, char bank, void* addr);

#endif
