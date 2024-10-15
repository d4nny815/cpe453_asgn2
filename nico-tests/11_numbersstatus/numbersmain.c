
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "lwp.h"

static void indentnum(uintptr_t num);

int main(int argc, char *argv[]){
  long i;
  int status;

  printf("Launching LWPS\n");

  /* spawn a number of individual LWPs */
  for(i=1;i<=5;i++) {
    lwp_create((lwpfun)indentnum,(void*)i);
  }

  lwp_start();                     /* returns when the last lwp exits */

  for(i=1;i<=5;i++) {
    lwp_wait(&status);
    printf("A thread exited with status %d\n",LWPTERMSTAT(status));
  }
  printf("Back from LWPS.\n");
  return 0;
}

static void indentnum(uintptr_t num) {
  /* print the number num num times, indented by 5*num spaces
   * Not terribly interesting, but it is instructive.
   */
  int howfar,i;

  howfar=(int)num;              /* interpret num as an integer */
  for(i=0;i<howfar;i++){
    printf("%*d\n",howfar*5,howfar);
    lwp_yield();                /* let another have a turn */
  }
}

