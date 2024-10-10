#ifndef SCHEDULERSH
#define SCHEDULERSH

#include "lwp.h"
//#include <cstddef.h> 

extern scheduler AlwaysZero;
extern scheduler ChangeOnSIGTSTP;
extern scheduler ChooseHighestColor;
extern scheduler ChooseLowestColor;

void rr_admit(thread new_thread);
void rr_remove(thread victim);
void next();
void qlen();

#endif
