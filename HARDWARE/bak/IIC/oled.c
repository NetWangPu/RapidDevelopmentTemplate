/* GND    ��Դ�� */
/* VCC  ��5V��3.3v��Դ */
/* D0   ��PD6��SCL�� */
/* D1   ��PD7��SDA�� */
/* RES  ��PD4 */
/* DC   ��PD5 */
/* CS   ��PD3 */

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"

// OLED���Դ�
// ��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

// ��ʼ�ź�
void I2C_Start(void)
{
	OLED_SDIN_Set;
	OLED_SCLK_Set;
	delay_us(1);
	OLED_SDIN_Clr;
	delay_us(1);
	OLED_SCLK_Clr;
	delay_us(1);
}

// �����ź�
void I2C_Stop(void)
{
	OLED_SDIN_Clr;
	OLED_SCLK_Set;
	delay_us(1);
	OLED_SDIN_Set;
}

// �ȴ��ź���Ӧ
void I2C_WaitAck(void) // �������źŵĵ�ƽ
{
	OLED_SDIN_Set;
	delay_us(1);
	OLED_SCLK_Set;
	delay_us(1);
	OLED_SCLK_Clr;
	delay_us(1);
}

// д��һ���ֽ�
void Send_Byte(u8 dat)
{
	u8 i;
	for (i = 0; i < 8; i++)
	{
		if (dat & 0x80) // ��dat��8λ�����λ����д��
		{
			OLED_SDIN_Set;
		}
		else
		{
			OLED_SDIN_Clr;
		}
		delay_us(1);
		OLED_SCLK_Set;
		delay_us(1);
		OLED_SCLK_Clr; // ��ʱ���ź�����Ϊ�͵�ƽ
		dat <<= 1;
	}
}

/// @brief ��SSD1106д��1byte
/// @param dat д�������/����
/// @param cmd ����0������1
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if (cmd)
	{
		Send_Byte(0x40);
	}
	else
	{
		Send_Byte(0x00);
	}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}

/* ������ʾ���꣬x y */
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
	OLED_WR_Byte(0xB0 + y, OLED_CMD);				  /* �����е�ַ */
	OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD); /* �����иߵ�ַ */
	OLED_WR_Byte((x & 0x0F) | 0x01, OLED_CMD);		  /* �����е͵�ַ */
}

/* ����OLED��ʾ */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); /* ���õ�ɱ����� */
	OLED_WR_Byte(0X14, OLED_CMD); /* ����ɱ� */
	OLED_WR_Byte(0XAF, OLED_CMD); /* ��oled��ʾ */
}

/* �ر�OLED��ʾ */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); /* ���õ�ɱ����� */
	OLED_WR_Byte(0X10, OLED_CMD); /* �ص�ɱ� */
	OLED_WR_Byte(0XAE, OLED_CMD); /* �ر�oled��ʾ */
}

/* �������� */
void OLED_Clear(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xB0 + i, OLED_CMD); /* ����ҳ��ַ��0~7�� */
		OLED_WR_Byte(0x00, OLED_CMD);	  /* ������ʾλ�á��е͵�ַ */
		OLED_WR_Byte(0x10, OLED_CMD);	  /* ������ʾλ�á��иߵ�ַ */
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0x00, OLED_DATA); /* д��հ����� */
	}
}

/* ��ָ��λ����ʾһ���ַ�,���������ַ� */
/* x:0~127 */
/* y:0~63 */
/* mode:0,������ʾ;1,������ʾ */
/* size:ѡ������ 16/12 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
	uint8_t c = 0, i = 0;
	c = chr - ' ';			/* �õ�ƫ�ƺ��ֵ */
	if (x > Max_Column - 1) /* ��ֹԽ�� */
	{
		x = 0;
		y = y + 2;
	}
	if (SIZE == 16)
	{
		OLED_Set_Pos(x, y);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
		OLED_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
	}
	else
	{
		// OLED_Set_Pos(x, y + 1);
		// for (i = 0; i < 6; i++)
		// OLED_WR_Byte(F6x8[c][i], OLED_DATA);
	}
}
// m^n����
u32 oled_pow(uint8_t m, uint8_t n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}
// ��ʾ2������
// x,y :�������
// len :���ֵ�λ��
// size:�����С
// mode:ģʽ	0,���ģʽ;1,����ģʽ
// num:��ֵ(0~4294967295);
void OLED_ShowNum(uint8_t x, uint8_t y, u32 num, uint8_t len, uint8_t size)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size / 2) * t, y, ' ');
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size / 2) * t, y, temp + '0');
	}
}

/* ��ʾһ���ַ��� */
void OLED_ShowString(uint8_t x, uint8_t y, char *chr)
{
	uint8_t j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j]);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

/* ��ʾ���� */
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
	uint8_t t;
	OLED_Set_Pos(x, y);
	for (t = 0; t < 16; t++)
	{
		OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
	}
	OLED_Set_Pos(x, y + 1);
	for (t = 0; t < 16; t++)
	{
		OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
	}
}

/* ��ʾһ������ */
void OLED_ShowCnString(uint8_t x, uint8_t y, uint16_t index, uint8_t HowChar)
{
	uint16_t i;
	for (i = 0; i < HowChar; i++)
	{
		OLED_ShowCHinese(x, y, index + i);
		x = (x + 16) % 128;
	}
}

