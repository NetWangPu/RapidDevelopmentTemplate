#ifndef __OLED_H
#define __OLED_H

#include "stdlib.h" 
#include "stdint.h"


typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef  uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

//汉字大小，英文数字大小
#define 	TYPE8X16		1
#define 	TYPE16X16		2
#define 	TYPE6X8			3

void LCD_WrCmd(uint8_t cmd);
void LCD_WrDat(uint8_t data);

void LCD_Init(void);
void SCL(void);
void LCD_CLS(void);
void LCD_CLS_y(char y);
void LCD_CLS_line_area(u8 start_x,u8 start_y,u8 width);
void LCD_P6x8Str(u8 x,u8 y,u8 *ch,const u8 *F6x8);
void LCD_P8x16Str(u8 x,u8 y,u8 *ch,const u8 *F8x16);
void LCD_P14x16Str(u8 x,u8 y,u8 ch[],const u8 *F14x16_Idx,const u8 *F14x16);
void LCD_P16x16Str(u8 x,u8 y,u8 *ch,const u8 *F16x16_Idx,const u8 *F16x16);
//extern void LCD_Print(u8 x, u8 y, u8 *ch);
void LCD_PutPixel(u8 x,u8 y);
void LCD_Print(u8 x, u8 y, u8 *ch,u8 char_size, u8 ascii_size);
void LCD_Rectangle(u8 x1,u8 y1,u8 x2,u8 y2,u8 gif);
void Draw_BMP(u8 x,u8 y,const u8 *bmp); 
void LCD_Fill(u8 dat);
#endif






