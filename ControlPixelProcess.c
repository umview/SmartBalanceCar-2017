#include "ControlPixelProcess.h"
uint8 PixelNear[128],PixelForward[128];
extern float32 ExpectOffset;
extern float32 ExpectOffsetLast;
struct PixelLineStruct PixelLineNear[10],PixelLineForward[10];
uint8 LineStepNear=0,LineStepForward=0,ForwardRight,ForwardLeft;
uint8 RoadWidth=50;//55
uint8 State=0;
uint8 hinder = 0,hinderCNT=0,hin_1_CNT = 0,hindermid,hinderforwrad = 0,hinderchange = 0,hinderchangeLast = 0,Stop = 0;
uint8 hinderRoadMin = 0,hinderRoadMinLast = 0,hinderRoadMax = 0,hinderRoadMaxLast = 0,hinderJump = 0,StopCNT = 0;
int16 StopAnti = 0;
int32 hinderdir = 0,hinderdirC = 0;
int8 AllHigh=0;
int8 Mode=0;
int8 CircleOut=0;
int8 cnt=0;
int8 RoadType=0,RoadTypeLast=0,hinderRoadType=0,hinderRoadTypeLast=0,hinderRealRoadLast = 0,hinderRealRoad = 0;
float32 hinderoffset,hinderadd = 0;
uint8 RoadWidthCnt=0;
int8 CrossRoad=0;
uint8 RoadWidthLast=0,RoadWidthNow=0;
uint8 RoadWidthArray[15]={0};
uint8 Object=0;
extern int ExpectSpeed;
uint8 PixelBinaryzation(){
    uint8 i,ThresholdNearMax,ThresholdNearMin,ThresholdForwardMax,ThresholdForwardMin,ThresholdNear,ThresholdForward;
    int32 PixelSumNear=0,PixelSumForward=0;
    //平滑处理
    
    for(i=1;i<127;i++){
        PixelNear[i] = (uint8)(0.15*PixelNear[i-1]+0.7*PixelNear[i]+0.15*PixelNear[i+1]);
        PixelForward[i] = (uint8)(0.15*PixelForward[i-1]+0.7*PixelForward[i]+0.15*PixelForward[i+1]);
    }
    
    //边界处理
    PixelNear[4] = PixelNear[5];   PixelNear[123] = PixelNear[122];
    PixelNear[3] = PixelNear[4];   PixelNear[124] = PixelNear[123];
    PixelNear[2] = PixelNear[3];   PixelNear[125] = PixelNear[124];
    PixelNear[1] = PixelNear[2];   PixelNear[126] = PixelNear[125];
    PixelNear[0] = PixelNear[1];   PixelNear[127] = PixelNear[126];

    PixelForward[4] = PixelForward[5];   PixelForward[123] = PixelForward[122];
    PixelForward[3] = PixelForward[4];   PixelForward[124] = PixelForward[123];
    PixelForward[2] = PixelForward[3];   PixelForward[125] = PixelForward[124];
    PixelForward[1] = PixelForward[2];   PixelForward[126] = PixelForward[125];
    PixelForward[0] = PixelForward[1];   PixelForward[127] = PixelForward[126];
    ThresholdNearMax=ThresholdNearMin=PixelNear[0];
    ThresholdForwardMax=ThresholdForwardMin=PixelForward[0];
    for(i=1;i<128;i++){
      if(PixelNear[i]>ThresholdNearMax)ThresholdNearMax=PixelNear[i];
      if(PixelNear[i]<ThresholdNearMin)ThresholdNearMin=PixelNear[i];
      if(PixelForward[i]>ThresholdForwardMax)ThresholdForwardMax=PixelForward[i];
      if(PixelForward[i]<ThresholdForwardMin)ThresholdForwardMin=PixelForward[i];
    }
    
    //计算阈值
    //ThresholdNear=(ThresholdNearMax+ThresholdNearMin)/2+ThresholdNearOffset;
    //ThresholdForward=(ThresholdForwardMax+ThresholdForwardMax)/2+ThresholdForwardOffset;
    
    ThresholdNear=230;
    ThresholdForward=230;
    for(i=0;i<=127;i++){//二值化
      PixelSumNear+=PixelNear[i];
      PixelSumForward+=PixelForward[i];
      PixelNear[i]=PixelNear[i]>ThresholdNear?255:0;
      PixelForward[i]=PixelForward[i]>ThresholdForward?255:0;
    }
    

    if(((PixelSumNear/128)>NearPixelHighThreshold)){
      AllHigh=1;
    }else AllHigh=0;
    if(((PixelSumNear/128)>200) && (PixelSumNear/128)<NearPixelHighThreshold){
      CircleOut=1;
    }else 
      CircleOut=0;
    //if((PixelSumForward/128)>150)CrossRoad=0;
    //else CrossRoad=0;
    //AllHigh=0;
   
    /*
        for(i=0;i<=127;i++){//二值化
      if(PixelNear[i]>200)PixelNear[i]=255;
      else PixelNear[i]=0;
     // PixelNear[i]=(PixelNear[i]>100)?255:0;
      //PixelForward[i]=PixelForward[i]>100?255:0;
    }
    
    return 0;
    */
    /*
    if(((PixelSumNear/128)>NearPixelHighThreshold)  || ((PixelSumNear/128)<NearPixelLowThreshold))
      return 1;
    //1:近线全白或全黑
    else return 0;
    */
    return 0;
}
uint8 PixelIndex(uint8 Pixel[],struct PixelLineStruct PixelLine[],uint8 length){
  uint8 i,StepNum=0;
  //Stop = 0;
 // static uint8 CntForRoadType=0;
 // static int16 RoadWidthTmp=0;
 // static uint8 LightWidth=0;

  for(i=0;i<=126;i++){
    if(Pixel[i]==0&&Pixel[i+1]==255){//上升跳变
      PixelLine[StepNum].Type=UP;
      PixelLine[StepNum].Position=i;
      StepNum++;
    }else if(Pixel[i]==255&&Pixel[i+1]==0){//下降跳变
      PixelLine[StepNum].Type=DOWN;
      PixelLine[StepNum].Position=i;
      StepNum++;
    }
    if(StepNum>=length)break;//需要异常处理，PixelLine溢出，默认长度10
  }/*
  if(StepNum==2 && PixelLine[0].Type==UP){
    RoadType=1;//正常赛道
    RoadWidthTmp+=PixelLine[1].Position-PixelLine[0].Position;
  }else RoadType=0;
  if(RoadType==1 && RoadTypeLast==1)CntForRoadType++;
  else{
    CntForRoadType=0;
    RoadWidthTmp=0;
  }
  if(CntForRoadType>=ValueforRoadType)RoadWidth=(uint8)(RoadWidth/(ValueforRoadType*1.0));
  */
    /*****障碍****/
  //if(RoadTypeLast==1 && ((StepNum==4 && PixelLine[0].Type=UP)||()

  
  if(/*ExpectSpeed>0&&*/StepNum>8&&!StopCNT&&(Stop==0||Stop==2)&&!StopAnti){
    Stop++;
    StopAnti++;
  }
  if(Stop==1||Stop==3) StopCNT++;
  if(StopAnti)StopAnti++;
  if(StopAnti>300)StopAnti = 0;
  
  return StepNum;
}
int16 PixelForwardIndex(uint8 *Pixel){
  uint8 i=0,j=0;
  ForwardRight = 127;ForwardLeft = 0;
  if(Pixel[64]==0){
    for(i=64;i<=126;i++){
      if(Pixel[i]==0 && Pixel[i+1]==255){
        ForwardRight = i;
        break;
      }
    }
    for(j=64;j>=1;j--){
      if(Pixel[j]==0 && Pixel[j-1]==255){
        ForwardLeft = j;
        break;
      }
    }
    if(i>=120 || j<=10)return 0;
    return -(i+j)/2;
  }else{
    for(i=64;i<=126;i++){
      if(Pixel[i]==255 && Pixel[i+1]==0)break;
    }
    for(j=64;j>=1;j--){
      if(Pixel[j]==255 && Pixel[j-1]==0)break;
    }
    if(i>=120 || j<=10)return 0;
    return (i+j)/2;
  }
}
float32 PixelSearch(uint8 *Pixel,int8 dir){
  uint8 i=0;
  if(dir<0){
    for(i=0;i<=126;i++){
      if(Pixel[i]==0 && Pixel[i+1]==255)break;
    }
    if(i<=110)return i+(50/2.0)-63.5;
    else return 0;
  }else if(dir>0){
    for(i=127;i>=1;i--){
      if(Pixel[i]==0 && Pixel[i-1]==255)break;
    }
    if(i>=10)return i-(50/2.0)-63.5;
    else return 0;
  }else return 0;
}
float32 ModeIdentification(struct PixelLineStruct *PixelLine,uint8 PixelStep){
  //uint8 i;
  float32 ExpectOffsetTmp[3]={0};
  float32 ExpectOffsetFinal=0;
  State=0;
  RoadTypeLast=RoadType;
  RoadWidthLast=RoadWidthNow;
  hinderRoadTypeLast = hinderRoadType;
  hinderRoadType = 0;
  hinderchangeLast = hinderchange;
  if(PixelStep!=2) hinderchange = 0;
  hinderRoadMinLast = hinderRoadMin;
  hinderRoadMin = 0;
  hinderRoadMaxLast = hinderRoadMax;
  hinderRoadMax = PixelLine[PixelStep].Position;

  
  if(PixelStep==1){//单跳变，丢线
    RoadWidthNow=0;
   // RoadType=2;
    if(PixelLine[0].Type==UP){//                                 ____\~~~~~~
      RoadType=3;
      ExpectOffsetFinal=(PixelLine[0].Position+(RoadWidth/2.0)-63.5);
    }else{//                                                      ~~~~~\______
      RoadType=2;
      ExpectOffsetFinal=(PixelLine[0].Position-(RoadWidth/2.0)-63.5);
    }
  }else if(PixelStep==2){//
    if(PixelLine[0].Type==UP){//                                 ______\~~~~Num~~~~\______   - Left
      RoadWidthNow=PixelLine[1].Position-PixelLine[0].Position;
      //if(Mode==CNT+4 || CircleOut==1)Mode=0;
      if((PixelLine[1].Position-PixelLine[0].Position)>RoadWidthThreshold){
        RoadType=1;hinderRoadType=2;hinderRealRoad = PixelLine[1].Position-PixelLine[0].Position;hinderRoadMin = PixelLine[0].Position;
        if(hinderchange==8) hinderchange = 8;
        else hinderchange++;
        ExpectOffsetFinal=(PixelLine[1].Position+PixelLine[0].Position)/2.0-63.5;
        hinderoffset = ExpectOffsetFinal;
      }else{ //                                                   _____\~\____
        RoadType=0;
        RoadWidthNow=0;
        ExpectOffsetFinal=0;
      }
    }else{//
      RoadWidthNow=0;
      //if(PixelLine[0].Position>(127-PixelLine[1].Position)){//     ~~~~~~~~~~~~~~\___\~~
      //ExpectOffsetTmp=(PixelLine[0].Position-(RoadWidth/2.0)-63.5);
      if(ABS((PixelLine[0].Position-(RoadWidth/2.0)-63.5)-ExpectOffsetLast) < ABS((PixelLine[1].Position+(RoadWidth/2.0)-63.5)-ExpectOffsetLast)){
        RoadType=2;
        ExpectOffsetFinal=(PixelLine[0].Position-(RoadWidth/2.0)-63.5);
      }else{ //                                                   ~~~\____\~~~~~~~~~~~~~~~~~~
        RoadType=3;
        ExpectOffsetFinal=(PixelLine[1].Position+(RoadWidth/2.0)-63.5);
      }
    }
  }else if(PixelStep==3){//
    /*
    if(PixelLine[0].Type==UP){
      if((PixelLine[1].Position-PixelLine[0].Position)>(127-PixelLine[2].Position))//__\~~~~~~~~~~~~\___\~~
        return (PixelLine[0].Position+PixelLine[1].Position)/2.0-63.5;
      else                                                 //                  ___\~~\__\~~~~~~~~~~~~~~~~
        return (PixelLine[2].Position+(RoadWidth/2.0)-63.5);
    }else{
      if((PixelLine[2].Position-PixelLine[1].Position)>PixelLine[0].Position)//~~~~\__\~~~~~~~~~~~~~~\__
        return (PixelLine[2].Position+PixelLine[1].Position)/2.0-63.5;
      else                              //                                  ~~~~~~~~~~~~~~~~\___\~~\__
        return (PixelLine[0].Position-(RoadWidth/2.0)-63.5);
    }
    */
    if(PixelLine[0].Type==UP){
      ExpectOffsetTmp[0]=(PixelLine[0].Position+PixelLine[1].Position)/2.0-63.5;
      ExpectOffsetTmp[1]=PixelLine[2].Position+(RoadWidth/2.0)-63.5;
      if(ABS(ExpectOffsetTmp[0]-ExpectOffsetLast)>=ABS(ExpectOffsetTmp[1]-ExpectOffsetLast)){
        RoadType=3;
        RoadWidthNow=0;
        ExpectOffsetFinal=ExpectOffsetTmp[1];
      }else{
        RoadType=1;
        RoadWidthNow=PixelLine[1].Position-PixelLine[0].Position;
        ExpectOffsetFinal=ExpectOffsetTmp[0];
      }
    }else{
      ExpectOffsetTmp[0]=(PixelLine[2].Position+PixelLine[1].Position)/2.0-63.5;
      ExpectOffsetTmp[1]=PixelLine[0].Position-(RoadWidth/2.0)-63.5;
      if(ABS(ExpectOffsetTmp[0]-ExpectOffsetLast)>=ABS(ExpectOffsetTmp[1]-ExpectOffsetLast)){
        RoadType=2;
        RoadWidthNow=0;
        ExpectOffsetFinal=ExpectOffsetTmp[1];
    }else{ 
      RoadWidthNow=PixelLine[2].Position-PixelLine[1].Position;
      RoadType=1;
      ExpectOffsetFinal=ExpectOffsetTmp[0];
     }
    }
   }else if(PixelStep==4){
    if(PixelLine[0].Type==UP){

      hindermid = (PixelLine[2].Position + PixelLine[1].Position)/2;
      if(!hinder) hinderRealRoadLast = hinderRealRoad;
      hinderRealRoad = PixelLine[3].Position-PixelLine[0].Position;
      if((PixelLine[1].Position-PixelLine[0].Position)>(PixelLine[3].Position-PixelLine[2].Position)){    
        if(((PixelLine[2].Position-PixelLine[1].Position)>1&&(PixelLine[1].Position-PixelLine[0].Position)>14/*&&(PixelLine[3].Position-PixelLine[2].Position)>4&&(hindermid>69&&hindermid<84)*/)||(0)){
          if(hinder==1) hinder = 2;
          if(ABS(hinderRealRoad-hinderRealRoadLast)<5&&(!hinder||hinderforwrad==1)&&hinderRoadTypeLast==2) hinder = 1;
        }
      }else{    
        if(((PixelLine[2].Position-PixelLine[1].Position)>2&&(PixelLine[3].Position-PixelLine[2].Position)>14/*&&(PixelLine[1].Position-PixelLine[0].Position)>1&&(hindermid>32&&hindermid<50)*/)||(0)){
          if(hinder==1) hinder = 2;
          if(ABS(hinderRealRoad-hinderRealRoadLast)<5&&(!hinder||hinderforwrad==1)&&hinderRoadTypeLast==2) hinder = 1;
        }
      }
      hinderRoadMin = PixelLine[0].Position;

      if((PixelLine[1].Position-PixelLine[0].Position)>(PixelLine[3].Position-PixelLine[2].Position)){//__\~~~~~~~~~\___\~~\__
        RoadWidthNow=PixelLine[1].Position-PixelLine[0].Position;
        RoadType=1;hinderRoadType=1;
        if(hinder==2) hinderdir = -1;
        ExpectOffsetFinal=(PixelLine[0].Position+PixelLine[1].Position)/2.0-63.5;
        hinderoffset = ExpectOffsetFinal;
      }else{                                                              //                        __\~~\__\~~~~~~~~~~~~\__
        RoadType=1;hinderRoadType=1;
        RoadWidthNow=PixelLine[3].Position-PixelLine[2].Position;
        if(hinder==2) hinderdir = 1;
        ExpectOffsetFinal=(PixelLine[2].Position+PixelLine[3].Position)/2.0-63.5;
        hinderoffset = ExpectOffsetFinal;
      }
    }else{// 
      if((PixelLine[2].Position-PixelLine[1].Position)>RoadWidthThreshold){//                      ~~~\__\~~~~~~Num~~~~~~~\__\~~
        RoadType=1;
        RoadWidthNow=PixelLine[2].Position-PixelLine[1].Position;
        ExpectOffsetFinal=(PixelLine[1].Position+PixelLine[2].Position)/2.0-63.5;
      }else{
        RoadWidthNow=0;
        if(PixelLine[0].Position>(127-PixelLine[3].Position)){//                                  ~~~~~~~~~~~~~~\__\~~\__\~~~
          RoadType=2;
          ExpectOffsetFinal=(PixelLine[0].Position-(RoadWidth/2.0)-63.5);
        }else{                                    //                                                ~~~\__\~~\__\~~~~~~~~~~~~~~
          RoadType=3;
          ExpectOffsetFinal=(PixelLine[3].Position+(RoadWidth/2.0)-63.5);
        }
      }
    }
  }else{
    RoadType=0;
    RoadWidthNow=0;
    ExpectOffsetFinal=0;
  }
  if(AllHigh==1){
    RoadType=5;
    ExpectOffsetFinal=0;
  }

  /******圆环*****/
   /*
  if(Mode==1 || Mode==2){
    Mode=3;
    ExpectOffsetFinal=Offset;
  }
  if(Mode==CNT+4){
  if(PixelForwardIndex(PixelForward)<=-10)ExpectOffsetFinal=Offset;
  Mode=0;
  }
  if(Mode>=3 && Mode<=CNT+3){
    Mode++;
    ExpectOffsetFinal=PixelSearch(PixelNear,(Offset>0?1:-1));
  }
  if(Mode==0){
  if(PixelForwardIndex(PixelForward)<=-10 && AllHigh==1 && CrossRoad==0 && RoadTypeLast==1){
    Mode=1;
    RoadType=5;
    ExpectOffsetFinal=Offset;
  }
  }
  */
  /*
    if(Mode==1 || Mode==2){
    Mode=3;
    RoadType=4;
    ExpectOffsetFinal=Offset;
  }
  if(Mode==CNT+4){
    if(RoadType ==1 && RoadTypeLast==1 && ABS(RoadWidthNow-40)<10 && ABS(RoadWidthLast-40)<10){
      Mode=0;
      State=100;
    }else {
      RoadType=4;
      ExpectOffsetFinal=PixelSearch(PixelNear,Offset);
    }
  }
  if(Mode>=3 && Mode<=CNT+3){
    Mode++;
    RoadType=4;
    ExpectOffsetFinal=PixelSearch(PixelNear,Offset);
  }
  if(Mode==0){
  if((RoadWidthLast>50 || RoadType==5) && (PixelForwardIndex(PixelNear)<-10)){
    Mode=1;
    RoadType=4;
    ExpectOffsetFinal=Offset;
    }
  }
  */
  
  /*//finish
  if(Mode==1 || Mode==2){
    Mode=3;
    ExpectOffsetFinal=Offset;
  }
  if(Mode==CNT+4){
    if(RoadType ==1 && RoadTypeLast==1){
      Mode=0;
      //State=100;
    }else {
      ExpectOffsetFinal=PixelSearch(PixelNear,(Offset>0?1:-1));
    }
  }
  if(Mode>=3 && Mode<=CNT+3){
    Mode++;
    ExpectOffsetFinal=PixelSearch(PixelNear,(Offset>0?1:-1));
  }
  if(Mode==0){
  if(PixelForwardIndex(PixelForward)<=-10 && AllHigh==1 && CrossRoad==0 && RoadTypeLast==1){
    Mode=1;
    RoadType=5;
    ExpectOffsetFinal=Offset;
  }
  }*/
  
  //float32 OffsetSet[8] = {4.5,12.5,20,15.5,11,5.5,3.3};
    float32 OffsetSet[8] = {5,13,6,2,19,9,-6};
  
  if(Mode>0){
    ExpectOffsetFinal=2*(PTB17_I==0?1:-1)*OffsetSet[Mode-1];
    Mode++;
    if(Mode-1==6)Mode = 0;
  }
  if(Mode==0){
    if(PixelForwardIndex(PixelForward)<=-15&&AllHigh==1&&(RoadWidthLast>80 || RoadWidthLast==0)&&(ForwardRight<100&&ABS(127-ForwardLeft-ForwardRight)<5&&ForwardLeft>15)){
      Mode = 1;
      ExpectOffsetFinal=20;
    }
  }
  /*
  if(Mode==0){
    if(PixelForwardIndex(PixelForward)<=-10 && AllHigh==1 && CrossRoad==0 && RoadTypeLast==1){
    Mode=1;
    RoadType=5;
    ExpectOffsetFinal=Offset;
    Buzzer(1);
    }
  }
  if(Mode==1){
    if(PixelForwardIndex(PixelForward)<=-10){
      ExpectOffsetFinal=Offset;
    Buzzer(0);
    Mode=0;
    }
  }
  */
  /************障碍***********/
  if(ABS(RoadWidthNow-RoadWidthLast)<5)cnt++;
  if(Object==1 && ABS(RoadWidthNow-RoadWidthLast)<5 && RoadWidthNow>30)Object=0;
  if(RoadWidthNow>RoadWidthLast && (RoadWidthLast-RoadWidthNow)>8 && RoadWidthLast>30){
   Object=1;
 //  ExpectOffsetFinal*=1.3;
  }
   /******十字**************/
  if(PixelForwardIndex(PixelForward)>20 && AllHigh==1){
    RoadType=4;
    ExpectOffsetFinal=0;
  }
  
  
  /*************************/
  RoadWidthArray[0]=RoadWidthArray[1];
  RoadWidthArray[1]=RoadWidthArray[2];
  RoadWidthArray[2]=RoadWidthArray[3];
  RoadWidthArray[3]=RoadWidthArray[4];
  RoadWidthArray[4]=RoadWidthArray[5];
  RoadWidthArray[5]=RoadWidthArray[6];
  RoadWidthArray[6]=RoadWidthArray[7];
  RoadWidthArray[7]=RoadWidthArray[8];
  RoadWidthArray[8]=RoadWidthArray[9];
  RoadWidthArray[9]=RoadWidthArray[10];
  RoadWidthArray[10]=RoadWidthArray[11];
  RoadWidthArray[11]=RoadWidthArray[12];
  RoadWidthArray[12]=RoadWidthArray[13];
  RoadWidthArray[13]=RoadWidthArray[14];
  RoadWidthArray[14]=RoadWidthNow;
  //zhangai
  if(hinderCNT)  hinderCNT--;
  if(hin_1_CNT>15) hinder = 0;
  if(hinder==1||hinder==2||hinder==4||hinder==6)  hin_1_CNT++;
  else  hin_1_CNT = 0;
  
  //if(ABS(RoadWidthNow-55)<6) hinderchange++;
  //else hinderchange = 0;
  //if(hinderchange>=8) hinderchange = 8;
  //if(hinderchangeLast==8&&ABS(RoadWidthLast-RoadWidthNow)>15&&(RoadWidthLast>RoadWidthNow)&&ABS(hinderRealRoad-hinderRealRoadLast)<5&&ABS(hinderRoadMin-hinderRoadMinLast)<5&&ABS(hinderRoadMaxLast-hinderRoadMax)<5/*&&PixelStep>1*/) hinder = 6;
  if(hinder!=0&&hinderdir!=0)  hinderdirC = hinderdir;
  if(((hinder==2||hinder==5)&&!hinderCNT)||((hinder==2||hinder==5)&&hinderforwrad==1))
  //if((hinder==6)&&!hinderCNT)
  {
     hinderCNT = 17;
     hinder = 3;
     hinderforwrad = 0;
     
     //hinderdirC = hinderdir;
  }
  if(hinder==4&&ABS(RoadWidthNow-RoadWidthLast)<5&&hinderRoadType>0) hinder = 5;
  //if(hinder==0&&(RoadWidthNow-RoadWidthLast)>17&&RoadWidthLast!=0&&RoadWidthNow!=0&&hinderRoadTypeLast==1&&hinderRoadType==1)  hinder = 4;  
  if(hinder==0&&(RoadWidthArray[13]-RoadWidthArray[14])>17/*&&RoadWidthLast!=0&&RoadWidthNow!=0*/&&hinderRoadTypeLast>0&&hinderRoadType>0)  hinder = 4; 
  /*
  if(hinder==7&&!hinderCNT){
    hinderCNT = 20;
    hinder = 3;
    hinderforwrad = 1;
    //hinderdirC = hinderdir;
  }*/
  if(hinder==3&&hinderCNT>1){
    if(hinderCNT>13) hinderadd = 8;
    else hinderadd = 13;
    if(hinderdirC>0){ 
      //if(PixelStep==2||PixelStep==4)
      hinderJump = PixelLine[PixelStep-1].Position;
    }
    if(hinderdirC<0) hinderJump = PixelLine[0].Position;
    ExpectOffsetFinal = (hinderJump-hinderdirC*hinderadd-63.5);
    //ExpectOffsetFinal = (hinderoffset*1.6+hinderdirC*hinderadd)*1.0;
    
    //ExpectOffsetFinal = (PixelSearch(PixelNear,hinderdir)+hinderdir*16)*1.1;
    //if(hinderdir>0)  ExpectOffsetFinal = (PixelSearch(PixelNear,hinderdir)+9)*1.1;
    //else ExpectOffsetFinal = (PixelSearch(PixelNear,hinderdir)-9)*1.1;
  }
  if(hinderCNT==1){
    hinder = 0;
    hinderforwrad = 0;
    hinderdirC = 0;
  }


  return ExpectOffsetFinal;
}
/*
uint8 StraighWayIndentification(uint8 ExpectOffset,uint8 cnt){
  static int8 Mode=0;
  static uint8 ExpectOffsetLast=0;
  if(ABS(ExpectOffset-ExpectOffsetLast)<7)Mode++;
  else Mode--;
  if(Mode>=20)Mode=20;
  if(Mode<=0)Mode=0;
  if(Mode>=cnt)return 1;
  
  else return 0;
}
uint8 TrendIdentification(uint8 RoadWidth,uint8 cnt){
  static uint8 RoadWidthLast=0;
  static uint8 Mode=0;
  if(RoadWidth>=RoadWidthLast){
    RoadWidthLast=RoadWidth;
    Mode++;
  }else Mode=0;
  if(Mode>=cnt)return 1;
  else return 0;
}
*/