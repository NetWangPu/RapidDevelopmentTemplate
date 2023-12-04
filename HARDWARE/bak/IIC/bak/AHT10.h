#ifndef __AHT10_H
#define __AHT10_H
#include "sys.h"
void AHT10_Init(void);
uint8_t ReadAHT10(float *Hum,float *Temp);

#endif
