#include "delay.h"
#include "usart1.h"
#include "sys.h"
#include "stdio.h"
#include "string.h"
#include "usart1.h"
#include "usart3.h"
#include "OLED.h"
#include "as608.h"
#include "HardwareIIC.h"

int main(void)
{
	delay_init();		 // 延时函数初始化
	Hardware_I2C_Init(); // 端口初始化
	OLED_Init();		 // OLED初始化
	OLED_CLS();			 // 清空屏幕
	uart1_init(9600);	 // 串口1
	usart3_init(115200);	 // 串口3 初始化
	u8 Show_Temp[16] = {0};
	u16 i = 0;
	while (PS_HandShake(&AS608Addr)) // 与AS608模块握手
	{
		sprintf(Show_Temp, "连接中...%d",i++);
		OLED_Print(0, 0, Show_Temp, TYPE16X16, TYPE8X16);
		memset(Show_Temp, 16, 0);
		delay_ms(1000);
		OLED_CLS();
	}
	while (1)
	{
		sprintf(Show_Temp, "连接中OK");
		OLED_Print(0, 16, Show_Temp, TYPE16X16, TYPE8X16);
		memset(Show_Temp, 16, 0);
	}
}
