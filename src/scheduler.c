#include "../include/lwp.h"
#include "../include/schedulers.h"

// TODO: danny fix this
struct SchedulerInfo_t {
    thread active_thread;  // the first thread in the scheduler
    thread tail;
    int count;    // the count of how many total threads in scheduler currently
} schedule = (struct SchedulerInfo_t) {NULL, NULL, 0};

void rr_admit(thread new_thread) {
    //if this is the first thread in the list
    //add it and make it point to itself
    //sched_one = next
    //sched_two = prev
    //set the head and tail
    if (schedule.active_thread == NULL) {
        new_thread->sched_one = new_thread;
        new_thread->sched_two = new_thread;
        schedule.active_thread = new_thread;
        schedule.tail = new_thread;
        schedule.count = 0;
    } 
    else {
        //find the current tail and add it on
        thread curtail = schedule.tail;

        //current tails next becomes the new thread
        curtail->sched_one = new_thread;

        //new threads previous becomes the tail
        new_thread->sched_two = curtail;

        //the new threads next becomes the start (head)(circles woo)
        new_thread->sched_one = schedule.active_thread;

        //the head's previous becomes the new thread (circles cuz happy)
        schedule.active_thread->sched_two = new_thread;

        schedule.tail = new_thread;
    }

    schedule.count++;
} 

void rr_remove(thread victim) {
    //if nothing in there, return NULL
    if(schedule.active_thread == NULL){
        return;
    }

    //if one to remove is the head and thats the only one
    if (schedule.count == 1){
        victim->sched_one = NULL;
        victim->sched_two = NULL;

        //update in the global
        schedule.active_thread = NULL;
        schedule.tail = NULL;
        schedule.count--;
        return;
    }

    //if one to remove is the head and there are more
    if (victim == schedule.active_thread){
        schedule.tail->sched_one = schedule.active_thread->sched_one;
        schedule.active_thread->sched_one->sched_two = schedule.tail;
        
        //update the global variable cha fel
        schedule.active_thread = schedule.active_thread->sched_one;
        
        schedule.count--;
        return;
    }

    //if the one to remove is the tail 
    if (victim == schedule.tail){
        schedule.tail->sched_two->sched_one = schedule.active_thread;
        schedule.active_thread->sched_two = schedule.tail->sched_two;

        schedule.tail = schedule.tail->sched_two; 

        schedule.count--;
        return;
    }

    //if its none of those
    victim->sched_one->sched_two = victim->sched_two;
    victim->sched_two->sched_one = victim->sched_one;
    schedule.count--;
    return;
}

thread rr_next() {
    if (schedule.active_thread == NULL){
        return NULL;
    }
    return schedule.active_thread->sched_one;
}

int qlen() {
    return schedule.count;
}
