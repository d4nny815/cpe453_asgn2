#include "schedulers.h"

void print_active_threads();

struct SchedulerInfo_t {
    thread active_thread;   // the first thread in the scheduler
    thread tail;
    int count;              // amount of threads in scheduler 
} schedule_info = (struct SchedulerInfo_t) {NULL, NULL, 0};

struct scheduler rr = {NULL, NULL, rr_admit, rr_remove, next, qlen};
scheduler cur_scheduler = &rr;

//void print_scheduler();
void print_sch_thread(thread th);

// TODO:
// ? when are we moving the current active? in remove or admit?
// ? remove if active should move the active to next

void rr_admit(thread new_thread) {

    //if this is the first thread in the list
    //add it and make it point to itself
    //sched_next = sched_next
    //sched_prev = sched_prev
    //set the head and tail
    if (schedule_info.active_thread == NULL) {
        new_thread->sched_next = new_thread;
        new_thread->sched_prev = new_thread;
        schedule_info.active_thread = new_thread;
        schedule_info.tail = new_thread;
        schedule_info.count = 1;
    } else {
        //find the current tail and add it on
        thread curtail = schedule_info.tail;

        //current tails sched_next becomes the new thread
        curtail->sched_next = new_thread;

        //new threads sched_previous becomes the tail
        new_thread->sched_prev = curtail;

        //the new threads sched_next becomes the start (head)(circles woo)
        new_thread->sched_next = schedule_info.active_thread;

        //the head's sched_previous becomes the new thread (circles cuz happy)
        schedule_info.active_thread->sched_prev = new_thread;

        schedule_info.tail = new_thread;
        schedule_info.count++;
    }

    //printf("[RR_ADMIT] thread %lu admited\n", new_thread->tid);
} 


void rr_remove(thread victim){
    //if nothing in there, return NULL
    if(schedule_info.active_thread == NULL){
        return;
    }

    //if one to remove is the head and thats the only one
    if (schedule_info.count == 1){
        victim->sched_next = NULL;
        victim->sched_prev = NULL;

        //update in the global
        schedule_info.active_thread = NULL;
        schedule_info.tail = NULL;
        schedule_info.count--;
        return;
    }

    //if one to remove is the head and there are more
    if (victim == schedule_info.active_thread){
        schedule_info.tail->sched_next = schedule_info.active_thread->sched_next;
        schedule_info.active_thread->sched_next->sched_prev = schedule_info.tail;
        
        //update the global variable cha fel
        schedule_info.active_thread = schedule_info.active_thread->sched_next;
        
    }

    //if the one to remove is the tail 
    else if (victim == schedule_info.tail){
        schedule_info.tail->sched_prev->sched_next = schedule_info.active_thread;
        schedule_info.active_thread->sched_prev = schedule_info.tail->sched_prev;

        schedule_info.tail = schedule_info.tail->sched_prev; 

    }

    //if its none of those
    else {
        victim->sched_next->sched_prev = victim->sched_prev;
        victim->sched_prev->sched_next = victim->sched_next;
    }

    schedule_info.count--;
    //printf("[RR_REMOVE] thread %lu removed\n", victim->tid);
    return;
}


thread next() {
    if (schedule_info.tail == NULL){
        return NULL;
    }
    return schedule_info.tail->sched_next;
}


int qlen() {
    return schedule_info.count;
}


void print_scheduler() {
    thread cur = schedule_info.active_thread;
    printf("\n[SCHEDULER QUEUE] active: %p tail: %p len: %d\n", 
            schedule_info.active_thread, schedule_info.tail, schedule_info.count);
    do {
        print_sch_thread(cur);
        cur = cur->sched_next;
    } while (cur->tid != schedule_info.active_thread->tid);

    printf("[END OF QUEUE]\n\n");
}


void print_sch_thread(thread th) {
    printf("thread %p ID: %lu sched_next %p sched_prev %p\n", 
            th, th->tid, th->sched_next, th->sched_prev);

}

