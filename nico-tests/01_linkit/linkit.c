#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <lwp.h>

int main(int argc, char *argv[]){
  if ( argc == -1 ) {
    /* can't happen, but the linker doesn't know it.  */
    /* these are all the required external symbols */
    lwp_create(NULL,NULL);
    lwp_exit(3);
    lwp_yield();
    lwp_start();
    lwp_wait(NULL);
    lwp_set_scheduler(NULL);
  }
  printf("Linked successfully.\n");
  exit(0);
}
