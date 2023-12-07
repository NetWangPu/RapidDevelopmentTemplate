#ifndef _LQOLED_H
#define _LQOLED_H
#include "sys.h"
#include "stdlib.h"

//汉字大小，英文数字大小
#define 	TYPE8X16		1
#define 	TYPE16X16		2
#define 	TYPE6X8			3

void OLED_Init(void);
extern void SCL(void);
extern void OLED_CLS(void);
extern void OLED_CLS_y(char y);
extern void OLED_CLS_line_area(u8 start_x,u8 start_y,u8 width);
extern void OLED_P6x8Str(u8 x,u8 y,u8 *ch,const u8 *F6x8);
extern void OLED_P8x16Str(u8 x,u8 y,u8 *ch,const u8 *F8x16);
extern void OLED_P14x16Str(u8 x,u8 y,u8 ch[],const u8 *F14x16_Idx,const u8 *F14x16);
extern void OLED_P16x16Str(u8 x,u8 y,u8 *ch,const u8 *F16x16_Idx,const u8 *F16x16);
extern void OLED_PutPixel(u8 x,u8 y);
extern void OLED_Print(u8 x, u8 y, u8 *ch,u8 char_size, u8 ascii_size);
extern void OLED_Rectangle(u8 x1,u8 y1,u8 x2,u8 y2,u8 gif);
extern void Draw_BMP(u8 x,u8 y,const u8 *bmp); 
extern void OLED_Fill(u8 dat);
#endif

