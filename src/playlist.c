#include "system.h"
#include "playlist.h"
#include <string.h>
#include <stdlib.h>

void playlist_add(char* path, char* file) {
  char n;
  char *tmp1;
  char *tmp2;
  char *s;

  if (playlist.count >= LIST_MAX_SIZE) return;
  for (n=0 ; n<playlist.count ; n++) {
    if (
      strcmp(playlist.name[n],file)==0
      &&
      strcmp(play_paths.name[n],path)==0
    ) return;
  }
  tmp1 = malloc(strlen(file)+1);
  if (tmp1 == NULL) return;
  tmp2 = malloc(strlen(path)+1);
  if (tmp2 == NULL) {
    free(tmp1);
    return;
  }
  strcpy(tmp1,file);
  strcpy(tmp2,path);
  n = playlist.count;
  s = playlist.name[n];
  if (s != NULL) free(s);
  playlist.name[n] = tmp1;
  ++playlist.count;
  s = play_paths.name[n];
  if (s != NULL) free(s);
  play_paths.name[n] = tmp2;
  ++play_paths.count;
}

void playlist_remove(char item) {
  if (item >= playlist.count) return;
  if (playlist.name[item]!=NULL) free(playlist.name[item]);
  if (play_paths.name[item]!=NULL) free(play_paths.name[item]);
  while (item<playlist.count) {
    playlist.name[item] = playlist.name[item+1];
    play_paths.name[item] = play_paths.name[item+1];
    ++item;
  }
  --playlist.count;
  --play_paths.count;
}
