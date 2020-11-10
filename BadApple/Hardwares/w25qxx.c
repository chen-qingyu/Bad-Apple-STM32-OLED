#include "w25qxx.h"

u16 W25QXX_TYPE = W25Q128; // 默认型号是W25Q128

// SPI在一页(0~65535)内写入少于256个字节的数据
// 在指定地址开始写入最大256字节的数据
// buffer: 数据存储区
// addr: 开始写入的地址(24bit)
// len: 要写入的字节数(最大256),该数不应该超过该页的剩余字节数
static void W25QXX_Write_Page(u8 *buffer, u32 addr, u16 len)
{
    W25QXX_WriteEnable();
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_PageProgram);
    SPI2_ReadWriteByte((u8)((addr) >> 16));
    SPI2_ReadWriteByte((u8)((addr) >> 8));
    SPI2_ReadWriteByte((u8)addr);
    for (int i = 0; i < len; i++)
    {
        SPI2_ReadWriteByte(buffer[i]);
    }
    W25QXX_CS = 1;
    W25QXX_WaitBusy(); //等待写入结束
}

// 无检验写数据
// 必须确保所写的地址范围内的数据全部为0XFF
// 具有自动换页功能
// 在指定地址开始写入指定长度的数据,但是要确保地址不越界
// buffer:数据存储区
// addr:开始写入的地址(24bit)
// len:要写入的字节数(最大65535)
static void W25QXX_Write_NoCheck(u8 *buffer, u32 addr, u16 len)
{
    u16 remainder = 256 - addr % 256; // 单页剩余的字节数
    if (len <= remainder)
    {
        remainder = len; // 不大于256个字节
    }
    while (1)
    {
        W25QXX_Write_Page(buffer, addr, remainder);
        if (len == remainder)
        {
            break; //写入结束了
        }
        else //len>remainder
        {
            buffer += remainder;
            addr += remainder;

            len -= remainder; //减去已经写入了的字节数
            if (len > 256)
            {
                remainder = 256;
            } //一次可以写入256个字节
            else
            {
                remainder = len;
            } //不够256个字节了
        }
    }
}

void W25QXX_Init(void)
{
    SPI2_Init();

    W25QXX_TYPE = W25QXX_ReadType(); // 读取芯片型号
}

// 读取W25QXX的状态寄存器
// BIT 7    6    5    4    3    2    1    0
//     SPR  RV   TB   BP2  BP1  BP0  WEL  BUSY
// SPR: 默认0,状态寄存器保护位,配合WP使用
// TB,BP2,BP1,BP0: FLASH区域写保护设置
// WEL: 写使能锁定
// BUSY: 忙标记位(1:忙; 0:空闲)
// 默认: 0x00
u8 W25QXX_ReadSR(void)
{
    u8 byte = 0;
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_ReadStatusReg);
    byte = SPI2_ReadWriteByte(0Xff);
    W25QXX_CS = 1;
    return byte;
}

// 写W25QXX状态寄存器
// 只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写
void W25QXX_WriteSR(u8 sr)
{
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_WriteStatusReg);
    SPI2_ReadWriteByte(sr);
    W25QXX_CS = 1;
}

// W25QXX写使能
void W25QXX_WriteEnable(void)
{
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_WriteEnable);
    W25QXX_CS = 1;
}

// W25QXX写禁止
void W25QXX_WriteDisable(void)
{
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_WriteDisable);
    W25QXX_CS = 1;
}

// 读取芯片型号
u16 W25QXX_ReadType(void)
{
    u16 type = 0;
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(0x90);
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);
    type |= SPI2_ReadWriteByte(0xFF) << 8;
    type |= SPI2_ReadWriteByte(0xFF);
    W25QXX_CS = 1;
    return type;
}

// 读取数据
// 在指定地址开始读取指定长度的数据
// buffer: 数据内容
// addr: 开始读取的地址(24bit)
// len: 要读取的字节数(最大65535)
void W25QXX_Read(u8 *buffer, u32 addr, u16 len)
{
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_ReadData);
    SPI2_ReadWriteByte((u8)((addr) >> 16));
    SPI2_ReadWriteByte((u8)((addr) >> 8));
    SPI2_ReadWriteByte((u8)addr);
    for (int i = 0; i < len; i++)
    {
        buffer[i] = SPI2_ReadWriteByte(0XFF);
    }
    W25QXX_CS = 1;
}

// 写入数据
// 在指定地址开始写入指定长度的数据
// 该函数带擦除操作
// buffer:数据存储区
// addr:开始写入的地址(24bit)
// len:要写入的字节数(最大65535)
void W25QXX_Write(u8 *buffer, u32 addr, u16 len)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    static u8 databuf[4096];
    secpos = addr / 4096;      // 扇区地址
    secoff = addr % 4096;      // 在扇区内的偏移
    secremain = 4096 - secoff; // 扇区剩余空间大小

    if (len <= secremain)
    {
        secremain = len; // 不大于4096个字节
    }
    while (1)
    {
        W25QXX_Read(databuf, secpos * 4096, 4096); // 读出整个扇区的内容
        for (i = 0; i < secremain; i++)            // 校验数据
        {
            if (databuf[secoff + i] != 0XFF)
            {
                break;
            } // 需要擦除
        }
        if (i < secremain) // 需要擦除
        {
            W25QXX_EraseSector(secpos);     // 擦除这个扇区
            for (i = 0; i < secremain; i++) // 复制
            {
                databuf[i + secoff] = buffer[i];
            }
            W25QXX_Write_NoCheck(databuf, secpos * 4096, 4096); // 写入整个扇区
        }
        else
        {
            W25QXX_Write_NoCheck(buffer, addr, secremain); // 写已经擦除了的,直接写入扇区剩余区间.
        }
        if (len == secremain)
        {
            break; // 写入结束了
        }
        else // 写入未结束
        {
            secpos++;   // 扇区地址增1
            secoff = 0; // 偏移位置为0

            buffer += secremain; // 指针偏移
            addr += secremain;   // 写地址偏移
            len -= secremain;    // 字节数递减
            if (len > 4096)
            {
                secremain = 4096;
            } // 下一个扇区还是写不完
            else
            {
                secremain = len;
            } // 下一个扇区可以写完了
        }
    }
}

// 擦除整个芯片
// 等待时间超长...
void W25QXX_EraseChip(void)
{
    W25QXX_WriteEnable();
    W25QXX_WaitBusy();
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_ChipErase);
    W25QXX_CS = 1;
    W25QXX_WaitBusy();
}

// 擦除一个扇区
// addr:扇区地址, 根据实际容量设置
// 擦除一个扇区的最少时间:150ms
void W25QXX_EraseSector(u32 addr)
{
    addr *= 4096;
    W25QXX_WriteEnable();
    W25QXX_WaitBusy();
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_SectorErase);
    SPI2_ReadWriteByte((u8)((addr) >> 16));
    SPI2_ReadWriteByte((u8)((addr) >> 8));
    SPI2_ReadWriteByte((u8)addr);
    W25QXX_CS = 1;
    W25QXX_WaitBusy();
}

// 等待空闲
void W25QXX_WaitBusy(void)
{
    while ((W25QXX_ReadSR() & 0x01) == 0x01)
        ;
}

// 进入掉电模式
void W25QXX_PowerDown(void)
{
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_PowerDown);
    W25QXX_CS = 1;
    delay_us(3);
}

// 唤醒
void W25QXX_WakeUp(void)
{
    W25QXX_CS = 0;
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);
    W25QXX_CS = 1;
    delay_us(3);
}
