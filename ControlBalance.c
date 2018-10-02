#include "ControlBalance.h"
extern int BalanceDuty;
extern float32 Angle,AngleSpeed;
extern float32 P_Balance,D_Balance;
extern float32 PhysicalZero;
void BalanceCtrl(){

  BalanceDuty = (int)(P_Balance * (Angle-PhysicalZero) + D_Balance * AngleSpeed);
}
