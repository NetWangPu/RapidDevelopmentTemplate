#ifndef __SOFTWAREIIC_H
#define __SOFTWAREIIC_H
#include "sys.h"

#define OLED_SCL_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_6)//SCL
#define OLED_SCL_Set() GPIO_SetBits(GPIOB,GPIO_Pin_6)

#define OLED_SDA_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_7)//DIN
#define OLED_SDA_Set() GPIO_SetBits(GPIOB,GPIO_Pin_7)

#define OLED_RES_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_8)//RES
#define OLED_RES_Set() GPIO_SetBits(GPIOB,GPIO_Pin_8)


#define OLED_CMD  0	//Ð´ÃüÁî
#define OLED_DATA 1	//Ð´Êý¾Ý

void Software_I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(u8 dat);

 
#endif /* __SOFTWAREIIC_H */
















