/*
 * 文 件 名：SPI.h
 * 芯    片：STC12C5A60S2
 * 晶    振：12MHz
 * 创 建 者：冷月
 * 创建日期：2010.10.15
 * 修 改 者：
 * 修改日期：
 * 功能描述：SPI总线读写
 */

#ifndef	SPI_H_
#define	SPI_H_

#include "common.h"


/*
 * 函 数 名：SPI_Init
 * 调    用：SPI_Init();
 * 功能描述：SPI初始化
 */
void SPI_Init(void);


/*
 * 函 数 名：SPI_Read
 * 调    用：val = SPI_Read();
 * 功能描述：从SPI设备读回一个字节
 */
uint8_t SPI_Read(void);


/*
 * 函 数 名：SPI_Write
 * 调    用：SPI_Write(val);
 * 功能描述：向SPI设备写一个字节
 */
void SPI_Write(uint8_t val);

#endif