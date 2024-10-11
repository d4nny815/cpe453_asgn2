#include "../include/lwp.h"

static void lwp_wrap(lwpfun fun, void* arg);

static size_t thread_id_counter = 0;

static thread threadlist_head = NULL; 
static thread waitlist_head = NULL;
static thread cur_thread = NULL;

// helper function
void print_thread(thread p_thread);
void print_all_threads();
void print_waitlist();

// function creates and returns the TID of the process it created
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
    if (init_ptr == MAP_FAILED){
        perror("mmaping stack failed");
        free(thread_created);
        return NO_THREAD;
    }

    // fill in the thread struct details
    intptr_t old_bp = (intptr_t) init_ptr;    
    void* p_fr_bp = (void*)((intptr_t) init_ptr + STACK_SIZE);
    thread_created->stack = (unsigned long*) p_fr_bp; // ? this be ptr to useable stack
    thread_created->stacksize = STACK_SIZE;

    // inject into the stack, return address, old base pointer*
    unsigned long* p_stack = (unsigned long*) p_fr_bp;;
    // ? is this correct type casting
    p_stack[-1] = (unsigned long) lwp_wrap; // RA 
    p_stack[-2] = (unsigned long) old_bp;

    // create an initial rfile
    rfile* init_rfile = &thread_created->state;

    // set the register pointers we need
    intptr_t sp = (intptr_t) init_ptr + STACK_SIZE - 2 * WORD_SIZE;
    init_rfile->rsp = (intptr_t) init_ptr + STACK_SIZE - 
                        2 * WORD_SIZE;
    init_rfile->rdi = (unsigned long) function; 
    init_rfile->rsi = (unsigned long) argument; 
    init_rfile->fxsave = FPU_INIT;

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

void lwp_start(void){
    // create a thread to be the main process 
    thread thread_main = (thread) malloc(sizeof(context));

    if(thread_main == NULL) {
        return; 
    }

    // assign it to the thread
    thread_main->tid = ++thread_id_counter;
    thread_main->stack = NULL; // ? do I have to put the actual stack size that main has here? or hecksnaw
    thread_main->stacksize = 0;

    // main thread is the current thread
    cur_thread = thread_main;

    // admit new thread to the scheduler
    round_robin->admit(thread_main);

    //  call yeild
    lwp_yield();
}


void lwp_yield(void){
    // find what the next thread in the scheduler is
    thread next_thread = round_robin->next();

    // take the current thread and make copy so you can update it to next curr
    thread old_thread = cur_thread;    
    cur_thread = next_thread;

    // swap that hoeeeee
    swap_rfiles(&old_thread->state, &cur_thread->state);
}

// tid_t lwp_wait(int *status){

// }






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
