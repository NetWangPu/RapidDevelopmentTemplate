/* GND    电源地 */
/* VCC  接5V或3.3v电源 */
/* D0   接PD6（SCL） */
/* D1   接PD7（SDA） */
/* RES  接PD4 */
/* DC   接PD5 */
/* CS   接PD3 */

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"

// OLED的显存
// 存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

// 起始信号
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

// 结束信号
void I2C_Stop(void)
{
	OLED_SDIN_Clr;
	OLED_SCLK_Set;
	delay_us(1);
	OLED_SDIN_Set;
}

// 等待信号响应
void I2C_WaitAck(void) // 测数据信号的电平
{
	OLED_SDIN_Set;
	delay_us(1);
	OLED_SCLK_Set;
	delay_us(1);
	OLED_SCLK_Clr;
	delay_us(1);
}

// 写入一个字节
void Send_Byte(u8 dat)
{
	u8 i;
	for (i = 0; i < 8; i++)
	{
		if (dat & 0x80) // 将dat的8位从最高位依次写入
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
		OLED_SCLK_Clr; // 将时钟信号设置为低电平
		dat <<= 1;
	}
}

/// @brief 向SSD1106写入1byte
/// @param dat 写入的数据/命令
/// @param cmd 命令0，数据1
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

/* 设置显示坐标，x y */
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
	OLED_WR_Byte(0xB0 + y, OLED_CMD);				  /* 设置行地址 */
	OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD); /* 设置列高地址 */
	OLED_WR_Byte((x & 0x0F) | 0x01, OLED_CMD);		  /* 设置列低地址 */
}

/* 开启OLED显示 */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); /* 设置电荷泵命令 */
	OLED_WR_Byte(0X14, OLED_CMD); /* 开电荷泵 */
	OLED_WR_Byte(0XAF, OLED_CMD); /* 打开oled显示 */
}

/* 关闭OLED显示 */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); /* 设置电荷泵命令 */
	OLED_WR_Byte(0X10, OLED_CMD); /* 关电荷泵 */
	OLED_WR_Byte(0XAE, OLED_CMD); /* 关闭oled显示 */
}

/* 清屏函数 */
void OLED_Clear(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xB0 + i, OLED_CMD); /* 设置页地址（0~7） */
		OLED_WR_Byte(0x00, OLED_CMD);	  /* 设置显示位置—列低地址 */
		OLED_WR_Byte(0x10, OLED_CMD);	  /* 设置显示位置—列高地址 */
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0x00, OLED_DATA); /* 写入空白数据 */
	}
}

/* 在指定位置显示一个字符,包括部分字符 */
/* x:0~127 */
/* y:0~63 */
/* mode:0,反白显示;1,正常显示 */
/* size:选择字体 16/12 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
	uint8_t c = 0, i = 0;
	c = chr - ' ';			/* 得到偏移后的值 */
	if (x > Max_Column - 1) /* 防止越界 */
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
// m^n函数
u32 oled_pow(uint8_t m, uint8_t n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}
// 显示2个数字
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// mode:模式	0,填充模式;1,叠加模式
// num:数值(0~4294967295);
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

/* 显示一个字符串 */
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

/* 显示汉字 */
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

/* 显示一串汉字 */
void OLED_ShowCnString(uint8_t x, uint8_t y, uint16_t index, uint8_t HowChar)
{
	uint16_t i;
	for (i = 0; i < HowChar; i++)
	{
		OLED_ShowCHinese(x, y, index + i);
		x = (x + 16) % 128;
	}
}

/* 显示整张图片 */
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

/* 显示整张图片 */
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

/* 显示整张图片 */
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

/* 初始化SSD1306 */
void OLED_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); /* 开GPIOA、B时钟 */
	GPIO_StructInit(&GPIO_InitStructure);				  /* 初始化GPIO结构 */
	/* DIN PB10 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  /* 开漏输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /* 速度50MHz */
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  /*  */
	GPIO_SetBits(GPIOB, GPIO_Pin_10);				  /* 拉高 */
	/* CLK PB11 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /* 推挽输出 */
	GPIO_Init(GPIOB, &GPIO_InitStructure);			 /*  */
	GPIO_SetBits(GPIOB, GPIO_Pin_11);				 /* 拉高 */

	/* 复位 */
	delay_ms(200);

	/* 配置oled模式 */
	OLED_WR_Byte(0xAE, OLED_CMD); /* 关闭oled显示进入睡眠模式，开启显示AFH */
	OLED_WR_Byte(0x00, OLED_CMD); /* 设置列低地址，00H-0FH */
	OLED_WR_Byte(0x10, OLED_CMD); /* 设置列高地址，10H-1FH */
	OLED_WR_Byte(0x40, OLED_CMD); /* 设置RAM显示开始线地址 */
	OLED_WR_Byte(0x81, OLED_CMD); /* 设置对比度命令 */
	OLED_WR_Byte(0xCF, OLED_CMD); /* 对比度设置为CFH，共分为256份 */
	OLED_WR_Byte(0xA1, OLED_CMD); /* 0xA0左右反置 0xA1正常 */
	OLED_WR_Byte(0xC8, OLED_CMD); /* 0xC0上下反置 0xC8正常 */
	OLED_WR_Byte(0xA6, OLED_CMD); /* 设置正常显示，A7H反相显示 */
	OLED_WR_Byte(0xA8, OLED_CMD); /* 设置复用比命令 */
	OLED_WR_Byte(0x3F, OLED_CMD); /* 复用比1/64*/
	OLED_WR_Byte(0xD3, OLED_CMD); /* 设置显示偏移量(0x00~0x3F) */
	OLED_WR_Byte(0x00, OLED_CMD); /* 显示不偏移 */
	OLED_WR_Byte(0xD5, OLED_CMD); /* 设置时钟频率和刷新率命令 */
	OLED_WR_Byte(0x80, OLED_CMD); /* 设置为默认值 */
	OLED_WR_Byte(0xD9, OLED_CMD); /* 设置预充电周期命令 */
	OLED_WR_Byte(0xF1, OLED_CMD); /* 1-15个DCLK为一周期 */
	OLED_WR_Byte(0xDA, OLED_CMD); /* 设置硬件引脚配置命令 */
	OLED_WR_Byte(0x12, OLED_CMD); /* 屏蔽COM引脚左右映射 */
	OLED_WR_Byte(0xDB, OLED_CMD); /* 设置vcomh优先级命令 */
	OLED_WR_Byte(0x40, OLED_CMD); /* 设置vcomh优先级 */
	OLED_WR_Byte(0x20, OLED_CMD); /* 设置内存地址模式命令 */
	OLED_WR_Byte(0x02, OLED_CMD); /* 00H水平模式，01H垂直模式，10H页模式 */
	OLED_WR_Byte(0x8D, OLED_CMD); /* 设置电荷泵开关命令 */
	OLED_WR_Byte(0x14, OLED_CMD); /* 电荷泵开，关闭为10H */
	OLED_WR_Byte(0xA4, OLED_CMD); /* 关闭RAM内容显示(0xa4/0xa5) */
	OLED_WR_Byte(0xAF, OLED_CMD); /* 开启oled显示 */

	OLED_Clear();		/* 清屏 */
	OLED_Set_Pos(0, 0); /* 内存指针复位于0,0 */
}
