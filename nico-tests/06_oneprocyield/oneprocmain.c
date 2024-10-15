/*
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "lwp.h"

int main(int argc, char *argv[]){
  lwp_start();
  printf("Greetings from the original thread.  Yielding...\n");
  lwp_yield();
  printf("I'm still alive.  Goodbye\n");

  return 0;
}


