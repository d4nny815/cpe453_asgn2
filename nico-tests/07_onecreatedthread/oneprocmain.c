/*
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "lwp.h"

static void indentnum(uintptr_t num) {
  /* print the number num num times, indented by 5*num spaces
   * Not terribly interesting, but it is instructive.
   */
  printf("Greetings from Thread %d.  Goodbye\n",(int)num);
  exit(0);
}


int main(int argc, char *argv[]){
  /* spawn a number of individual LWPs */
  lwp_create((lwpfun)indentnum,(void*)3);

  lwp_start();
  lwp_wait(NULL);

  return 0;
}


