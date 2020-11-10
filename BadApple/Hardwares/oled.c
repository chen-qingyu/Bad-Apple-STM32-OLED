#include "oled.h"
#include "oledfont.h"

u8 OLED_GRAM[128][8]; // 1 KB

void OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(OLED_RCC, ENABLE);

    // 配置四个引脚都为推挽输出
    GPIO_InitStructure.GPIO_Pin = OLED_PIN_CS | OLED_PIN_SCL | OLED_PIN_SDA | OLED_PIN_RST | OLED_PIN_DC;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_PORT, &GPIO_InitStructure);
    GPIO_SetBits(OLED_PORT, OLED_PIN_CS | OLED_PIN_SCL | OLED_PIN_SDA | OLED_PIN_RST | OLED_PIN_DC);

    // 复位
    OLED_RST = 0;
    delay_ms(100);
    OLED_RST = 1;

    // 以下配置参考SSD1306芯片手册
    OLED_WriteByte(0xAE, OLED_CMD); // 关闭显示
    OLED_WriteByte(0xD5, OLED_CMD); // 设置时钟分频,震荡频率
    OLED_WriteByte(0x80, OLED_CMD); // [3:0];[7:4]
    OLED_WriteByte(0xA8, OLED_CMD); // 设置驱动路数
    OLED_WriteByte(0x3F, OLED_CMD); // 默认0x3F(1/64)
    OLED_WriteByte(0xD3, OLED_CMD); // 设置显示偏移
    OLED_WriteByte(0x00, OLED_CMD); // 默认0
    OLED_WriteByte(0x40, OLED_CMD); // 设置显示开始行
    OLED_WriteByte(0x8D, OLED_CMD); // 设置电荷泵
    OLED_WriteByte(0x14, OLED_CMD); // [1:1]
    OLED_WriteByte(0x20, OLED_CMD); // 设置内存地址模式
    OLED_WriteByte(0x02, OLED_CMD); // [1:0]
    OLED_WriteByte(0xA1, OLED_CMD); // 设置段重定义
    OLED_WriteByte(0xC0, OLED_CMD); // 设置COM扫描方向(0xC8/0xC0上下翻转！)
    OLED_WriteByte(0xDA, OLED_CMD); // 设置COM硬件引脚
    OLED_WriteByte(0x12, OLED_CMD); // [5:4]
    OLED_WriteByte(0x81, OLED_CMD); // 设置对比度
    OLED_WriteByte(0xEF, OLED_CMD); // 1~255;越大越亮
    OLED_WriteByte(0xD9, OLED_CMD); // 设置预充电周期
    OLED_WriteByte(0xf1, OLED_CMD); // [3:0]
    OLED_WriteByte(0xDB, OLED_CMD); // 设置电压倍率
    OLED_WriteByte(0x30, OLED_CMD); // [6:4]
    OLED_WriteByte(0xA4, OLED_CMD); // 全局显示开启
    OLED_WriteByte(0xA6, OLED_CMD); // 设置显示方式
    OLED_WriteByte(0xAF, OLED_CMD); // 开启显示

    OLED_Clear();
}

void OLED_WriteByte(u8 dat, u8 dc)
{
    OLED_CS = 0;
    OLED_DC = dc; // 0->CMD; 1->DATA
    for (u8 i = 0; i < 8; i++)
    {
        OLED_SCL = 0;
        if (dat & 0x80)
        {
            OLED_SDA = 1;
        }
        else
        {
            OLED_SDA = 0;
        }
        OLED_SCL = 1; // 上升沿传输
        dat <<= 1;
    }
    OLED_DC = 1;
    OLED_CS = 1;
}

void OLED_RefreshRAM(void)
{
    for (u8 i = 0; i < 8; i++)
    {
        OLED_WriteByte(0xb0 + i, OLED_CMD);
        OLED_WriteByte(0x00, OLED_CMD);
        OLED_WriteByte(0x10, OLED_CMD);
        for (u8 j = 0; j < 128; j++)
        {
            OLED_WriteByte(OLED_GRAM[j][i], OLED_DATA);
        }
    }
}

void OLED_Clear(void)
{
    for (u8 i = 0; i < 8; i++)
    {
        for (u8 n = 0; n < 128; n++)
        {
            OLED_GRAM[n][i] = 0x00;
        }
    }
    OLED_RefreshRAM();
}

void OLED_DrawPoint(u8 x, u8 y, u8 mode)
{
    u8 pos, bx, temp;
    if (x > 127 || y > 63)
    {
        return;
    }
    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);
    if (mode) // mode==1 填充; mode==0 清空
    {
        OLED_GRAM[x][pos] |= temp;
    }
    else
    {
        OLED_GRAM[x][pos] &= ~temp;
    }
}

