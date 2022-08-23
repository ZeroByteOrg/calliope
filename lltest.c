#include <cbm.h>
#include "src/files.h"

const char path[16]="zsm";
const char file[16]="greenhill.zsm";

void main() {
  workdir.depth=0;
  workdir.path[0]=0;
  workdir.root[0]=0;
  init_lazy_load(path,file,1,(void*)0xa000);
  while(1) {
    lazy_load();
  }
}
