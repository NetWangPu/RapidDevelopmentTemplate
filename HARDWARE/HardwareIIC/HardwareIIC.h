#ifndef HARDWAREIIC_H
#define HARDWAREIIC_H
#include "sys.h"
#include "stm32f10x.h"

#define I2Cx I2C1		 

void Hardware_I2C_Init(void);
void I2C_WriteByte(uint8_t addr, uint8_t data, uint8_t slave_address);
uint8_t I2C_ReadByte(uint8_t addr, uint8_t slave_address);


#endif /* HARDWAREIIC_H */
