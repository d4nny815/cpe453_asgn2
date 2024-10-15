#include "lwp.h"

// ? should we put these in a struct
static size_t thread_id_counter = 0;
static thread threadlist_head = NULL; 
static thread waitlist_head = NULL;
static thread cur_thread = NULL;
static tid_t main_id = 0;

// default scheduler
static scheduler cur_scheduler = NULL;

// helper function
void print_thread(thread p_thread);
//void print_all_threads();
void print_waitlist();
static void lwp_wrap(lwpfun fun, void* arg);
static thread remove_waitlist();
static void add_2_waitlist(thread thread_to_add);
static void insert_waitlist_head(thread thread_to_insert);
static void add_2_biglist(thread thread_to_add);
static void remove_from_big_list(thread thread_to_remove);
static void swap_scheduler(scheduler old, scheduler new);


// function creates and returns the TID of the process it created
tid_t lwp_create(lwpfun function, void *argument) {
    // create the thread
    thread thread_created = (thread) malloc(sizeof(context));
    if(thread_created == NULL) {
        return NO_THREAD; 
    }

    // assign it to the thread
    thread_created->tid = ++thread_id_counter;

    // TODO: dumbass rlimit shit for stack size
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
    void* p_fr_bp = (void*)((intptr_t) init_ptr + STACK_SIZE - WORD_SIZE);
    thread_created->stack = (unsigned long*) p_fr_bp; 
    thread_created->stacksize = STACK_SIZE;
    thread_created->stack = LWP_LIVE;

    // inject into the stack, return address, old base pointer*
    unsigned long* p_stack = (unsigned long*) p_fr_bp;;
    p_stack[0] = (unsigned long) lwp_exit;
    p_stack[-1] = (unsigned long) lwp_wrap; // RA 
    p_stack[-2] = (unsigned long) old_bp;

    // create an initial rfile
    rfile* init_rfile = &thread_created->state;

    // set the register pointers we need
    intptr_t sp = (intptr_t) init_ptr + STACK_SIZE - 
                    NUM_INJECTED_STACK_WORDS * WORD_SIZE;
    init_rfile->rsp = (unsigned long) sp;
    init_rfile->rbp = (unsigned long) sp;
    init_rfile->rdi = (unsigned long) function; 
    init_rfile->rsi = (unsigned long) argument; 
    init_rfile->fxsave = FPU_INIT;

    // admit new thread to the cur_schedulerr
    cur_scheduler = lwp_get_scheduler();
    if (cur_scheduler == NULL) {
        cur_scheduler = RoundRobin;
    }
    cur_scheduler->admit(thread_created);
    #ifdef DEBUG
    if (cur_thread != NULL) {
        printf("[LWP_CREATE] Th %lu is admitting Th %lu\n", 
                cur_thread->tid, thread_created->tid);
    }
    #endif
    // add to total thread list
    add_2_biglist(thread_created);

    //print_cur_scheduler();
    return thread_created->tid;
}


void lwp_start(void) {
    // create a thread to be the main process 
    thread thread_main = (thread) malloc(sizeof(context));

    if(thread_main == NULL) {
        return; 
    }

    // assign it to the thread
    thread_main->tid = ++thread_id_counter;
    main_id = thread_main->tid;
    thread_main->stack = NULL; 
    thread_main->stacksize = 0;
    thread_main->status = LWP_LIVE;

    // main thread is the current thread
    cur_thread = thread_main;
    add_2_biglist(thread_main);

    cur_scheduler = lwp_get_scheduler();
    if (cur_scheduler == NULL) {
        cur_scheduler = RoundRobin;
    }

    // admit new thread to the cur_schedulerr
    cur_scheduler->admit(thread_main);
    #ifdef DEBUG
        printf("[LWP_START] Th %lu is admitting Th %lu\n", 
            cur_thread->tid, thread_main->tid);
    #endif

    //  call yield
    lwp_yield();
}


void lwp_yield(void) {
    cur_scheduler = lwp_get_scheduler();
    if (cur_scheduler == NULL) {
        cur_scheduler = RoundRobin;
    }

    thread old_thread = cur_thread;    
    thread next_thread = cur_scheduler->next();
    #ifdef DEBUG
    printf("[LWP_YIELD] cur_thread %lu next %lu\n", 
            cur_thread->tid, next_thread->tid);
    // print_scheduler();
    #endif
    cur_thread = next_thread;


    // swap that hoeeeee
    swap_rfiles(&old_thread->state, &cur_thread->state);
}


