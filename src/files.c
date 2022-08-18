#include "files.h"
#include <cbm.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define FILENAME_LEN 32

char* bload(const char* filename,const void* address) {
  // loads with current HiRAM bank = the one we want.
  // Even if it's not, it's the one we're gonna get! :)
	cbm_k_setnam(filename);
	cbm_k_setlfs(0,8,2);
	return (char*)cbm_k_load(0,(uint16_t)address);
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
  if (cbm_opendir(21,8)) {
    if (!cbm_readdir(21,item)) { // read but ignore volume header info
      list->count=0;
      list->active=0xff;
      while(!cbm_readdir(21,item)) {
        list->name[count] != NULL)

      }
    }
    cbm_closedir(21);
  }
}
