#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <lwp.h>

#define N 50

static void counter(long num)
{
  if ( num ) {
    lwp_create((lwpfun)counter,(void*)num-1);
    printf("Counter is %ld.  Exiting\n",num);
  } else {
    printf("Bye\n");
  }
  lwp_exit(0);
}


int main() {
  int i;
  lwp_create((lwpfun)counter,(void*)N);
  lwp_create((lwpfun)counter,(void*)N);
  lwp_start();
  for(i=0;i<2*N+2;i++)
    lwp_wait(NULL);
  exit(0);
}
