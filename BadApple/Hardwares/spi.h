#ifndef SPI_H
#define SPI_H

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"

#define SPI_RCC      RCC_APB1Periph_SPI2
#define SPI_PORT     GPIOB
#define SPI_PIN_CS   GPIO_Pin_12
#define SPI_PIN_SCK  GPIO_Pin_13
#define SPI_PIN_MISO GPIO_Pin_14
#define SPI_PIN_MOSI GPIO_Pin_15

void SPI2_Init(void);
u8 SPI2_ReadWriteByte(u8 data);

#endif
