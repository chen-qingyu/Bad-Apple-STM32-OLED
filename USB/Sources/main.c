#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "malloc.h"

#include "led.h"
#include "w25qxx.h"
#include "sdio_sdcard.h"

#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "memory.h"
#include "usb_bot.h"

extern u8 Max_Lun; //支持的磁盘个数,0表示1个,1表示2个.
int main(void)
{
	u8 offline_cnt = 0;
	u8 tct = 0;
	u8 USB_STA;
	u8 Divece_STA;

	delay_init();		 //延时函数初始化
	uart_init(115200);	 //串口初始化为115200
	my_mem_init(SRAMIN); //初始化内部内存池

	LED_Init();	   //初始化与LED连接的硬件接口
	W25QXX_Init(); //初始化W25Q128

	if (SD_Init())
	{
		Max_Lun = 0; //SD卡错误,则仅只有一个磁盘.
	}
	else //SD 卡正常
	{
		Mass_Memory_Size[1] = SDCardInfo.CardCapacity; //得到SD卡容量（字节），当SD卡容量超过4G的时候,需要用到两个u32来表示
		Mass_Block_Size[1] = 512;					   //因为我们在Init里面设置了SD卡的操作字节为512个,所以这里一定是512个字节.
		Mass_Block_Count[1] = Mass_Memory_Size[1] / Mass_Block_Size[1];
	}
	if (W25QXX_TYPE != W25Q128)
	{
	}
	else //SPI FLASH 正常
	{
		Mass_Memory_Size[0] = 1024 * 1024 * 12; //前12M字节
		Mass_Block_Size[0] = 512;				//设置SPI FLASH的操作扇区大小为512
		Mass_Block_Count[0] = Mass_Memory_Size[0] / Mass_Block_Size[0];
	}
	delay_ms(1800);
	USB_Port_Set(0); //USB先断开
	delay_ms(700);
	USB_Port_Set(1);											  //USB再次连接
	Data_Buffer = mymalloc(SRAMIN, BULK_MAX_PACKET_SIZE * 2 * 4); //为USB数据缓存区申请内存
	Bulk_Data_Buff = mymalloc(SRAMIN, BULK_MAX_PACKET_SIZE);	  //申请内存
	//USB配置
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	delay_ms(1800);
	while (1)
	{
		delay_ms(1);
		if (USB_STA != USB_STATUS_REG) //状态改变了
		{
			USB_STA = USB_STATUS_REG; //记录最后的状态
		}
		if (Divece_STA != bDeviceState)
		{
			Divece_STA = bDeviceState;
		}
		tct++;
		if (tct == 200)
		{
			tct = 0;
			LED0 = !LED0; //提示系统在运行
			if (USB_STATUS_REG & 0x10)
			{
				offline_cnt = 0; //USB连接了,则清除offline计数器
				bDeviceState = CONFIGURED;
			}
			else //没有得到轮询
			{
				offline_cnt++;
				if (offline_cnt > 10)
					bDeviceState = UNCONNECTED; //2s内没收到在线标记,代表USB被拔出了
			}
			USB_STATUS_REG = 0;
		}
	}
}
