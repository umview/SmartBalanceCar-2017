#ifndef _CONTROL_H_
#define _CONTROL_H_ 
#include "AllInclude.h"

#define BalancePeriod 5
#define SpeedPeriod 16
void SafeMode();
void ScheduleInit();
void GlobalControl();
void OutputSmooth();
void TimerInit();
void Timer();
void NVICInit();
#endif