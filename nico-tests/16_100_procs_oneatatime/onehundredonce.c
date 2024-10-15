#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <lwp.h>

static void counter(long num)
{
  if ( num ) {
    lwp_create((lwpfun)counter,(void*)num-1);
    printf("%ld\n",num);
  } else {
    printf("Bye\n");
  }
  lwp_exit(0);
}


#define N 100
int main() {
  int i;
  lwp_create((lwpfun)counter,(void*)N);
  lwp_start();
  for(i=0;i<N;i++)
    lwp_wait(NULL);
  exit(0);
}
