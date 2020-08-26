#include "stm32f10x.h"

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "malloc.h"

#include "spi.h"
#include "w25qxx.h"

#include "ff.h"
#include "exfuns.h"

int main(void)
{
    u8 res = 0;
    FIL fp;
    u8 readData[1024];
    UINT br;

    delay_init();
    uart_init(115200);
    my_mem_init(SRAMIN);
    exfuns_init();

    W25QXX_Init();

    res = f_mount(fs[1], "1:", 1); // 挂载FLASH。已配置好选项，如果FLASH没有文件系统则挂载时会先自动格式化
    printf("Mount res = %d.\r\n", res);

    res = f_open(&fp, "1:BadApple.bin", FA_READ); // 打开文件
    printf("Open res = %d.\r\n", res);

    res = f_read(&fp, readData, 1, &br); // 读取一个字节（串口打印地址）
    printf("Read res = %d.\r\n", res);

    f_close(&fp);

    while (1)
    {
    }
}
