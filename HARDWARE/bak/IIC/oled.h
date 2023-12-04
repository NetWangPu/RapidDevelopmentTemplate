/*        GND    电源地 */
/*        VCC  5V或3.3v电源 */
/*        D0   PA5（SCL） */
/*        D1   PA7（SDA） */
/*        RES  PA6 */
/*        DC   PB0 */
/*        CS   PA4 */
/*******************************************************************************/
#ifndef OLED_H
#define OLED_H

#include "stm32f10x.h"

#define SIZE 16
#define XLevelL 0x00
#define XLevelH 0x10
#define Max_Column 128
#define Max_Row 64
#define Brightness 0xFF
#define X_WIDTH 128
#define Y_WIDTH 64

/*-----------------OLED端口定义---------------- */
#define OLED_SCLK_Clr GPIOB->BRR = GPIO_Pin_11 /* CLK */
#define OLED_SCLK_Set GPIOB->BSRR = GPIO_Pin_11

#define OLED_SDIN_Clr GPIOB->BRR = GPIO_Pin_10 /* DIN */
#define OLED_SDIN_Set GPIOB->BSRR = GPIO_Pin_10

#define OLED_CMD 0  /* 写命令 */
#define OLED_DATA 1 /* 写数据 */

/* OLED控制用函数 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
void OLED_ShowNum(uint8_t x, uint8_t y, u32 num, uint8_t len, uint8_t size);
void OLED_ShowString(uint8_t x, uint8_t y, char *p);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no);
void OLED_ShowCnString(uint8_t x, uint8_t y, uint16_t index, uint8_t HowChar);
// void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void OLED_DrawLight(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void OLED_DrawSet(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void OLED_DrawFmq(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

#endif /* OLED_H */
