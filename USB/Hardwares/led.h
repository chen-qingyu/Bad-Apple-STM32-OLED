#ifndef LED_H
#define LED_H

#include "stm32f10x.h"

#define LED0  PBout(5)
#define LED1  PEout(5)

void LED_Init(void);

#endif
