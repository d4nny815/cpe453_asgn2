#include "../include/lwp.h"


// Create:
// - create thread ID (counter)
// - allocate a stack for it (mmap will return start of chunk)
// *inject into the stack, return address, old base pointer*
// - create an innitial rfile
// *put the wrapper address into the respective register*
// - put the function argument into the respective register
// - admit it into the scheduler 



// typedef struct threadinfo_st *thread;
// typedef struct threadinfo_st {
//   tid_t         tid;            /* lightweight process id  */
//   unsigned long *stack;         /* Base of allocated stack */
//   size_t        stacksize;      /* Size of allocated stack */
//   rfile         state;          /* saved registers         */
//   unsigned int  status;         /* exited? exit status?    */
//   thread        lib_one;        /* Two pointers reserved   */
//   thread        lib_two;        /* for use by the library  */
//   thread        sched_one;      /* Two more for            */
//   thread        sched_two;      /* schedulers to use       */
//   thread        exited;         /* and one for lwp_wait()  */
// } context;

size_t thread_id_counter = 0;


//funcion creates and returns the TID of the process it created
tid_t lwp_create(lwpfun function, void *argument) {

    //first create the thread
    thread thread_created = (thread) malloc(sizeof(struct threadinfo_st));

    //check if that mallocking worked or nah
    if(thread_created == NULL) {
        return NO_THREAD; 
    }

    //must first create a thread ID and then assign it to the thread
    thread_created->tid = thread_id_counter;

    //make sure the global ID counts up
    thread_id_counter++;

    //mmap a portion of the stack for the thread
    void *init_ptr = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, 
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    //check if that failed
    if(init_ptr == MAP_FAILED){
        perror("mmaping stack failed");
        free(thread_created);
        return NO_THREAD;
    }

    //fill in the thread struct details
    intptr_t sp = (intptr_t) init_ptr + STACK_SIZE;    
    thread_created->stack = (size_t) sp;
    thread_created->stacksize = STACK_SIZE;

    // *inject into the stack, return address, old base pointer*

    //create an initial rfile
    rfile* init_regs = (rfile*) calloc(1, sizeof(rfile));

    //set the register pointers we need
    //rps = stack pointer, rbp = base pointer, rdi = function argument
    init_regs->rsp = init_ptr;
    init_regs->rbp = init_ptr;
    init_regs->rdi = 0; 

    //put the wrapper address into the right register 

    //put the function argument in the right register

    //once created, admit it to the scheduler
    // admit(thread_created);
    return thread_created->tid;
}


void lwp_exit(int status) {
    printf("hello world\n");
}