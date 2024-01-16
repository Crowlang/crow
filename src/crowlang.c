#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

#include <crow/types.h>
#include <crow/math.h>
#include <crow/core.h>
#include <crow/io.h>
#include <crow/data.h>
#include <crow/funcond.h>
#include <crow/system.h>
#include <crow/string.h>

static CRO_State *s;

static void handler (int sig) {
  /* TODO: Make this work with the new core */
  CRO_freeState(s);
  exit(0);
}

int main (int argc, char *argv[]) {
  FILE *src;

  CRO_initColor();
  signal(SIGINT, handler);

  s = CRO_createState();

  CRO_exposeStandardFunctions(s);

  if (argc >= 1) {
    /* TODO: Manually expose arguements here */
  }

  src = stdin;

  /* TODO: Make this prettier */
  while (!feof(src)) {
    putchar('%');
    putchar(' ');
    puts(CRO_printStd(s, CRO_eval(s, readWord(s, src))));
    CRO_callGC(s);
  }


  return 0;
}
