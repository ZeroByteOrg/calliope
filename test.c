#include <cbm.h>
#include <stdio.h>

extern signed char chdir(char* dirname);

void main() {
  printf("cd %-5s : %s\n","foo",chdir("foo") ? "success" : "fail");
  printf("cd %-5s : %s\n","bar",chdir("bar") ? "success" : "fail");
  printf("cd %-5s : %s\n","/bar",chdir("/bar") ? "success" : "fail");
}
