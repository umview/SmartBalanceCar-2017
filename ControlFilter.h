#ifndef _CONTROLFILTER_H_
#define _CONTROLFILTER_H_ 
#include "AllInclude.h"
#define AccelMin 758  //���ٶȼ���Сֵ
#define AccelMax 2656  //���ֵ
//#define PhysicalZero 1961-26// 2300 //Zero //2520//1961 Min//  //��ƽ��ʱ���ٶȼ�ֵ
#define GyroIntegralTime 0.005//0.0045  //�����ǻ���ʱ��
#define K_AngleSpeed 0.9 //���ٶȻ����ں�ϵ��

#define K_Angle 0.65   //�ǶȻ����ں�ϵ��

//#define TurnGyroStandard 2000  //ת����ٶȻ�׼
#define dt 0.005//0.0045
#define K_Accel 0.005//0.002   
#define GyroCOE 3.313// 7.07503474//7.6617//13.29 //13.2266667 //13.466667 //6.7324//4.034146 //8.632456  //��������������ٶȵ�ת��ϵ��29.87593
void GetAngle();
void GetAngleSpeed();

void KalmanFilter(float32 Accel,float32 Gyro);
#endif