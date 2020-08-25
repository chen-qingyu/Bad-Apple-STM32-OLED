#include "stm32f10x.h"

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "malloc.h"

#include "spi.h"
#include "w25qxx.h"
#include "oled.h"

#include "ff.h"
#include "exfuns.h"

int main(void)
{
    FIL fp;
    UINT br;
    
    u8 readBuff[1024];
    u32 readAddr = 4976640; // 通过FATFS得到
    u16 numOfImages = 9480;

    delay_init();
    uart_init(115200);
    my_mem_init(SRAMIN);
    exfuns_init();

    OLED_Init();
    W25QXX_Init();

    f_mount(fs[1], "1:", 1); // 挂载FLASH.
    f_open(&fp, "1:BadApple.bin", FA_READ); // 打开文件

    for (u16 i = 0; i < numOfImages; i++)
    {
        f_read(&fp, readBuff, 1024, &br);
        OLED_DrawImage(readBuff);
        readAddr += 1024;
        delay_ms(10); // 控制帧率
    }
    
    f_close(&fp);
    
    delay_ms(100);
    OLED_Reverse();
    OLED_ShowString(0, 0, "THANK YOU");
    
    while (1)
    {
    }
}
