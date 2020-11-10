#include "spi.h"

void SPI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(SPI_RCC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = SPI_PIN_CS;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PORT, &GPIO_InitStruct); // Init CS
    GPIO_SetBits(SPI_PORT, SPI_PIN_CS);    // CS片选不选中

    GPIO_InitStruct.GPIO_Pin = SPI_PIN_SCK | SPI_PIN_MISO | SPI_PIN_MOSI;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PORT, &GPIO_InitStruct); // Init SCK/MISO/MOSI
    GPIO_SetBits(SPI_PORT, SPI_PIN_SCK | SPI_PIN_MISO | SPI_PIN_MOSI);

    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; // 软件片选
    SPI_Init(SPI2, &SPI_InitStruct);

    SPI_Cmd(SPI2, ENABLE);
}

// SPI读写一字节数据
u8 SPI2_ReadWriteByte(u8 data)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
        ;
    SPI_I2S_SendData(SPI2, data);

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
        ;
    return SPI_I2S_ReceiveData(SPI2);
}
