#ifndef _SOIL_H_
#define _SOIL_H_
#include "sys.h"

void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times);

#endif