void lwp_exit(int exitval) {
    cur_scheduler = lwp_get_scheduler();
    if (cur_scheduler == NULL) {
        cur_scheduler = RoundRobin;
    }

    if (cur_scheduler->qlen() == 0 && waitlist_head == NULL) {
        #ifdef DEBUG
        printf("[LWP_EXIT] scheduler and waitlist empty");
        #endif
        return;
    }

    if (cur_thread == NULL) {
        return;
    }


    // change status
    cur_thread->status = MKTERMSTAT(LWP_TERM, exitval);

    // add to the wait list 
    add_2_waitlist(cur_thread);

    cur_scheduler = lwp_get_scheduler();
    if (cur_scheduler == NULL) {
        cur_scheduler = RoundRobin;
    }

    // remove from the cur_schedulerr
    cur_scheduler->remove(cur_thread);

    #ifdef DEBUG
    if (cur_thread != NULL) {
        printf("[LWP_EXIT] Th %lu is removing itself\n", 
                cur_thread->tid);
    }
    print_waitlist();
    // print_scheduler();
    #endif

    thread wl_thread = remove_waitlist();

    if (wl_thread != NULL) {
        if (!LWPTERMINATED(wl_thread->status)) {

        cur_scheduler = lwp_get_scheduler();
        if (cur_scheduler == NULL) {
            cur_scheduler = RoundRobin;
        }

            cur_scheduler->admit(wl_thread);
            #ifdef DEBUG
            if (cur_thread != NULL) {
                printf("[LWP_EXIT] Th %lu is admitting Th %lu\n", 
                        cur_thread->tid, wl_thread->tid);
            }
            // print_scheduler();
            #endif
        } else {
            insert_waitlist_head(wl_thread);

        }
    }


    //waitlist after adding something
    if (cur_scheduler->qlen() > 0) {
        lwp_yield();
    } else {
        if (cur_scheduler->shutdown != NULL) {
            cur_scheduler->shutdown();
        }
        free(remove_waitlist());
    }

    return;
}


tid_t lwp_wait(int *status) {
    thread terminated_thread;
    
    HEHE:
    if (waitlist_head) { 
        // take the oldest terminated therad out from the waitlist 
        terminated_thread = remove_waitlist();
        if (terminated_thread->tid != main_id){
            // deallocate it all
            // put the status of the one terminated into the status pointer 
            if (status != NULL) {
                *status = terminated_thread->status;
            }
            tid_t id_to_return = terminated_thread->tid;
            #ifdef DEBUG
                printf("[LWP_WAIT] Th %zu is killing thread %lu\n", 
                cur_thread->tid, id_to_return);
            #endif
            remove_from_big_list(terminated_thread);
            munmap((void*)((intptr_t)terminated_thread - STACK_SIZE), 
                    STACK_SIZE);
            free(terminated_thread);
            return id_to_return;
        }
        return main_id;
    }

    cur_scheduler = lwp_get_scheduler();
    if (cur_scheduler == NULL) {
        cur_scheduler = RoundRobin;
    }

    // if there is more than one in the cur_schedulerr
    if (cur_scheduler->qlen() >= 1) {
        cur_scheduler->remove(cur_thread);
        #ifdef DEBUG
        printf("[LWP_WAIT] Th %lu is removing Th %lu cuz waiting\n", 
                cur_thread->tid, cur_thread->tid);
        #endif
        insert_waitlist_head(cur_thread);
        lwp_yield();     
        goto HEHE;  
    }

    return NO_THREAD;
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
        if (cur->tid == tid && !LWPTERMINATED(cur->status)) {
            return cur;
        }
        cur = cur->lib_tl_next;
    }
    return NULL;
}


scheduler lwp_get_scheduler() {
    return cur_scheduler;
}


void lwp_set_scheduler(scheduler sched){
    //if it is null, then just default to round robin 
    if (cur_scheduler == NULL) {
        cur_scheduler = RoundRobin;
    }

    // bad input 
    if (sched == NULL && cur_scheduler != RoundRobin) {
        swap_scheduler(cur_scheduler, RoundRobin);
        return;
    }

    if (sched == cur_scheduler) {
        return;
    }

    swap_scheduler(cur_scheduler, sched);

    return;
}


