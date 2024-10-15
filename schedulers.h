#ifndef SCHEDULERSH
#define SCHEDULERSH

#include "lwp.h"

typedef struct scheduler *scheduler;
typedef struct threadinfo_st *thread;

extern scheduler AlwaysZero;
extern scheduler ChangeOnSIGTSTP;
extern scheduler ChooseHighestColor;
extern scheduler ChooseLowestColor;
extern scheduler RoundRobin;


#endif
