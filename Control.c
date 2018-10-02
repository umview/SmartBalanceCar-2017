#include "Control.h"
PIT_InitTypeDef Control;
extern void CCDhandle();
extern void Havemidline();
/*****Global Variable********/
int LeftDuty=0,RightDuty=0;
int16 LeftSpeed=0,RightSpeed=0;
int BalanceDuty=0,SpeedDuty=0,TurnDuty=0;
float32 Angle=0,AngleSpeed=0;
float32 TurnAngleSpeed=0;
float32 GyroIntegral=0;
int32 SpeedIntegral=0;
int Speed=0;
int ExpectSpeed=0;
int ExpectSpeedSet=365;
float32 ExpectOffset=0;
float32 ExpectOffsetLast=0;
uint8 Pixel[128];
int GyroStandard=0;
int TurnGyroStandard=0;
float32 PhysicalZero=1039;//1030;//370;//zero 2300//12188
uint16 Distance=0;
extern uint8 RoadWidthNow;
/*
float32 P_Balance=178,D_Balance=7.79;//350 13 1350 12///12//2350//15.6
float32 P_Speed=127,I_Speed=0,D_Speed=0;//23 -0.0002
float32 P_Turn=100,D_Turn=2.8;
*/
float32 P_Balance=50,D_Balance=37.0;//350 13 1350 12///12//2350//15.6
float32 P_Speed=34,I_Speed=0.338,/*0.269,*/D_Speed=0;//23 -0.0002
float32 P_Turn=428,D_Turn=35.5;//48;//120 22.7 //16 23
uint8 IncreaseValue=20;
int8 MotorState=1;
uint8 PixelProcessBegin=0;
extern uint8 PixelNear[128],PixelForward[128];
extern struct PixelLineStruct PixelLineNear[10],PixelLineForward[10];
int PWMTmp=0;
int AllStart=0;
/**********CCD*********/
extern unsigned char Pixel_2[128];
extern unsigned char Pixel_1[128];
extern uint8 midline_2[2];
extern uint8 PixelB_2[128];
//uint8 DelayForExpectSpeedSet=10; //100ms for each time
/*******function**************
P_Balance 2540 D_Balance 15.5
          480            16.520






**************************/
void ScheduleInit(){
    Control.PIT_Pitx = PIT0;
    Control.PIT_PeriodMs =1;
    Control.PIT_PeriodS=NULL;
    Control.PIT_PeriodUs=NULL;
    Control.PIT_Isr = GlobalControl;
    LPLD_PIT_Init(Control);
    LPLD_PIT_EnableIrq(Control);
}
void TimerInit(){
    Control.PIT_Pitx = PIT1;
    Control.PIT_PeriodMs =1;
    Control.PIT_PeriodS=NULL;
    Control.PIT_PeriodUs=NULL;
    Control.PIT_Isr = Timer;
    LPLD_PIT_Init(Control);
    LPLD_PIT_EnableIrq(Control);
}
void Timer(){
  LPLD_GPIO_Toggle_b(PTB,23);
}
void GlobalControl(){ //主时序控制
  static unsigned char FlagMs=0;
  static unsigned char FlagGetSpeed=0;
  static unsigned char FlagSpeed=0;
  static unsigned char FlagCCD=0;
 // static unsigned char FlagDistance=0;
//  static float32 AngleLast=0;
//  static uint8 cnt=0;
 // static int pwm=200;
  FlagMs++;


  
  if(FlagMs==1){

  FlagCCD++;
  
  if(FlagCCD==1){
    CCD2Integration();
    CCD1Integration();
  }
  else  if(FlagCCD==3){
  
    //CCD2ImageCapture(Pixel_2);
    CCD2ImageCapture(PixelNear);
    CCD1ImageCapture(PixelForward);
   //CCDhandle();
    //Havemidline();
    FlagCCD=0;
    //CCDView(PixelNear);
  //CCDView(PixelForward);
    //if(!PTB17_I)CCDViewAll();
    //CCDViewAll();
    PixelProcessBegin=1;
    //ExpectOffset=midline_2[1]-64;
  
       if(PixelBinaryzation()==1){
         ExpectOffset=0;
         PixelProcessBegin=0;
       }else{
         //CCDViewAll();
        ExpectOffset=ModeIdentification(PixelLineNear,PixelIndex(PixelNear,PixelLineNear,10));
        //if(RoadWidthNow>105)RoadWidthNow=0;
        ExpectOffsetLast=ExpectOffset;
         //ExpectOffset=ModeIdentification(PixelLineForward,PixelIndex(PixelForward,PixelLineForward,10));
         PixelProcessBegin=0;
         LPLD_GPIO_Toggle_b(PTB,20);
         //CCDView(PixelForward);
         //CCDViewAll();
         if(AllStart>0 && MotorState==1){
           AllStart++;
           if(AllStart>=30)AllStart=30;
         }else {
           ExpectSpeed=0;
           AllStart=0;
         }
       }
  }
  }else if(FlagMs==2){
    //FlagDistance++;
    //if(FlagDistance==8){
    //  RequestDistance();
    //  FlagDistance=0;
    //}
  }else if(FlagMs==3){

    
     Plot();
     //OLEDShowLine(PixelNear);
  }else if(FlagMs==4){
    
    FlagGetSpeed++;
    FlagSpeed++;
    if(FlagGetSpeed==2)GetSpeed(),FlagGetSpeed=0;
    if(FlagSpeed==SpeedPeriod){
      //I_Speed=(P_Speed/200.0);
      //达到ExpectSpeedSet的缓冲  ExpectSpeed平滑输出
      //if(ExpectSpeed!=ExpectSpeedSet)ExpectSpeed+=((ExpectSpeedSet-ExpectSpeed)>0?1:-1) * IncreaseValue;
      
      FlagSpeed=0;
      SpeedCtrl();//速度环控制
    }
    
  }else if(FlagMs==5){
    /*
    if(MotorState==2){
      if(LeftDuty>0)LeftDuty-=50;
      if(LeftDuty<0)LeftDuty+=50;
      if(RightDuty>0)RightDuty-=50;
      if(RightDuty<0)RightDuty+=50;
      UpdateDuty();
      return;
    }
    */
    //LPLD_GPIO_Toggle_b(PTB,21);
    GetAngle();
    //GetTurnAngleSpeed();
    TurnCtrl();
    BalanceCtrl();//直立环控制
    OutputSmooth();//速度平滑输出
    //PWM 限幅

    //保护模式
    //SafeMode();
    if(!PTB16_I)TurnDuty=0;
      
    LeftDuty=BalanceDuty - SpeedDuty - (int)(TurnDuty*1);
    RightDuty=BalanceDuty - SpeedDuty + (int)(TurnDuty*1);
    /*    
    if(TurnDuty>0){
    LeftDuty=BalanceDuty - SpeedDuty;
    RightDuty=BalanceDuty - SpeedDuty + TurnDuty;
    }else{
    LeftDuty=BalanceDuty - SpeedDuty - TurnDuty;
    RightDuty=BalanceDuty - SpeedDuty;
    }
    */
    //LeftDuty=-1600;
    //RightDuty=-1600;
    //if(!PTB17_I)SafeMode();
    /*
    if(!PTB16_I){
      Buzzer(1);
    LeftDuty=pwm;
    RightDuty=pwm;
    if(pwm>0)pwm+=5;
    if(pwm>=9000)pwm=300;
    }else Buzzer(0);
    */
    SafeMode();
     //   LeftDuty=-1600;
    //RightDuty=-1600;
    if(MotorState==1)UpdateDuty();//更新占空比
    else {
    //关电机
      FTM0ChangeCH0Duty(0);
      FTM0ChangeCH1Duty(0);
      FTM0ChangeCH2Duty(0);
      FTM0ChangeCH3Duty(0);
    }

   
  }
  if(FlagMs==5)FlagMs=0;
}
void SafeMode(){
  static uint8 cnt=0;
 // if(LeftSpeed>(ExpectSpeedSet*2.8) || (Angle<300 || Angle>1890) || RightSpeed>(ExpectSpeedSet*2.8))cnt++;
    if(MotorState==1 && ABS(Speed)>900)cnt++;

  else cnt=0;
  if(cnt>8)MotorState=-1;
}
//平滑输出函数
void OutputSmooth(){
    static unsigned char CounterForSpeedOutput=0;
    static int SpeedDutyDiff=0;
    static int SpeedDutyLast=0;
    CounterForSpeedOutput++;
    if(CounterForSpeedOutput==1)SpeedDutyDiff=(SpeedDuty-SpeedDutyLast)/SpeedPeriod,SpeedDuty=SpeedDutyLast;
    SpeedDuty+=SpeedDutyDiff;
    if(CounterForSpeedOutput==SpeedPeriod)CounterForSpeedOutput=0,SpeedDutyLast=SpeedDuty;
}
void NVICInit(){
	NVIC_InitTypeDef nvic_init_struct;

	//配置PIT0的NVIC分组
	nvic_init_struct.NVIC_IRQChannel = PIT0_IRQn;
	nvic_init_struct.NVIC_IRQChannelGroupPriority = NVIC_PriorityGroup_3;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 2;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 1;
	nvic_init_struct.NVIC_IRQChannelEnable = FALSE;
	LPLD_NVIC_Init(nvic_init_struct);
	//配置portb和portc的NVIC分组
	nvic_init_struct.NVIC_IRQChannel = PORTC_IRQn;
	nvic_init_struct.NVIC_IRQChannelGroupPriority = NVIC_PriorityGroup_3;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 5;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 1;
	nvic_init_struct.NVIC_IRQChannelEnable = TRUE;
	LPLD_NVIC_Init(nvic_init_struct);
	//配置PORTA的NVIC分组
	nvic_init_struct.NVIC_IRQChannel = PORTA_IRQn;
	nvic_init_struct.NVIC_IRQChannelGroupPriority = NVIC_PriorityGroup_3;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 1;
	nvic_init_struct.NVIC_IRQChannelEnable = TRUE;
	LPLD_NVIC_Init(nvic_init_struct);
        
        nvic_init_struct.NVIC_IRQChannel = PORTD_IRQn;
	nvic_init_struct.NVIC_IRQChannelGroupPriority = NVIC_PriorityGroup_3;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 4;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 1;
	nvic_init_struct.NVIC_IRQChannelEnable = TRUE;
	LPLD_NVIC_Init(nvic_init_struct);

}