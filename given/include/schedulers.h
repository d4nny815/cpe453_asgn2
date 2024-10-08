#ifndef SCHEDULERSH
#define SCHEDULERSH

#include <lwp.h>
#include <cstddef> //to make the red under the NULL go away but now this is red so idk 

extern scheduler AlwaysZero;
extern scheduler ChangeOnSIGTSTP;
extern scheduler ChooseHighestColor;
extern scheduler ChooseLowestColor;


typedef struct schedulerInfo {
    thread head;  // the first thread in the scheduler
    thread tail; //the last thread added (idk if we need this)
    thread current; //pointer to the current thread executing 
    int count;    // the count of how many total threads in scheduler currently
} schedulerInfo_t;

//made it a global variable so that it will store the first head and the amount
schedulerInfo_t schedule;

#endif