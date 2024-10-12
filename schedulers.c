#include "schedulers.h"

void print_active_threads();

struct SchedulerInfo_t {
    thread active_thread;   // the first thread in the scheduler
    thread tail;
    int count;              // amount of threads in scheduler 
} schedule = (struct SchedulerInfo_t) {NULL, NULL, 0};

struct scheduler rr = {NULL, NULL, rr_admit, rr_remove, next, qlen};
scheduler round_robin = &rr;

void print_scheduler();
void print_thread(thread th);

void rr_admit(thread new_thread) {

    //if this is the first thread in the list
    //add it and make it point to itself
    //sched_next = sched_next
    //sched_prev = sched_prev
    //set the head and tail
    if (schedule.active_thread == NULL) {
        new_thread->sched_next = new_thread;
        new_thread->sched_prev = new_thread;
        schedule.active_thread = new_thread;
        schedule.tail = new_thread;
        schedule.count = 0;
    } 
    else {
        //find the current tail and add it on
        thread curtail = schedule.tail;

        //current tails sched_next becomes the new thread
        curtail->sched_next = new_thread;

        //new threads sched_previous becomes the tail
        new_thread->sched_prev = curtail;

        //the new threads sched_next becomes the start (head)(circles woo)
        new_thread->sched_next = schedule.active_thread;

        //the head's sched_previous becomes the new thread (circles cuz happy)
        schedule.active_thread->sched_prev = new_thread;

        schedule.tail = new_thread;
    }

    schedule.count++;
    //printf("[RR_ADMIT] thread %lu admited\n", new_thread->tid);
} 

void rr_remove(thread victim){
    //if nothing in there, return NULL
    if(schedule.active_thread == NULL){
        return;
    }

    //if one to remove is the head and thats the only one
    if (schedule.count == 1){
        victim->sched_next = NULL;
        victim->sched_prev = NULL;

        //update in the global
        schedule.active_thread = NULL;
        schedule.tail = NULL;
        schedule.count--;
        return;
    }

    //if one to remove is the head and there are more
    if (victim == schedule.active_thread){
        schedule.tail->sched_next = schedule.active_thread->sched_next;
        schedule.active_thread->sched_next->sched_prev = schedule.tail;
        
        //update the global variable cha fel
        schedule.active_thread = schedule.active_thread->sched_next;
        
        schedule.count--;
        return;
    }

    //if the one to remove is the tail 
    if (victim == schedule.tail){
        schedule.tail->sched_prev->sched_next = schedule.active_thread;
        schedule.active_thread->sched_prev = schedule.tail->sched_prev;

        schedule.tail = schedule.tail->sched_prev; 

        schedule.count--;
        return;
    }

    //if its none of those
    victim->sched_next->sched_prev = victim->sched_prev;
    victim->sched_prev->sched_next = victim->sched_next;
    schedule.count--;

    //printf("[RR_REMOVE] thread %lu removed\n", victim->tid);
    return;
}


thread next() {
    if (schedule.active_thread == NULL){
        return NULL;
    }
    return schedule.active_thread->sched_next;
}


int qlen() {
    return (int)schedule.count;
}


void print_scheduler() {
    thread cur = schedule.active_thread;
    printf("\n[SCHEDULER QUEUE] active: %p tail: %p len: %zu\n", 
            schedule.active_thread, schedule.tail, schedule.count);
    do {
        print_sch_thread(cur);
        cur = cur->sched_next;
    } while (cur->tid != schedule.active_thread->tid);

    printf("[END OF QUEUE]\n\n");
}


void print_sch_thread(thread th) {
    printf("thread %p sched_next %p sched_prev %p\n", 
            th, th->sched_next, th->sched_prev);

}

