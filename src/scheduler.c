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
    //next = next
    //prev = prev
    //set the head and tail
    if (schedule.active_thread == NULL) {
        new_thread->next = new_thread;
        new_thread->prev = new_thread;
        schedule.active_thread = new_thread;
        schedule.tail = new_thread;
        schedule.count = 0;
    } 
    else {
        //find the current tail and add it on
        thread curtail = schedule.tail;

        //current tails next becomes the new thread
        curtail->next = new_thread;

        //new threads previous becomes the tail
        new_thread->prev = curtail;

        //the new threads next becomes the start (head)(circles woo)
        new_thread->next = schedule.active_thread;

        //the head's previous becomes the new thread (circles cuz happy)
        schedule.active_thread->prev = new_thread;

        schedule.tail = new_thread;
    }

    schedule.count++;
} 

void rr_remove(thread victim){
    //if nothing in there, return NULL
    if(schedule.active_thread == NULL){
        return;
    }

    //if one to remove is the head and thats the only one
    if (schedule.count == 1){
        victim->next = NULL;
        victim->prev = NULL;

        //update in the global
        schedule.active_thread = NULL;
        schedule.tail = NULL;
        schedule.count--;
        return;
    }

    //if one to remove is the head and there are more
    if (victim == schedule.active_thread){
        schedule.tail->next = schedule.active_thread->next;
        schedule.active_thread->next->prev = schedule.tail;
        
        //update the global variable cha fel
        schedule.active_thread = schedule.active_thread->next;
        
        schedule.count--;
        return;
    }

    //if the one to remove is the tail 
    if (victim == schedule.tail){
        schedule.tail->prev->next = schedule.active_thread;
        schedule.active_thread->prev = schedule.tail->prev;

        schedule.tail = schedule.tail->prev; 

        schedule.count--;
        return;
    }

    //if its none of those
    victim->next->prev = victim->prev;
    victim->prev->next = victim->next;
    schedule.count--;
    return;
}

thread rr_next() {
    if (schedule.active_thread == NULL){
        return NULL;
    }
    return schedule.active_thread->next;
}

int qlen() {
    return schedule.count;
}
