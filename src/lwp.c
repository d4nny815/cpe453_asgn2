#include "../include/lwp.h"

static void lwp_wrap(lwpfun fun, void* arg);

static size_t thread_id_counter = 0;

static thread threadlist_head = NULL; 
static thread waitlist_head = NULL;

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
    void *init_ptr = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, 
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK,
                        -1, 0);
    if(init_ptr == MAP_FAILED){
        perror("mmaping stack failed");
        free(thread_created);
        return NO_THREAD;
    }

    //fill in the thread struct details
    intptr_t sp = (intptr_t) init_ptr + STACK_SIZE;    
    intptr_t bp = (intptr_t) init_ptr + STACK_SIZE - 8;    
    thread_created->stack = (unsigned long*) sp;
    thread_created->stacksize = STACK_SIZE;

    // inject into the stack, return address, old base pointer*
    intptr_t* p_stack = (intptr_t*) init_ptr;
    p_stack[0] = (intptr_t) lwp_wrap;
    p_stack[1] = (intptr_t) bp;

    // create an initial rfile
    rfile* init_rfile = &thread_created->state;

    // set the register pointers we need
    //rps = stack pointer, rbp = base pointer, rdi = function argument
    init_regs->rsp = init_ptr;
    init_regs->rbp = init_ptr;
    init_regs->rdi = 0; 
    // dont forget about FPU too 

    //put the wrapper address into the right register 
    init_rfile->rsp = sp + 16;
    init_rfile->rbp = bp;
    init_rfile->rdi = 0; 
    init_rfile->fxsave = FPU_INIT;

    // ? this need to be the args into lwp_wrap?
    // ? how i set up the stack?

    // TODO: put the wrapper address into the right register 

    // TODO: put the function argument in the right register

    // admit new thread to the scheduler
    round_robin->admit(thread_created);

    // add to total thread list
    if (threadlist_head == NULL) {
        threadlist_head = thread_created;
        threadlist_head->lib_tl_next = NULL;
    } else {
        thread_created->lib_tl_next = threadlist_head;
        threadlist_head = thread_created;
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
            "rfile = %s\n\tlib_tl_next %p, lib_prev %p\n\tsched_next"
            " = %p, sched_prev = %p\n\texited = %p\n}\n", 
            p_thread->tid, p_thread->stack, p_thread->stacksize, 
            "tmp", p_thread->lib_tl_next, p_thread->lib_wl_next, 
            p_thread->sched_next, p_thread->sched_prev, 
            p_thread->exited);

    return;
}


void print_all_threads() {
    thread cur = threadlist_head;
    do {
        print_thread(cur);
        cur = cur->lib_tl_next;
    } while (cur);

    return;
}