static void swap_scheduler(scheduler old, scheduler new) {
    if (new->init != NULL){
        new->init();
    }
    
    //if the current scheduler is not empty, copy all the active threads over
    thread curr_thread = old->next();
    while (curr_thread) {

        //take it out of the old scheduler 
        old->remove(curr_thread);

        //put it into the new scheduler 
        new->admit(curr_thread);

        curr_thread = old->next();
    }

    //check if shutdown exists
    if (old->shutdown != NULL) {
        old->shutdown();
    }

    //make the global cur_scheduler point to the right thing
    cur_scheduler = new;

    return;
}


static void add_2_waitlist(thread thread_to_add) {
    if (waitlist_head == NULL) {
        waitlist_head = thread_to_add;
        thread_to_add->lib_wl_next = NULL;
        return;
    }

    thread curr_thread = waitlist_head;
    while (curr_thread->lib_wl_next != NULL) {
        curr_thread = curr_thread->lib_wl_next;
    }

    curr_thread->lib_wl_next = thread_to_add;
    thread_to_add->lib_wl_next = NULL;

    #ifdef DEBUG
    print_waitlist();
    #endif
    return;
}


static void insert_waitlist_head (thread thread_to_insert){
    thread old_head = waitlist_head;
    waitlist_head = thread_to_insert;
    waitlist_head->lib_wl_next = old_head;
}


static thread remove_waitlist() {
    if (waitlist_head == NULL){
        return NULL;
    }


    thread thread_removed = waitlist_head;
    waitlist_head = waitlist_head->lib_wl_next;
    #ifdef DEBUG
    printf("[WAITLIST POP] removing Th %zu\n", thread_removed->tid);
    #endif

    return thread_removed;
}


void print_waitlist() {
    thread current = waitlist_head;

    if (current == NULL) {
        printf("[WAITLIST] Waitlist is empty.\n");
        return;
    }

    printf("[WAITLIST] Current threads in waitlist:\n");
    while (current) {
        printf("    Thread ID: %lu @ %p Status: %d next %p\n", 
            current->tid, current,
            current->status, current->lib_wl_next);
        current = current->lib_wl_next;
    }
}


static void add_2_biglist(thread thread_to_add){
     if (threadlist_head == NULL) {
        threadlist_head = thread_to_add;
        thread_to_add->lib_tl_next = NULL;
    } else {
        thread_to_add->lib_tl_next = threadlist_head;
        threadlist_head = thread_to_add;
    }
    return;
}

static void remove_from_big_list(thread thread_to_remove){
    if (threadlist_head == NULL){
        return;
    }

    // just one thing and need to remove it
    if (threadlist_head->lib_tl_next == NULL){

        // check if its the one to remove then remove it
        if (thread_to_remove->tid == threadlist_head->tid){
            threadlist_head = NULL;
        }
        return;
    }

    // case where theres more things and removing head
    if (threadlist_head->tid == thread_to_remove->tid){
        threadlist_head = threadlist_head->lib_tl_next;
    }
   
    thread cur_thread = threadlist_head->lib_tl_next;
    thread prev_thread = threadlist_head;

    while(cur_thread){
        if (cur_thread->tid == thread_to_remove->tid){
            prev_thread->lib_tl_next = cur_thread->lib_tl_next;
            return;
        }
        cur_thread = cur_thread->lib_tl_next;
        prev_thread = prev_thread->lib_tl_next;
    }
    return;
}

static void lwp_wrap(lwpfun fun, void* arg) {
    int rval; 
    rval = fun(arg);
    lwp_exit(rval);
    return;
}


void print_thread(thread p_thread) {
    printf("Thread %lu @ %p {\n\tstack_addr = %p stack_size = 0x%zu\n\t"
            "lib_tl_next %p, lib_wl_next %p\n\tsched_next"
            " = %p, sched_prev = %p\n\texited = %p\n}\n", 
            p_thread->tid, p_thread, p_thread->stack, p_thread->stacksize, 
            p_thread->lib_tl_next, p_thread->lib_wl_next, 
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