/* ��ʾ����ͼƬ */
void OLED_DrawLight(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint16_t j = 0;
	uint8_t x, y;

	y1 /= 8;
	for (y = 0; y < y1; y++)
	{
		OLED_Set_Pos(x0, y0);
		for (x = 0; x < x1; x++)
		{
			OLED_WR_Byte(LightBmp[j++], OLED_DATA);
		}
		y0++;
	}
}

/* ��ʾ����ͼƬ */
void OLED_DrawSet(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint16_t j = 0;
	uint8_t x, y;

	y1 /= 8;
	for (y = 0; y < y1; y++)
	{
		OLED_Set_Pos(x0, y0);
		for (x = 0; x < x1; x++)
		{
			OLED_WR_Byte(SetBmp[j++], OLED_DATA);
		}
		y0++;
	}
}

/* ��ʾ����ͼƬ */
void OLED_DrawFmq(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint16_t j = 0;
	uint8_t x, y;

	y1 /= 8;
	for (y = 0; y < y1; y++)
	{
		OLED_Set_Pos(x0, y0);
		for (x = 0; x < x1; x++)
		{
			OLED_WR_Byte(FmqBmp[j++], OLED_DATA);
		}
		y0++;
	}
}

/* ��ʼ��SSD1306 */
void OLED_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); /* ��GPIOA��Bʱ�� */
	GPIO_StructInit(&GPIO_InitStructure);				  /* ��ʼ��GPIO�ṹ */
	/* DIN PB10 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  /* ��©��� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /* �ٶ�50MHz */
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  /*  */
	GPIO_SetBits(GPIOB, GPIO_Pin_10);				  /* ���� */
	/* CLK PB11 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /* ������� */
	GPIO_Init(GPIOB, &GPIO_InitStructure);			 /*  */
	GPIO_SetBits(GPIOB, GPIO_Pin_11);				 /* ���� */

	/* ��λ */
	delay_ms(200);

	/* ����oledģʽ */
	OLED_WR_Byte(0xAE, OLED_CMD); /* �ر�oled��ʾ����˯��ģʽ��������ʾAFH */
	OLED_WR_Byte(0x00, OLED_CMD); /* �����е͵�ַ��00H-0FH */
	OLED_WR_Byte(0x10, OLED_CMD); /* �����иߵ�ַ��10H-1FH */
	OLED_WR_Byte(0x40, OLED_CMD); /* ����RAM��ʾ��ʼ�ߵ�ַ */
	OLED_WR_Byte(0x81, OLED_CMD); /* ���öԱȶ����� */
	OLED_WR_Byte(0xCF, OLED_CMD); /* �Աȶ�����ΪCFH������Ϊ256�� */
	OLED_WR_Byte(0xA1, OLED_CMD); /* 0xA0���ҷ��� 0xA1���� */
	OLED_WR_Byte(0xC8, OLED_CMD); /* 0xC0���·��� 0xC8���� */
	OLED_WR_Byte(0xA6, OLED_CMD); /* ����������ʾ��A7H������ʾ */
	OLED_WR_Byte(0xA8, OLED_CMD); /* ���ø��ñ����� */
	OLED_WR_Byte(0x3F, OLED_CMD); /* ���ñ�1/64*/
	OLED_WR_Byte(0xD3, OLED_CMD); /* ������ʾƫ����(0x00~0x3F) */
	OLED_WR_Byte(0x00, OLED_CMD); /* ��ʾ��ƫ�� */
	OLED_WR_Byte(0xD5, OLED_CMD); /* ����ʱ��Ƶ�ʺ�ˢ�������� */
	OLED_WR_Byte(0x80, OLED_CMD); /* ����ΪĬ��ֵ */
	OLED_WR_Byte(0xD9, OLED_CMD); /* ����Ԥ����������� */
	OLED_WR_Byte(0xF1, OLED_CMD); /* 1-15��DCLKΪһ���� */
	OLED_WR_Byte(0xDA, OLED_CMD); /* ����Ӳ�������������� */
	OLED_WR_Byte(0x12, OLED_CMD); /* ����COM��������ӳ�� */
	OLED_WR_Byte(0xDB, OLED_CMD); /* ����vcomh���ȼ����� */
	OLED_WR_Byte(0x40, OLED_CMD); /* ����vcomh���ȼ� */
	OLED_WR_Byte(0x20, OLED_CMD); /* �����ڴ��ַģʽ���� */
	OLED_WR_Byte(0x02, OLED_CMD); /* 00Hˮƽģʽ��01H��ֱģʽ��10Hҳģʽ */
	OLED_WR_Byte(0x8D, OLED_CMD); /* ���õ�ɱÿ������� */
	OLED_WR_Byte(0x14, OLED_CMD); /* ��ɱÿ����ر�Ϊ10H */
	OLED_WR_Byte(0xA4, OLED_CMD); /* �ر�RAM������ʾ(0xa4/0xa5) */
	OLED_WR_Byte(0xAF, OLED_CMD); /* ����oled��ʾ */

	OLED_Clear();		/* ���� */
	OLED_Set_Pos(0, 0); /* �ڴ�ָ�븴λ��0,0 */
}
