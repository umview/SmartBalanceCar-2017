#include "ControlFilter.h"
/*************************/

/*****************************/
extern float32 Angle;
extern float32 AngleSpeed;
extern float32 GyroIntegral;
extern float32 TurnAngleSpeed;
extern float32 PhysicalZero;
extern int GyroStandard;
extern int TurnGyroStandard;
float32 TmpAngle=0;
int16 Angle1=0;
/****for Kalman*****/
float32    AngleFilter=0;      //�ںϺ�õ��ĽǶ�
float32  Gyro_x=0;

float32  Q_angle=0.001; //1--0.001 ����������Э����//0.2//0.001
                        //��ֵԽС���仯Խ��
float32  Q_gyro =0.001;  //0.3--0.003 ����������Э���� ����������Э����Ϊһ��һ�����о���,//0.1//0.05//0.4
                        //��ֵԽС���仯Խ��
float32  R_angle=0.5;    //0.01--0.5   ����������Э���� �Ȳ���ƫ�� //0.5//0.4
                        //��ֵԽС���仯Խ��
//float32  dt=0.005;       //0.001--0.005;Ϊ����ʱ�䣬��Ӧ5ms	                  
float32  C_0 = 1.0;
float32  Q_bias=0, Angle_err=0;
float32  PCt_0=0.0, PCt_1=0, E=0.0;
float32  K_0=0.0, K_1=0.0, t_0=0.0, t_1=0.0;
float32  Pdot[4] ={0,0,0,0};
float32  PP[2][2] = { { 1.0, 0 },{ 0, 1.0 } };
/****/
void GetAngle(){
//  static int16 AngleArray[5]={0};
  //static int16 AngleSpeedArray[5]={0};
  int16 AngleTmp=0;
  int16 AngleSpeedTmp=0;
  //static float32 TmpAngle=0;
  //unsigned char  i=0;
  //AngleTmp=GetAccelZ();//+PhysicalZero;
  //for(i=1;i<=5;i++)AngleTmp+=-GetADC0AD12();//+PhysicalZero;;//+PhysicalZero;
  //AngleTmp/=5;
  AngleSpeedTmp=GetGyroZ()-GyroStandard;
  AngleTmp=GetAccelZ();//-PhysicalZero;
  Angle1=AngleTmp;
  //AngleSpeedTmp=GetADC0AD14()-GyroStandard;
  //for(i=1;i<=10;i++)AngleSpeedTmp+=GetADC0AD14()-GyroStandard;
  //AngleSpeedTmp/=10;
  TurnAngleSpeed=(float32)((GetGyroY()-TurnGyroStandard)/100.0);
  //TurnAngleSpeed=GetADC0AD13()-TurnGyroStandard;
  //AngleSpeedTmp=AngleSpeedAvg/5.0;
    /*
  AngleArray[0]=AngleArray[1];
  AngleArray[1]=AngleArray[2];
  AngleArray[2]=AngleArray[3];
  AngleArray[3]=AngleArray[4];
  AngleArray[4]=AngleTmp;
  AngleTmp=(int16)((AngleArray[0]+AngleArray[1]+AngleArray[2]+AngleArray[3]+AngleArray[4])*0.2);
    */
  if(AngleTmp>32760)AngleTmp=32760;
  else if(AngleTmp<-32760)AngleTmp=-32760;
  if(AngleSpeedTmp>32760)AngleSpeedTmp=32760;
  else if(AngleSpeedTmp<-32760)AngleSpeedTmp=-32760;
  if(TurnAngleSpeed>32760)TurnAngleSpeed=32760;
  else if(TurnAngleSpeed<-32760)TurnAngleSpeed=-32760;
  AngleSpeed=(float32)AngleSpeedTmp/100.0;
   /*
  AngleSpeedArray[0]=AngleSpeedArray[1];
  AngleSpeedArray[1]=AngleSpeedTmp;
  AngleSpeedTmp=(int16)((AngleSpeedArray[0]+AngleSpeedArray[1])*0.5);
  */
  GyroIntegral+=AngleSpeedTmp * dt * GyroCOE;
  //Angle=(int)AngleTmp;
  /****For Kalman******/
  
  //KalmanFilter((float32)(AngleTmp),(float32)(AngleSpeedTmp * GyroCOE));
  //Angle=AngleFilter/10.0;
  //GyroIntegral=(float32)AngleTmp;
  //AngleSpeed=(float32)AngleSpeedTmp;///10.0;
  //Angle=(float32)AngleTmp;
  
  TmpAngle=K_Accel * AngleTmp + (1-K_Accel) * (TmpAngle + AngleSpeedTmp * GyroIntegralTime * GyroCOE);
  Angle=TmpAngle/10.0;
  
  //AngleSpeed=(float32)AngleSpeedTmp/100.0;
  //AngleSpeed=Gyro_x/100.0;
}
void GetAngleSpeed(){
}

