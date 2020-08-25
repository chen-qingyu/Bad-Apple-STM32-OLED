#ifndef SPI_H
#define SPI_H

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"

void SPI2_Init(void);
u8 SPI2_ReadWriteByte(u8 data);

#endif
