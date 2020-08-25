#include "led.h"

void LED_Init()
{
    /*Library*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SetBits(GPIOB, GPIO_Pin_5);

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    GPIO_SetBits(GPIOE, GPIO_Pin_5);

    // /*Register*/
    // RCC->APB2ENR |= 1 << 3;
    // RCC->APB2ENR |= 1 << 6;

    // GPIOB->CRL &= 0XFF0FFFFF;
    // GPIOB->CRL |= 0X00300000;
    // GPIOB->ODR |= 1 << 5;

    // GPIOE->CRL &= 0XFF0FFFFF;
    // GPIOE->CRL |= 0X00300000;
    // GPIOE->ODR |= 1 << 5;
}
