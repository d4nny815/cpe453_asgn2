#include "lwp.h"
#include "schedulers.h"


//just putting these here for context to refer to when I write this
//so the sched_one and the sched_two would act like my next and previous pointers in my linked list

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

// typedef int (*lwpfun)(void *);  /* type for lwp function */

// /* Tuple that describes a scheduler */
// typedef struct scheduler {
//   void   (*init)(void);            /* initialize any structures     */
//   void   (*shutdown)(void);        /* tear down any structures      */
//   void   (*admit)(thread new);     /* add a thread to the pool      */
//   void   (*remove)(thread victim); /* remove a thread from the pool */
//   thread (*next)(void);            /* select a thread to schedule   */
//   int    (*qlen)(void);            /* number of ready threads       */
// } *scheduler;


void init() {
    schedule.head = NULL;    // No threads in the list initially
    schedule.current = NULL; //current one? confused when to update this
    schedule.count = 0;      // Start with zero threads
}

void admit(thread newThread){
    //if this is the first thread in the list
    //add it and make it point to itself
    //sched_one = next
    //sched_two = prev
    //set the head and tail
    if (schedule.head == NULL){
        newThread->sched_one = newThread;
        newThread->sched_two = newThread;
        schedule.head = newThread;
        schedule.current = newThread;
        schedule.tail = newThread;
    }
    else{
        //find the current tail and add it on
        thread curtail = schedule.tail;

        //current tails next becomes the new thread
        curtail->sched_one = newThread;

        //new threads previous becomes the tail
        newThread->sched_two = curtail;

        //the new threads next becomes the start (head)(circles woo)
        newThread->sched_one = schedule.head;

        //the head's previous becomes the new thread (circles cuz happy)
        schedule.head->sched_two = newThread;

        schedule.tail = newThread;
    }

    schedule.count += 1;
} 

void remove(thread victim){
    //if nothing in there, return NULL
    if(schedule.head == NULL){
        return;
    }

    //if one to remove is the head and thats the only one
    if (victim == schedule.head && schedule.count == 1){
        victim->sched_one = NULL;
        victim->sched_two = NULL;
       
        //update in the global
        schedule.head = NULL;
        schedule.tail = NULL;
        schedule.current = NULL;

        deallocateStuff(victim);
        schedule.count--;
        return;
    }

    //if one to remove is the head and there are more
    if (victim == schedule.head && schedule.count != 1){
        schedule.tail->sched_one = schedule.head->sched_one;
        schedule.head->sched_one->sched_two = schedule.tail;
        
        //update the global variable cha fel
        schedule.head = schedule.head->sched_one;
        
        deallocateStuff(victim);
        schedule.count--;
        return;
    }

    //if the one to remove is the tail 
    if (victim == schedule.tail){
        schedule.tail->sched_two->sched_one = schedule.head;
        schedule.head->sched_two = schedule.tail->sched_two;

        schedule.tail = schedule.tail->sched_two; 

        deallocateStuff(victim);
        schedule.count--;
        return;
    }

    //if its none of those
    victim->sched_one->sched_two = victim->sched_two;
    victim->sched_two->sched_one = victim->sched_one;

    deallocateStuff(victim);
    schedule.count--;
    return;
}

thread next(){
    if (schedule.head == NULL){
        return NULL;
    }
    schedule.current = schedule.current->sched_one;
    return schedule.current;
}

int qlen(){
    return schedule.count;
}

void shutdown() {
    if (schedule.head == NULL) {
        return;
    }
    //start at the head 
    thread current = schedule.head;

    //go through the entire circle
    while (current->sched_one != schedule.head) {

        //keep track of the next thread
        thread next = current->sched_one;  

        deallocateStuff(current);      

        current = next; 
    }

    //at the end, the current will be the head and need to deallocate that too
    deallocateStuff(current); 

    schedule.head = NULL;
    schedule.tail = NULL;
    schedule.current = NULL;
    schedule.count = 0;
}

void deallocateStuff(thread threadToFree){
    if (threadToFree == NULL) {
        return;
    }

    munmap(threadToFree->stack, threadToFree->stacksize);

    //free the struct for the thread itself
    free(threadToFree);
}
