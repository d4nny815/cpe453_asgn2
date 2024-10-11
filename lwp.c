#include "lwp.h"

static void lwp_wrap(lwpfun fun, void* arg);
static thread remove_wait_list();
static void add_2_wait_list(thread thread_to_add);
static void insert_waitlist_head(thread thread_to_insert);

static size_t thread_id_counter = 0;

static thread threadlist_head = NULL; 
static thread waitlist_head = NULL;
static thread waitlist_tail = NULL;
static thread cur_thread = NULL;
static tid_t main_id = 0;

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
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (init_ptr == MAP_FAILED){
        perror("mmaping stack failed");
        free(thread_created);
        return NO_THREAD;
    }

    // fill in the thread struct details
    intptr_t old_bp = (intptr_t) init_ptr;    
    void* p_fr_bp = (void*)((intptr_t) init_ptr + STACK_SIZE);
    // ? this be ptr to useable stack
    thread_created->stack = (unsigned long*) p_fr_bp; 
    thread_created->stacksize = STACK_SIZE;
    thread_created->stack = LWP_LIVE;

    // inject into the stack, return address, old base pointer*
    unsigned long* p_stack = (unsigned long*) p_fr_bp;;
    // ? is this correct type casting
    p_stack[-1] = (unsigned long) lwp_wrap; // RA 
    p_stack[-2] = (unsigned long) old_bp;

    // create an initial rfile
    rfile* init_rfile = &thread_created->state;

    // set the register pointers we need
    intptr_t sp = (intptr_t) init_ptr + STACK_SIZE - 2 * WORD_SIZE;
    init_rfile->rsp = (unsigned long) sp;
    init_rfile->rbp = (unsigned long) sp;
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


void lwp_start(void){
    // create a thread to be the main process 
    thread thread_main = (thread) malloc(sizeof(context));

    if(thread_main == NULL) {
        return; 
    }

    // assign it to the thread
    thread_main->tid = ++thread_id_counter;
    main_id = thread_main->tid;
    // ? do I have to put the actual stack size that main has here? or hecksnaw
    thread_main->stack = NULL; 
    thread_main->stacksize = 0;
    thread_main->status = LWP_LIVE;

    // main thread is the current thread
    cur_thread = thread_main;

    // admit new thread to the scheduler
    round_robin->admit(thread_main);

    //  call yield
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


void lwp_exit(int exitval) {
    // pop off the head 
    // check if the term status is not term (it is waiting)
    //      put back in scheduler 
    // else 
    //      insert it into the waitlist before the head

    if (cur_thread == NULL) {
        return;
    }

    thread wl_thread = remove_wait_list();
    if (!LWPTERMINATED(wl_thread->status)) {
        round_robin->admit(wl_thread);
    } else {
        insert_waitlist_head(wl_thread);
    }

    // change status
    cur_thread->status = MKTERMSTAT(LWP_TERM, exitval);

    // add to the wait list 
    add_2_wait_list(cur_thread);

    // remove from the scheduler
    round_robin->remove(cur_thread);

    lwp_yield();

    return;
}


// ? what is status for?
tid_t lwp_wait(int *status) {
    thread terminated_thread;
    if (waitlist_head) { 
        // take the oldest terminated therad out from the waitlist 
        terminated_thread = remove_wait_list();
        if (terminated_thread->tid != main_id){
            // deallocate it all
            tid_t id_to_return = terminated_thread->tid;
            munmap((void*)((intptr_t)terminated_thread - STACK_SIZE), 
                    STACK_SIZE);
            free(terminated_thread);
            return id_to_return;
        }
        return main_id;
    }

    // if there is more than one in the scheduler
    if (round_robin->qlen() > 1) {
        round_robin->remove(terminated_thread);
        insert_waitlist_head(terminated_thread);
        lwp_yield();       
    }
    return NO_THREAD;

    //  if anythings in the waitlist 
    //      pop waitlist
    //      if the thread is not the main one
    //         deallocate it.
    //      else
    //         return main TID
    //  else 
    //      if more than one in the scheduler
    //         remove from scheduler 
    //         push to waitlist
    //      else (there is nothing else)
    //         return NO_THREAD

}


tid_t lwp_gettid (void) {
    if(cur_thread == NULL){
        return NO_THREAD;
    }
    return cur_thread->tid;
}


thread tid2thread (tid_t tid) {
    thread cur = threadlist_head;
    while (cur) {
        if (cur->tid == tid) {
            return cur;
        }
        cur = cur->lib_tl_next;
    }
    return NULL;
}


static void add_2_wait_list (thread thread_to_add) {
    if (waitlist_head == NULL) {
        waitlist_head = thread_to_add;
        waitlist_tail = thread_to_add;
        thread_to_add->lib_wl_next = NULL;
    } else {
        waitlist_tail->lib_wl_next = thread_to_add;
        thread_to_add->lib_wl_next = NULL;
        waitlist_tail = thread_to_add;
    }
    return;
}

static void insert_waitlist_head (thread thread_to_insert){
    thread old_head = waitlist_head;
    waitlist_head = thread_to_insert;
    waitlist_head->lib_wl_next = old_head;
}


static thread remove_wait_list() {
    if (waitlist_head == NULL){
        return NULL;
    }

    if (waitlist_head->lib_wl_next == NULL){
        waitlist_tail = NULL;
    }

    thread thread_removed = waitlist_head;
    waitlist_head = waitlist_head->lib_wl_next;

    return thread_removed;
}


static void lwp_wrap(lwpfun fun, void* arg) {
    int rval; 
    rval = fun(arg);
    lwp_exit(rval);
    return;
}


void print_thread(thread p_thread) {
    printf("Thread %lu {\n\tstack_addr = %p stack_size = 0x%zu\n\t"
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

