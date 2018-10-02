#include "DriverUART.h"
UART_InitTypeDef uart3_init_struct;
extern int ExpectSpeed;
extern int ExpectOffset;
extern uint16 Distance;
void UARTInit(){
  
    uart3_init_struct.UART_Uartx = UART5;
    uart3_init_struct.UART_BaudRate = 115200;
    uart3_init_struct.UART_TxPin = PTE8;
    uart3_init_struct.UART_RxPin = PTE9;
    LPLD_UART_Init(uart3_init_struct);
  
}
void RemoteCtrl(){
  char cmd;
  cmd=LPLD_UART_GetChar(UART3);
  if(cmd=='Z')ExpectSpeed=0,ExpectOffset=0;
  if(cmd=='A')ExpectSpeed=150;
  if(cmd=='E')ExpectSpeed=-150;
  if(cmd=='G')ExpectOffset=150;
  if(cmd=='C')ExpectOffset=-150;
}
void UltraSoundInit(){
    uart3_init_struct.UART_Uartx = UART4;
    uart3_init_struct.UART_BaudRate = 9600;
    uart3_init_struct.UART_TxPin = PTC15;
    uart3_init_struct.UART_RxPin = PTC14;
      uart3_init_struct.UART_RxIntEnable = TRUE;    //使能接收中断
  uart3_init_struct.UART_RxIsr = UltraSoundISR;      //设置接收中断函数
    LPLD_UART_Init(uart3_init_struct);
    LPLD_UART_EnableIrq(uart3_init_struct);
}
void UltraSoundISR(){
    int tmp;
    tmp=(uint8)(LPLD_UART_GetChar(UART4))*256;
    LPLD_UART_RxFIFO_Flush(UART4);      
    tmp+=(uint8)(LPLD_UART_GetChar(UART4));
    LPLD_UART_RxFIFO_Flush(UART4);
    Distance=(uint16)(tmp);
}