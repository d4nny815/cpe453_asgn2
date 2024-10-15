#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "lwp.h"
#include "schedulers.h"

#define NUMTHREADS 1
#define ITERS 1

#define tnext sched_one
#define tprev sched_two

typedef void (*sigfun)(int signum);
extern thread randomGetHead(void);

int main(int argc, char *argv[]){
  thread t,head;

  lwp_set_scheduler(Random);

  printf("Starting LWPS.\n");
  lwp_start();

  head=randomGetHead();
  tid_t tid = head->tid;
  t = tid2thread(tid+1);
  if ( t ) {
    fprintf(stderr,
            "tid2thread() returned a value with a bogus tid.  Not good\n");
  } else {
    fprintf(stdout,
            "tid2thread() coped with a bad value.\n");
  }
  printf("Back from LWPS.\n");
  lwp_exit(0);
  return 0;
}

