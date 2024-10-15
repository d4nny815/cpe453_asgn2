#include "schedulers.h"

struct SchedulerInfo_t {
    thread active_thread;   // the first thread in the scheduler
    thread tail;
    int count;              // amount of threads in scheduler 
} schedule_info = (struct SchedulerInfo_t) {NULL, NULL, 0};

void my_rr_admit(thread new_thread);
void my_rr_remove(thread victim);
thread my_next();
int my_qlen();

struct scheduler rr = {NULL, NULL, my_rr_admit, my_rr_remove, my_next, my_qlen};
scheduler RoundRobin = &rr;

void my_rr_admit(thread new_thread) {
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

    return;
} 


void my_rr_remove(thread victim) {
    // If no threads are in the scheduler, return
    if (schedule_info.active_thread == NULL) {
        return;
    }

    // If there's only one thread in the scheduler
    if (schedule_info.count == 1 && schedule_info.active_thread == victim) {
        schedule_info.active_thread = NULL;
        schedule_info.tail = NULL;
        schedule_info.count = 0;
        return;
    }

    // If victim is the head (active_thread) and there are more threads
    if (victim == schedule_info.active_thread) {
        schedule_info.tail->sched_next = 
                schedule_info.active_thread->sched_next;
        schedule_info.active_thread->sched_next->sched_prev = 
                schedule_info.tail;
        schedule_info.active_thread = schedule_info.active_thread->sched_next;
        schedule_info.active_thread = victim->sched_next;
    }
    // If victim is the tail 
    else if (victim == schedule_info.tail) {
        schedule_info.tail->sched_prev->sched_next = 
                schedule_info.active_thread;
        schedule_info.active_thread->sched_prev = 
                schedule_info.tail->sched_prev;
        schedule_info.tail = schedule_info.tail->sched_prev;
    }
    // If victim is somewhere in the middle
    else {
        victim->sched_next->sched_prev = victim->sched_prev;
        victim->sched_prev->sched_next = victim->sched_next;
    }

    // Decrement the thread count
    schedule_info.count--;
}

thread my_next() {
    if (schedule_info.active_thread == NULL) {
        return NULL;
    }

    // Move to the next thread in the circular list and return it
    schedule_info.active_thread = schedule_info.active_thread->sched_next;
    schedule_info.tail = schedule_info.active_thread->sched_prev;
    return schedule_info.tail;
}


int my_qlen() {
    return schedule_info.count;
}


void init() {
    schedule_info.active_thread = NULL;    // No threads in the list initially
    schedule_info.tail = NULL; //current one? confused when to update this
    schedule_info.count = 0;      // Start with zero threads
}




