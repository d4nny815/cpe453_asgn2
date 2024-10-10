#include "../include/lwp.h"

static void lwp_wrap(lwpfun fun, void* arg);

static size_t thread_id_counter = 0;


// helper function
static void print_context(context p_thread_info);

//function creates and returns the TID of the process it created
tid_t lwp_create(lwpfun function, void *argument) {
    // ?init scheudler if doesnt exist?

    // create the thread
    thread thread_created = (thread) malloc(sizeof(context));
    if(thread_created == NULL) {
        return NO_THREAD; 
    }

    // assign it to the thread
    thread_created->tid = ++thread_id_counter;

    // allocate stack for the thread
    // ? why dont MAP_STACK and MAP_ANON not work?
    void *init_ptr = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, 
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK,
                        -1, 0);
    if(init_ptr == MAP_FAILED){
        perror("[LWP_CREATE] mmaping stack failed");
        free(thread_created);
        return NO_THREAD;
    }

    //fill in the thread struct details
    intptr_t sp = (intptr_t) init_ptr + STACK_SIZE;    
    thread_created->stack = (unsigned long*) sp;
    thread_created->stacksize = STACK_SIZE;

    // TODO: inject into the stack, return address, old base pointer*
    // place RA
    // place BP

    // TODO: create an initial rfile
    rfile* init_regs = (rfile*) calloc(1, sizeof(rfile));

    // TODO: set the register pointers we need
    //rps = stack pointer, rbp = base pointer, rdi = function argument
    init_regs->rsp = init_ptr;
    init_regs->rbp = init_ptr;
    init_regs->rdi = 0; 
    // TODO: dont forget about FPU too 


    // TODO: put the wrapper address into the right register 

    // TODO: put the function argument in the right register

    // admit new thread to the scheduler
    round_robin->admit(thread_created);

    print_context(*thread_created);
    
    // TODO: add to total thread list

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


static void print_context(context p_th_info) {
    printf("Thread %lu {\n\tstack_addr = %p stack_size = 0x%X\n\t"
            "rfile = %s\n\tlib_next %p, lib_prev %p\n\tsched_next"
            " = %p, sched_prev = %p\n\texited = %p\n}\n", 
            p_th_info.tid, p_th_info.stack, "rfile", 
            p_th_info.lib_next, p_th_info.lib_prev, 
            p_th_info.sched_next, p_th_info.sched_prev, 
            p_th_info.exited);

    return;
}
