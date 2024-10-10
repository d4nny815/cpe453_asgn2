#include "../include/lwp.h"
#include ""

static void lwp_wrap(lwpfun fun, void* arg);

static size_t thread_id_counter = 0;

static scheduler round_robin = NULL;


//funcion creates and returns the TID of the process it created
tid_t lwp_create(lwpfun function, void *argument) {
    // init scheudler if doesnt exist
    if (round_robin == NULL) {
        round_robin = (scheduler) malloc(sizeof(struct scheduler));
    }    


    //first create the thread
    thread thread_created = (thread) malloc(sizeof(struct threadinfo_st));
    if(thread_created == NULL) {
        return NO_THREAD; 
    }

    //must first create a thread ID and then assign it to the thread
    thread_created->tid = ++thread_id_counter;

    //mmap a portion of the stack for the thread
    void *init_ptr = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, 
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if(init_ptr == MAP_FAILED){
        perror("mmaping stack failed");
        free(thread_created);
        return NO_THREAD;
    }

    //fill in the thread struct details
    intptr_t sp = (intptr_t) init_ptr + STACK_SIZE;    
    thread_created->stack = (size_t) sp;
    thread_created->stacksize = STACK_SIZE;

    // inject into the stack, return address, old base pointer*
    // place RA
    // place BP

    //create an initial rfile
    rfile* init_regs = (rfile*) calloc(1, sizeof(rfile));

    //set the register pointers we need
    //rps = stack pointer, rbp = base pointer, rdi = function argument
    init_regs->rsp = init_ptr;
    init_regs->rbp = init_ptr;
    init_regs->rdi = 0; 
    // dont forget about FPU too 


    //put the wrapper address into the right register 

    //put the function argument in the right register

    //once created, admit it to the scheduler
    // admit(thread_created);
    return thread_created->tid;
}


void lwp_exit(int status) {
    printf("hello world\n");
}


static void lwp_wrap(lwpfun fun, void* arg) {
    int rval; 
    rval = fun(arg);
    lwp_exit(rval);
    return;
}
