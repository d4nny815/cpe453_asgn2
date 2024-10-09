#ifndef SCHEDULERSH
#define SCHEDULERSH

#include <lwp.h>
#include <cstddef.h> //to make the red under the NULL go away but now this is red so idk 

extern scheduler AlwaysZero;
extern scheduler ChangeOnSIGTSTP;
extern scheduler ChooseHighestColor;
extern scheduler ChooseLowestColor;

#endif