/*
void GetAngle(){
//  static float32 AngleArray[5]={0};
//  static float32 AngleSpeedArray[5]={0};
  int AngleTmp=0;
  int AngleSpeedTmp=0;
  unsigned char  i=0;
  //AngleTmp=GetAccelZ();//+PhysicalZero;
  //for(i=1;i<=5;i++)AngleTmp+=-GetADC0AD12();//+PhysicalZero;;//+PhysicalZero;
  //AngleTmp/=5;
  //AngleSpeedTmp=GetGyroZ()-GyroStandard;
  AngleTmp=-GetADC0AD12()+PhysicalZero;
  //AngleSpeedTmp=GetADC0AD14()-GyroStandard;
  for(i=1;i<=10;i++)AngleSpeedTmp+=GetADC0AD14()-GyroStandard;
  AngleSpeedTmp/=10;
  //TurnAngleSpeed=GetGyroY()-TurnGyroStandard;
  TurnAngleSpeed=GetADC0AD13()-TurnGyroStandard;
  //AngleSpeedTmp=AngleSpeedAvg/5.0;
  //if(AngleSpeedTmp>32760)AngleSpeedTmp=32760;
  //else if(AngleSpeedTmp<-32760)AngleSpeedTmp=-32760;
  GyroIntegral+=AngleSpeedTmp * dt * GyroCOE;
  //Angle=(int)AngleTmp;
 
  
  KalmanFilter((float32)(AngleTmp),(float32)(AngleSpeedTmp * GyroCOE));
  //Angle=(float32)AngleTmp;///10.0;
  Angle=AngleFilter;
  //GyroIntegral=(float32)AngleTmp;
  AngleSpeed=(float32)AngleSpeedTmp;///10.0;


}

*/


/******************/

/**************************************************************************
�������ܣ����׿������˲�
��ڲ��������ٶȡ����ٶ�
����  ֵ����
**************************************************************************/
void KalmanFilter(float32 Accel,float32 Gyro)		
{
    AngleFilter+=(Gyro - Q_bias) * dt;           //�������

    Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-����������Э�����΢��
    Pdot[1]=- PP[1][1];
    Pdot[2]=- PP[1][1];
    Pdot[3]=Q_gyro;
            
    PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
    PP[0][1] += Pdot[1] * dt;   // =����������Э����
    PP[1][0] += Pdot[2] * dt;
    PP[1][1] += Pdot[3] * dt;
                    
    Angle_err =Accel-AngleFilter;     //zk-�������

    PCt_0 = C_0 * PP[0][0];
    PCt_1 = C_0 * PP[1][0];

    E = R_angle + C_0 * PCt_0;

    K_0 = PCt_0 / E;
    K_1 = PCt_1 / E;

    t_0 = PCt_0;
    t_1 = C_0 * PP[0][1];
    
    PP[0][0] -= K_0 * t_0;		 //����������Э����
    PP[0][1] -= K_0 * t_1;
    PP[1][0] -= K_1 * t_0;
    PP[1][1] -= K_1 * t_1;
            
    AngleFilter	+= K_0 * Angle_err;	 //�������
    Q_bias	+= K_1 * Angle_err;	 //�������
    Gyro_x   = Gyro - Q_bias;	         //���ֵ(�������)��΢��=���ٶ�    
}
/**************************************************************************
�������ܣ�һ�׻����˲�
��ڲ��������ٶȡ����ٶ�
����  ֵ����
**************************************************************************/
/*
void Yijielvbo(float angle_m, float gyro_m)
{
   angle = K1 * angle_m+ (1-K1) * (angle + gyro_m * 0.005);
}
*/



