#include "DriverFlash.h"
#define FLASH_TEST_SECTOR   (128)
#define FLASH_TEST_ADDR     (FLASH_TEST_SECTOR*2048)// FLASH_TEST_SECTOR*(2048)
#define DATA_SIZE           (32)
float32 Arg[8]={342.5,344.6,543,43.635,45.7};
uint8 FlashBuffer[32];  //?¡§¨°??o3???
uint8 *ptr=FlashBuffer;
//	ptr = (float32*)FLASH_TEST_ADDR;
void ReadFlash()
{
//	float32 *ptr;
//	ptr = (float32*)FLASH_TEST_ADDR;
//	for(uint8 len = 0 ; len < 32 ; len++)
//	{
//		FlashBuffer[len] = *(ptr + len);
//	}
 float32 *p=(float32*)ptr;
        for(uint8 i=0;i<8;i++){
          Arg[i]=*(p+i);
        }
        for(uint8 i=0;i<8;i++)Arg[i]+=10;
}
void WriteFlash()
{

  float32 *p=(float32*)ptr;
	LPLD_Flash_SectorErase(FLASH_TEST_ADDR);//2¨¢3y
        for(uint8 i=0;i<8;i++){
          *(p+i)=Arg[i];
        }
	DisableInterrupts;
	LPLD_Flash_ByteProgram((uint32)FLASH_TEST_ADDR, (uint32*)FlashBuffer, 32);
	EnableInterrupts;
}