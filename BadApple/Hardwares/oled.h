#ifndef OLED_H
#define OLED_H

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"

// OLED(SSD1306) 驱动代码
// 驱动方式: SPI

#define OLED_CS  PDout(3)  // 片选（低电平有效）
#define OLED_RST PDout(4)  // 复位（低电平有效）
#define OLED_DC  PDout(5)  // 数据/命令
#define OLED_SCL PDout(6)  // CLK时钟（D0）
#define OLED_SDA PDout(7)  // MOSI数据（D1）

#define OLED_RCC     RCC_APB2Periph_GPIOD
#define OLED_PORT    GPIOD
#define OLED_PIN_CS  GPIO_Pin_3
#define OLED_PIN_RST GPIO_Pin_4
#define OLED_PIN_DC  GPIO_Pin_5
#define OLED_PIN_SCL GPIO_Pin_6
#define OLED_PIN_SDA GPIO_Pin_7

#define OLED_CMD  0
#define OLED_DATA 1

void OLED_Init(void);
void OLED_WriteByte(u8 dat, u8 cmd);
void OLED_RefreshRAM(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x, u8 y, u8 mode);
void OLED_ShowChar(u8 x, u8 y, u8 chr);
void OLED_ShowString(u8 x, u8 y, const u8 *str);
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len);
void OLED_DrawImage(u8 img[]);
void OLED_Reverse(void);

#endif
