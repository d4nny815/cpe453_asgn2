#ifndef SCHEDULERSH
#define SCHEDULERSH

#include "lwp.h"

typedef struct scheduler *scheduler;
typedef struct threadinfo_st *thread;

/*
extern scheduler AlwaysZero;
extern scheduler ChangeOnSIGTSTP;
extern scheduler ChooseHighestColor;
extern scheduler ChooseLowestColor;
*/

extern sceduler RoundRobin;
extern scheduler cur_scheduler;

void rr_admit(thread new_thread);
void rr_remove(thread victim);
thread next();
int qlen();

#endif
