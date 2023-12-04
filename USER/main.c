#include "delay.h"
#include "usart1.h"
#include "sys.h"
#include "stdio.h"
#include "string.h"
#include "usart1.h"
#include "usart2.h"
#include "oled.h"

int main(void){
	IIC_Init(); /* 初始化IIC */
	LCD_Init(); /* 初始化OLED */
	char Show_Temp[16] = {0};
	LCD_Fill(0);
	sprintf(Show_Temp, "Test %d",100);
	LCD_Print(0, 0, Show_Temp, TYPE16X16, TYPE8X16);
	memset(Show_Temp, 16, 0);
	delay_ms(1000);
}



















