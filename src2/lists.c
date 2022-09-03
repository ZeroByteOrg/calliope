#include "lists.h"
#include <string.h>
#include <stdlib.h>

char blank[1] = { 0 }; // placeholder blank list item name

void list_clear(itemlist* list) {
  uint8_t i;
  for(i=0;i<LIST_MAX_SIZE;i++)
    if (list->name[i] != blank)
      if (list->name[i] != NULL) {
        free(list->name[i]);
        list->name[i] = blank;
      }
  list->count=0;
  list->selected=0xff;
  list->active=0xff;
}

char list_add(itemlist* list, const char* name) {
  char* tmp;

  if(list->count >= LIST_MAX_SIZE) return 0; // list full

  // allocate memory for the permanent copy of the name being added
  // and copy the name into the new memory if successful.
  tmp = malloc(strlen(name)+1);
  if (tmp == NULL) return 0;  // out of memory?
  strcpy(tmp,name);

  // make sure no old name still exists for this entry.
  if (list->name[list->count] != NULL)
    if (list->name[list->count] != blank)
      free(list->name[list->count]);
  list->name[list->count] = tmp;
  ++list->count;
}

char list_insert(itemlist* list, const char* name, uint8_t index) {

  // just add to end and then move it into place
  if (index > list->count) return 0; // invalid index
  if (list_add(list, name)==0) return 0;
  list_move(list,list->count-1,index);
  return 1;
}

void list_remove(itemlist* list, uint8_t index) {
  // move item to end, destroy the string and --count
  if(index >= list->count) return;
  list_move(list,index,list->count-1);
  --list->count;
  if (list->name[list->count] != blank)
    free(list->name[list->count]);
  if (list->selected == list->count) list->selected=0xff;
  if (list->active == list->count) list->active=0xff;
}

char list_move (itemlist* list, uint8_t from, uint8_t to) {
  char* tmp;
  uint8_t i;

  if (from == to) return 1;
  if (from >= list->count || to >= list->count ) return 0;
  if (from>=to) {
    tmp = list->name[from];
    for (i=from;i>to;--i)
      list->name[i] = list->name[i-1];
    list->name[to] = tmp;

    if (list->selected == from)
      list->selected = to;
    else if (list->selected >= to && list->selected < from)
      ++list->selected;

    if (list->active == from)
      list->active = to;
    else if (list->active >= to && list->active < from)
      ++list->active;
  }
  else {
    tmp = list->name[from];
    for (i=from;i<to;++i)
      list->name[i] = list->name[i+1];
    list->name[to] = tmp;

    if (list->selected == from)
      list->selected = to;
    else if (list->selected > from && list->selected <= to)
      --list->selected;

    if (list->active == from)
      list->active = to;
    else if (list->active > from && list->active <= to)
      --list->active;
  }
  return 1;
}

char list_swap (itemlist* list, uint8_t from, uint8_t to) {
  char* tmp;

  if (from>=list->count || to>=list->count) return 0;
  if (from==to) return 1;
  tmp = list->name[from];
  list->name[from] = list->name[to];
  list->name[to] = tmp;
  if (list->selected==from)
    list->selected=to;
  else if (list->selected==to)
    list->selected=from;
  if (list->active==from)
    list->active=to;
  else if (list->active==to)
    list->active=from;
  return 1;
}

// selection sort
void list_sort (itemlist* list) {
  uint8_t i, j, max;
  char* s;
  char** item=list->name;

  for (i=list->count-1 ; i>=1 ; i--) {
    max=i;
    s=item[max];
    for (j=0;j<i;j++) {
      if (strcmp(s,item[j])<0) {
        s=item[j];
        max=j;
      }
    }
    if (max != i) list_swap(list,max,i);
  }
}

char list_index(const itemlist* list, const char* target) {
  uint8_t n;

  if (list->count >= LIST_MAX_SIZE) return 0xff;
  for (n=0 ; n<list->count ; n++) {
    if (strcmp(list->name[n],target)==0) return n;
  }
  return 0xff;
}
