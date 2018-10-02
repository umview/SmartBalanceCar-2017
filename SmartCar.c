#include "AllInclude.h"
extern uint8 Pixel[128];
extern int LeftSpeed,RightSpeed;
extern int LeftDuty,RightDuty;
extern float32 Angle,AngleSpeed;
extern int GyroStandard;
extern float32 P_Balance,D_Balance,P_Speed,I_Speed,P_Turn;
extern uint8 PixelProcessBegin;
extern void Havemidline();
extern void CCDhandle();
extern uint8 midline_2[2];
extern float32 ExpectOffset;
extern int PhysicalZero;
extern float32 GyroIntegral;
extern int8 MotorState,AllStart;
extern uint8 Pixel_2[128],PixelB_2[128],Stop,StopCNT;
extern struct PixelLineStruct PixelLineNear[10],PixelLineForward[10];
extern uint8 PixelNear[128],PixelForward[128];
void delay();
extern int8 Mode;
extern int8 AllHigh;
float32 tmp;
extern int ExpectSpeed,ExpectSpeedSet;
extern float32 TmpAngle,SpeedIntegral;
extern int8 CircleOut;
extern uint8 State;
extern int8 RoadType,RoadTypeLast;
extern int8 Object;
extern uint8 hinderCNT;
void main(){
    DisableInterrupts;
    FPUEnable();
/************System Init*************/
    OLEDInit();
    OLEDShowString(0,0,"System Initing ...");
    OLEDShowString(0,2,"Please wait without touching !");
    MotorInit();
    UARTInit();//Default for UART5
    ADCInit();
    SwitchInit();
    LEDInit();
    CCDInit();
    EncoderInit();
    //FTM3Init();
    BuzzerInit();
    TeleControlInit();
    //InfraredTubeInit();
    ButtonInit();
    UltraSoundInit();
    MPU6050Init();
    FlashInit();
/***********End of System Init********/
    //U5PutChar('a');
    //PutChar('b');
/***************/
    delay();
    //NVICInit();
    EnableInterrupts;
    GetGyroStandard();
    //TimerInit();
    ScheduleInit();
    OLEDClear();
    MotorState=-1;
    GyroIntegral=(float32)(GetAccelZ());
    TmpAngle=(float32)(GetAccelZ());
    ExpectSpeed=0;
    LED1(1);
    LED2(1);
    LED3(1);
    LED4(1);
   for(;;){
     //if(Mode>=CNT && Mode<=CNT+2)Buzzer(1);
     //if((Mode>=CNT+1 && Mode <=CNT+3))Buzzer(1);
     //if(Mode==1 || (State==100))Buzzer(1);
     //else Buzzer(0);
     //if(Object>0)Buzzer(1);
     //Buzzer(1);
     if(hinderCNT||Mode||StopCNT)  Buzzer(1);
     else Buzzer(0);
     
     if(StopCNT==20){
        StopCNT = 0;
        if(Stop==1)Stop++;
        if(Stop==3){
          Stop = 0;
          MotorState=-1;
        }
     }
     if(AllStart==30) ExpectSpeed = ExpectSpeedSet;
     
     //if(Mode>=1 && Mode<=CNT+4)Buzzer(1);
     ///else Buzzer(0);
     //if(hinderCNT>0)Buzzer(1);
     //else Buzzer(0);
    OLEDShow();
    }
}

void delay()
{
  uint16 i, n;
  for(i=0;i<90;i++)
  {
    for(n=0;n<200;n++)
    {
      asm("nop");
    }
  }
}
    
     /*******
         accel_x = MPU6050GetResult(ACCEL_XOUT_H);
    accel_y = MPU6050GetResult(ACCEL_YOUT_H);
    accel_z = MPU6050GetResult(ACCEL_ZOUT_H);
    gyro_x  = MPU6050GetResult(GYRO_XOUT_H);
    gyro_y  = MPU6050GetResult(GYRO_YOUT_H);
    gyro_z  = MPU6050GetResult(GYRO_ZOUT_H);
     *****/
    

