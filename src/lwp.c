#include "../include/lwp.h"

static void lwp_wrap(lwpfun fun, void* arg);

static size_t thread_id_counter = 0;

static thread thread_list_head = NULL; 

// helper function
void print_thread(thread p_thread);
void print_all_threads();
void print_waitlist();

//function creates and returns the TID of the process it created
tid_t lwp_create(lwpfun function, void *argument) {
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

    // TODO: add to total thread list
    if (thread_list_head == NULL) {
        thread_list_head = thread_created;
        thread_list_head->lib_next = NULL;
    } else {
        thread_created->lib_next = thread_list_head;
        thread_list_head = thread_created;
    }

    return thread_created->tid;
}


void lwp_exit(int status) {
    print_all_threads();
}


static void lwp_wrap(lwpfun fun, void* arg) {
    int rval; 
    rval = fun(arg);
    lwp_exit(rval);
    return;
}


void print_thread(thread p_thread) {
    printf("Thread %lu {\n\tstack_addr = %p stack_size = 0x%X\n\t"
            "rfile = %s\n\tlib_next %p, lib_prev %p\n\tsched_next"
            " = %p, sched_prev = %p\n\texited = %p\n}\n", 
            p_thread->tid, p_thread->stack, p_thread->stacksize, 
            "tmp", p_thread->lib_next, p_thread->lib_prev, 
            p_thread->sched_next, p_thread->sched_prev, 
            p_thread->exited);

    return;
}


void print_all_threads() {
    thread cur = thread_list_head;
    do {
        print_thread(cur);
        cur = cur->lib_next;
    } while (cur);

    return;
}
