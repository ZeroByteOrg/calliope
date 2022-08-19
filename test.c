#include <cbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern signed char chdir(char* dirname);

char foo[20] = "";
char bar[20] = "this is a test";

void main() {
  strcpy(foo,bar);
  printf("%s (%d)\n",foo,strlen(foo));
  printf("cd %-5s : %s\n","foo",chdir("foo") ? "success" : "fail");
  printf("cd %-5s : %s\n","bar",chdir("bar") ? "success" : "fail");
  printf("cd %-5s : %s\n","/bar",chdir("/bar") ? "success" : "fail");
}
