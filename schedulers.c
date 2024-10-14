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
void print_sch_thread(thread th); // TODO: exile this 

// FIXME: something wrong here
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
    return;
} 


void rr_remove(thread victim) {
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

thread next() {
    if (schedule_info.active_thread == NULL) {
        return NULL;
    }

    // Move to the next thread in the circular list and return it
    thread ret_thread = schedule_info.active_thread;
    schedule_info.active_thread = schedule_info.active_thread->sched_next;
    schedule_info.tail = ret_thread;
    return ret_thread;
}


int qlen() {
    return schedule_info.count;
}


void print_scheduler() {
    thread cur = schedule_info.active_thread;
    printf("\n[SCHEDULER QUEUE] active: %p tail: %p len: %d\n", 
            schedule_info.active_thread, schedule_info.tail, 
            schedule_info.count);
    if (cur == NULL) return;
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

