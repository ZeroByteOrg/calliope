// Test program for new list printing / data structures.

#include <stdio.h>
#include <string.h>
#include <cbm.h>
#include "lists.h"

char foo[20] = "foo/bar";
char bar[20] = "bar";

void main() {
  printf("%s\n",foo);
  strrchr(foo,'/')[0]=0;
  bar[0]=0;
  printf("%s\n",foo);
  printf("%s\n",bar);
}
