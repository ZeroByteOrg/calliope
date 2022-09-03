#ifndef _lists_h_
#define _lists_h_

#include <stdint.h>

#define LIST_MAX_SIZE 128

typedef struct {
  uint8_t count, selected, active;
  char* name[LIST_MAX_SIZE];
} itemlist;

extern void list_clear(itemlist* list);
extern char list_add(itemlist* list, const char* name);
extern char list_insert(itemlist* list, const char* name, uint8_t index);
extern void list_remove(itemlist* list, uint8_t index);
extern char list_move (itemlist* list, uint8_t from, uint8_t to);
extern char list_swap (itemlist* list, uint8_t from, uint8_t to);

extern char list_index(const itemlist* list, const char* target);

extern void list_sort(itemlist* list);
#endif
