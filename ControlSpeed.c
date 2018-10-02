#include "ControlSpeed.h"
extern int16 LeftSpeed,RightSpeed;
extern int SpeedDuty;
extern float32 P_Speed,I_Speed;
extern int ExpectSpeed;
extern int Speed;
extern int BalanceDuty;
extern float32 D_Speed;
extern int32 SpeedIntegral;
extern int ExpectSpeedSet;
int SpeedError[2]={0},SpeedErrorLast=0;
extern uint8 RoadWidthCnt;
extern uint8 RoadWidthNow,RoadWidthLast;
int16 dSpeed=0,dSpeedLast=0;
int8 dSpeedCnt=0;
extern int8 RoadWidthArray[10];
extern float32 PhysicalZero;
extern uint8 RoadType;
extern float32 Angle;
uint8 Hill=0;
void SpeedCtrl(){
  static int16 SpeedLast=0;
  static int SpeedArray[5]={0};
  static int SpeedDutyLast=0;
  static uint8 CntforSpeed=0;
  static uint8 Hill=0;
  //static int SpeedError=0,SpeedErrorLast=0;
  dSpeedLast=dSpeed;
  SpeedArray[0]=SpeedArray[1];
  SpeedArray[1]=SpeedArray[2];
  SpeedArray[2]=SpeedArray[3];
  SpeedArray[3]=SpeedArray[4];
  SpeedArray[4]=(LeftSpeed+RightSpeed)/2;
//  Speed=(int)((SpeedArray[0]+SpeedArray[1]+SpeedArray[2]+SpeedArray[3]+SpeedArray[4]+SpeedArray[5]+SpeedArray[6]+SpeedArray[7]+SpeedArray[8]+SpeedArray[9])*0.1);
  Speed=(int)((SpeedArray[0]+SpeedArray[1]+SpeedArray[2]+SpeedArray[3]+SpeedArray[4])*0.2);
  //if(Speed>(ExpectSpeed+40))Speed=ExpectSpeed+40;
  dSpeed=Speed-SpeedLast;
  if(Speed>SpeedLast){
    dSpeedCnt++;
    //Speed=SpeedLast;
  }else dSpeedCnt=0;
  if(RoadWidthNow>RoadWidthLast)RoadWidthCnt++;
  else RoadWidthCnt=0;
  //if((Speed-ExpectSpeed)>ExpectSpeed*0.2)Speed=ExpectSpeed;
  //if(dSpeedCnt>=5)Buzzer(1);
  /*
  if(RoadWidthNow>(RoadWidthLast) && RoadWidthNow>0 && RoadWidthLast>0)RoadWidthCnt++;
//  if(RoadWidthNow>RoadWidthLast)RoadWidthCnt++;
  
  else RoadWidthCnt=0;
//  if((((RoadWidthArray[0]+RoadWidthArray[1]+RoadWidthArray[2]+RoadWidthArray[3]+RoadWidthArray[4]+RoadWidthArray[5]+RoadWidthArray[6]+RoadWidthArray[7]+RoadWidthArray[8]+RoadWidthArray[9])/10)>85 && RoadWidthNow>80  )){
   // ExpectSpeed*=0.3;
    //PhysicalZero=1012-50;
  if(dSpeed>60){
    Hill=1;
    Buzzer(1);
  }else Buzzer(0);
  if(Hill==1)Hill++;
  if(Hill>=4)ExpectSpeed=ExpectSpeedSet,PhysicalZero=1012,Hill=0;
  */
  /*
  if(RoadWidthCnt>=2 && dSpeedCnt>=3 && dSpeed>20)Buzzer(1);// ((RoadWidthArray[0]+RoadWidthArray[1]+RoadWidthArray[2]+RoadWidthArray[3]+RoadWidthArray[4]+RoadWidthArray[5]+RoadWidthArray[6]+RoadWidthArray[7]+RoadWidthArray[8]+RoadWidthArray[9]+RoadWidthArray[14]+RoadWidthArray[10]+RoadWidthArray[11]+RoadWidthArray[12]+RoadWidthArray[13])/15)>55)Buzzer(1);
  else Buzzer(0);
    */
  if((Speed-ExpectSpeed)>ValueforSpeed && (SpeedLast-ExpectSpeed)>ValueforSpeed)CntforSpeed++;
  else  CntforSpeed=0;
  if(Hill==0 &&Angle>1120){
    //Buzzer(1);
    Hill++;
  }
  if(Hill>=2 && Hill==20)Hill++;
  if(Hill==21)Hill=0;
/*
    if(Hill>=1 && Hill<=5){
 //ExpectSpeed*=0.1;
 PhysicalZero=1012-100;
      Hill++;
      Buzzer(1);
    }else if(Hill>=6 && Hill<=30){
     Hill++;
     PhysicalZero+=30;
     if(PhysicalZero>=1012)PhysicalZero=1012;
     ExpectSpeed=ExpectSpeedSet;
    }else Hill=0;
  if(dSpeed>50 && Hill==0){
 //ExpectSpeed*=0.;
 PhysicalZero=1012-100;
        Buzzer(1);
   Hill=0;
  }else Buzzer(0);
  */
  //if(CntforSpeed>ValueforSpeedPlus){
  //  ExpectSpeed-=10;
  //}else ExpectSpeed=ExpectSpeedSet;
  //Speed-=ExpectSpeed;
  SpeedError[1]=ExpectSpeed-Speed;
  SpeedIntegral+=SpeedError[1];
  //积分限幅
  SpeedIntegral=SpeedIntegral>IntegralMax?IntegralMax:SpeedIntegral;
  SpeedIntegral=SpeedIntegral<-IntegralMax?-IntegralMax:SpeedIntegral;
  SpeedDuty=(int)(P_Speed * SpeedError[1] + I_Speed * SpeedIntegral);// + D_Speed * (SpeedError[1]-SpeedErrorLast));
  SpeedErrorLast=SpeedError[1];
  //平滑处理
  SpeedDuty=(int)(K_SpeedDuty * SpeedDuty + (1-K_SpeedDuty) * SpeedDutyLast);
  SpeedDutyLast=SpeedDuty;
  SpeedDuty=SpeedDuty>SpeedDutyMax?SpeedDutyMax:SpeedDuty;
  SpeedDuty=SpeedDuty<-SpeedDutyMax?-SpeedDutyMax:SpeedDuty;
  SpeedLast=Speed;
}