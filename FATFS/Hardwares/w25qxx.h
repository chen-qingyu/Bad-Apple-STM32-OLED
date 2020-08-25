#ifndef W25QXX_H
#define W25QXX_H

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "spi.h"

// W25Q128容量说明：
// 最小擦除单位为一个Sector(4KB)
// 16个Sector为一个Block(64KB)
// W25Q128容量为128Mb = 16MB，共有256个Block，4096个Sector

// W25QXX系列芯片型号
#define W25Q80 0XEF13
#define W25Q16 0XEF14
#define W25Q32 0XEF15
#define W25Q64 0XEF16
#define W25Q128 0XEF17

#define W25QXX_CS PBout(12) // W25QXX的片选信号

// W25QXX指令表
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F

void W25QXX_Init(void);                           // 初始化
u16 W25QXX_ReadType(void);                        // 读取W25QXX芯片型号
u8 W25QXX_ReadSR(void);                           // 读取状态寄存器
void W25QXX_WriteSR(u8 sr);                       // 写状态寄存器
void W25QXX_WriteEnable(void);                    // 写使能
void W25QXX_WriteDisable(void);                   // 写禁止
void W25QXX_Read(u8 *buffer, u32 addr, u16 len);  // 读取数据
void W25QXX_Write(u8 *buffer, u32 addr, u16 len); // 写入数据
void W25QXX_EraseChip(void);                      // 整片擦除
void W25QXX_EraseSector(u32 addr);                // 扇区擦除
void W25QXX_WaitBusy(void);                       // 等待空闲
void W25QXX_PowerDown(void);                      // 进入掉电模式
void W25QXX_WakeUp(void);                         // 唤醒

#endif
