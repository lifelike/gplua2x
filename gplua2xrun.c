#include "gplua2x.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv) {
   if (argc < 2) {
      fprintf(stderr, "usage: %s luafilename\n", argv[0]);
   } else {
      lua2x_run_file(argv[1], NULL, NULL);
   }
#ifdef GP2X
   // if lua2x_run_file was successful on GP2X we will not come here
    chdir("/usr/gp2x");
    execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif
   return 0;
}

