#include "files.h"
#include <cbm.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // realloc()

#define FILENAME_LEN 32

struct workdir_s workdir;

char* bload(const char* filename,const void* address) {
  // loads with current HiRAM bank = the one we want.
  // Even if it's not, it's the one we're gonna get! :)
	cbm_k_setnam(filename);
	cbm_k_setlfs(0,8,2);
	return (char*)cbm_k_load(0,(uint16_t)address);
}

void go_root() {
  char d = workdir.depth;
  while (d) {
    chdir("..");
    --d;
  }
}

signed char chdir(char* dirname) {
  char cmd_buffer[FILENAME_LEN];

  // reset string length to 0
  cmd_buffer[0]=0;
  strcat(cmd_buffer,"cd:");
  strcat(cmd_buffer,dirname);

// debug: shows command being sent.
//  printf("%s\n", cmdbuffer);
// /debug
  cbm_k_setnam(cmd_buffer);
  cbm_k_setlfs(15,8,15);
  cbm_k_open();
  cbm_read(15,cmd_buffer,2);
  cbm_k_close(15);
  return (cmd_buffer[0]=='0' && cmd_buffer[1]=='0');
}

void get_zsm_list(itemlist* list) {
  struct cbm_dirent item;
  char l;
  list->count=0;
  list->scroll=0;
  list->active=0xff;
  if (!cbm_opendir(1,8)) {
    if (!cbm_readdir(1,&item)) { // read but ignore volume header info
      while(!cbm_readdir(1,&item)) {
        l = strlen(item.name);
        if (l<5) continue;
        if (strcmp(".zsm",item.name+l-4)!=0) continue;
        list->name[list->count] = realloc(list->name[list->count],l+1);
        strcpy(list->name[list->count],item.name);
        ++list->count;
      }
      if (list->count)
        list->active=0;
    }
    cbm_closedir(1);
  }
}

signed char get_dir_list(itemlist* list) {
  struct cbm_dirent item;
  char l;
  signed char root = 1;

  list->count=0;
  list->scroll=0;
  list->active=0;
  if (!cbm_opendir(1,8)) {
    if (!cbm_readdir(1,&item)) { // read but ignore volume header info
      while(!cbm_readdir(1,&item)) {
        if(item.type != 2) continue;
        if(strcmp(".",item.name)==0) continue; // skip . entry
        if(strcmp("..",item.name)==0) {
          root = 0;
          // disallow CD.. from initial directory
          if (workdir.depth == 0) continue;
        }
        l = strlen(item.name);
        list->name[list->count] = realloc(list->name[list->count],l+1);
        strcpy(list->name[list->count],item.name);
        ++list->count;
      }
    }
    cbm_closedir(1);
  }
  if (list->count) return root;
  else return 0;
}
