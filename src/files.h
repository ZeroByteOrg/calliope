#ifndef _files_h_
#define _files_h_

#include "system.h"

#define FILENAME_LEN 32

extern signed char chdir(char* dirname);
extern char* bload(const char* filename,const void* address);
extern void get_zsm_list(itemlist* list);


#endif
