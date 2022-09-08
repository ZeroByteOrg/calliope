#include "files.h"
#include "screen.h"
#include <cbm.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define FILENAME_LEN 32

struct workdir_s workdir;

char ll_bank;
char* ll_addr;

char ll_working=0;

char loaded_song[FILENAME_LEN] = "";
char loaded_path[128] = "";

extern int __fastcall__ macptr(char nbytes, void* addr);
// defined in macptr.asm

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

char already_loaded(const char* path, const char* file) {
	if(strcmp(path,loaded_path)==0 && strcmp(file,loaded_song)==0)
		return 1;
	else
		return 0;
}

signed char chdir(const char* dirname) {
  char cmd_buffer[FILENAME_LEN];

	if (strlen(dirname)==0) return 1;
	if (strlen(dirname)==1 && dirname[0]=='.') return 1;
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
	list_clear(list);
  if (!cbm_opendir(1,8)) {
    if (!cbm_readdir(1,&item)) { // read but ignore volume header info
      while(!cbm_readdir(1,&item)) {
        l = strlen(item.name);
        if (l<5) continue;
        if (strcmp(".zsm",item.name+l-4)!=0) continue;
				list_add(list,item.name);
      }
    }
    cbm_closedir(1);
		list_sort(list);
  }
}

signed char get_dir_list(itemlist* list) {
  struct cbm_dirent item;
  signed char root = 1;

	list_clear(list);
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
				list_add(list, item.name);
      }
    }
    cbm_closedir(1);
  }
	list_sort(list);
	list_move(list, list_index(list, ".."), 0);
  if (list->count) return root;
  else return 0;
}

char start_lazy_load(const char* path, const char* filename, char bank, void* addr) {
	if (already_loaded(path,filename)) {
		// the loaded song is the song being requested. Just leave
		// the lazy loader alone and return true
		return 1;
	}
	if (ll_working) {
		cbm_close(LAZY_LFN);
	}
  ll_working = 0;
	go_root();
	if(chdir(path)) {
  	if (cbm_open(LAZY_LFN,8,LAZY_LFN,filename)!=0) return 0;
  	ll_bank = bank;
  	ll_addr = (char*)addr;
		strcpy(loaded_path,path);
		strcpy(loaded_song,filename);
		print_loading(1);
  	go_root();
    ll_working = 1;
  }
	else {
		loaded_path[0]=0;
		loaded_song[0]=0;
	}
	chdir(workdir.path);
	return ll_working;
}

void stop_lazy_load() {
	if (ll_working) {
		cbm_close(LAZY_LFN);
		ll_working=0;
		loaded_path[0]=0;
		loaded_song[0]=0;
		print_loading(0);
	}
}

int lazy_load() {
	char b;
	int n=0;
  if (!ll_working) return 0;
	if (!cbm_k_chkin(LAZY_LFN)) {
		b=RAM_BANK;
		RAM_BANK = ll_bank;
		n=macptr(0, ll_addr);
		ll_addr += n;
		if (ll_addr >= (char*)0xc000) ll_addr -= 0x2000;
		ll_bank =RAM_BANK;
		RAM_BANK = b;
	}
	if (n<1) {
		ll_working=0;
		cbm_close(LAZY_LFN);
		n=-1;
	}
	return n;
}

char load(const char* path, const char* filename, char bank, void* addr) {
  char b,b1;
  char* a;
	if (already_loaded(path,filename)) return 1;
	if (ll_working) {
		cbm_close(LAZY_LFN);
		ll_working=0;
	}
  go_root();
  if(!chdir(path)) return 0;
  b = RAM_BANK;
  RAM_BANK=bank;
  a=ll_addr;
  b1=RAM_BANK;
  ll_addr=bload(filename,addr);
  ll_bank=RAM_BANK;
  RAM_BANK = b;
  go_root();
  chdir(workdir.path);
  if (b1 != ll_bank || a != ll_addr) {
		// success
		strcpy(loaded_song,filename);
		strcpy(loaded_path,path);
		return 1;
	}
	else {
		loaded_path[0]=0;
		loaded_song[0]=0;
		return 0;
	}
}

unsigned int load_asset(const char* filename, char bank, void* addr) {
	cbm_k_setnam(filename);
	cbm_k_setlfs(0,8,0);
	return (unsigned int)cbm_k_load(bank+2,addr);
}
