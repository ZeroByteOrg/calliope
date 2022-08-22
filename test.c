#include <stdio.h>
#include <string.h>

char foo[20] = "foo/bar";
char bar[20] = "bar";

void main() {
  printf("%s\n",foo);
  strrchr(foo,'/')[0]=0;
  bar[0]=0;
  printf("%s\n",foo);
  printf("%s\n",bar);
}
