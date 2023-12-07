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
	delay_init();		 // ��ʱ������ʼ��
	Hardware_I2C_Init(); // �˿ڳ�ʼ��
	OLED_Init();		 // OLED��ʼ��
	OLED_CLS();			 // �����Ļ
	uart1_init(9600);	 // ����1
	usart3_init(115200);	 // ����3 ��ʼ��
	u8 Show_Temp[16] = {0};
	u16 i = 0;
	while (PS_HandShake(&AS608Addr)) // ��AS608ģ������
	{
		sprintf(Show_Temp, "������...%d",i++);
		OLED_Print(0, 0, Show_Temp, TYPE16X16, TYPE8X16);
		memset(Show_Temp, 16, 0);
		delay_ms(1000);
		OLED_CLS();
	}
	while (1)
	{
		sprintf(Show_Temp, "������OK");
		OLED_Print(0, 16, Show_Temp, TYPE16X16, TYPE8X16);
		memset(Show_Temp, 16, 0);
	}
}