void OLED_ShowChar(u8 x, u8 y, u8 chr)
{
    u8 temp;
    u8 y0 = y;
    u8 size = 16; // 16 x 8 font
    chr -= ' ';   // 偏移量 32
    for (u8 i = 0; i < size; i++)
    {
        temp = ASCII_1608[chr][i];
        for (u8 j = 0; j < 8; j++)
        {
            if (temp & 0x80)
            {
                OLED_DrawPoint(x, y, 1);
            }
            else
            {
                OLED_DrawPoint(x, y, 0);
            }
            temp <<= 1;
            y++;
            if (y - y0 == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

void OLED_ShowString(u8 x, u8 y, const u8 *str)
{
    u8 size = 16; // 16 x 8 font
    while ((*str <= '~') && (*str >= ' '))
    {
        if (x > (128 - (size / 2)))
        {
            x = 0;
            y += size; // 这行满了就下一行
        }
        if (y > (64 - size))
        {
            y = 0;
            x = 0;
            OLED_Clear();
        }
        OLED_ShowChar(x, y, *str);
        x += size / 2;
        str++;
    }
    OLED_RefreshRAM();
}

// 填充一个矩形区域
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode)
{
    for (u8 x = x1; x <= x2; x++)
    {
        for (u8 y = y1; y <= y2; y++)
        {
            OLED_DrawPoint(x, y, mode);
        }
    }
    OLED_RefreshRAM();
}

u32 Pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)
    {
        result *= m;
    }
    return result;
}

void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len)
{
    u8 temp;
    u8 enshow = 0;
    u8 size = 16;
    for (u8 t = 0; t < len; t++)
    {
        temp = (num / Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size / 2) * t, y, ' ');
                continue;
            }
            else
            {
                enshow = 1;
            }
        }
        OLED_ShowChar(x + (size / 2) * t, y, temp + '0');
    }
    OLED_RefreshRAM();
}

void OLED_DrawImage(u8 img[])
{
    u16 i = 0;
    for (u8 y = 0; y < 8; y++)
    {
        for (u8 x = 0; x < 128; x++)
        {
            OLED_GRAM[x][y] = img[i];
            i++;
        }
    }
    OLED_RefreshRAM();
}

void OLED_Reverse(void)
{
    OLED_WriteByte(0xAE, OLED_CMD); // 关闭显示
    OLED_WriteByte(0xD5, OLED_CMD); // 设置时钟分频,震荡频率
    OLED_WriteByte(0x80, OLED_CMD); // [3:0];[7:4]
    OLED_WriteByte(0xA8, OLED_CMD); // 设置驱动路数
    OLED_WriteByte(0x3F, OLED_CMD); // 默认0x3F(1/64)
    OLED_WriteByte(0xD3, OLED_CMD); // 设置显示偏移
    OLED_WriteByte(0x00, OLED_CMD); // 默认0
    OLED_WriteByte(0x40, OLED_CMD); // 设置显示开始行
    OLED_WriteByte(0x8D, OLED_CMD); // 设置电荷泵
    OLED_WriteByte(0x14, OLED_CMD); // [1:1]
    OLED_WriteByte(0x20, OLED_CMD); // 设置内存地址模式
    OLED_WriteByte(0x02, OLED_CMD); // [1:0]
    OLED_WriteByte(0xA1, OLED_CMD); // 设置段重定义
    OLED_WriteByte(0xC8, OLED_CMD); // 设置COM扫描方向(0xC8/0xC0上下翻转！)
    OLED_WriteByte(0xDA, OLED_CMD); // 设置COM硬件引脚
    OLED_WriteByte(0x12, OLED_CMD); // [5:4]
    OLED_WriteByte(0x81, OLED_CMD); // 设置对比度
    OLED_WriteByte(0xEF, OLED_CMD); // 1~255;越大越亮
    OLED_WriteByte(0xD9, OLED_CMD); // 设置预充电周期
    OLED_WriteByte(0xf1, OLED_CMD); // [3:0]
    OLED_WriteByte(0xDB, OLED_CMD); // 设置电压倍率
    OLED_WriteByte(0x30, OLED_CMD); // [6:4]
    OLED_WriteByte(0xA4, OLED_CMD); // 全局显示开启
    OLED_WriteByte(0xA6, OLED_CMD); // 设置显示方式
    OLED_WriteByte(0xAF, OLED_CMD); // 开启显示

    OLED_Clear();
}
