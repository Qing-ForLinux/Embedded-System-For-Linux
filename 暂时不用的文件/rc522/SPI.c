/*
 * 文 件 名：SPI.c
 * 芯    片：STC12C5A60S2
 * 晶    振：12MHz
 * 创 建 者：冷月
 * 创建日期：2010.10.15
 * 修 改 者：
 * 修改日期：
 * 功能描述：SPI总线读写
 */

#include "SPI.h"
#include "common.h"


/*
 * 函 数 名：SPI_Init
 * 功能描述：SPI初始化
 * 输入参数：无
 * 返 回 值：无
 */
void SPI_Init(void)
{
		/* 初始化GPIO */
	s3c2410_gpio_cfgpin(RC522_SDA, S3C2410_GPIO_OUTPUT);	/* 设置为输出 */
	s3c2410_gpio_cfgpin(RC522_SCK, S3C2410_GPIO_OUTPUT);	/* 设置为输出 */
	s3c2410_gpio_cfgpin(RC522_MISO, S3C2410_GPIO_INPUT);	/* 设置为输入 */
	s3c2410_gpio_cfgpin(RC522_MOSI, S3C2410_GPIO_OUTPUT);	/* 设置为输出 */
	s3c2410_gpio_cfgpin(RC522_RST, S3C2410_GPIO_OUTPUT);	/* 设置为输出 */
	
	s3c2410_gpio_setpin(RC522_SCK, 0);
	s3c2410_gpio_setpin(RC522_SDA, 1);
	
}


/*
 * 函 数 名：SPI_Read
 * 功能描述：读一个字节到单片机
 * 输入参数：无
 * 返 回 值：recv：接收到一个字节数据
 */
uint8_t SPI_Read(void)
{
	uint8_t i;
	uint8_t recv = 0x00;

	for (i=0; i<8; i++)
	{	
		recv <<= 1;
		s3c2410_gpio_setpin(RC522_SCK, 0);
		if (s3c2410_gpio_getpin(RC522_MISO))
		{
			recv |= 0x01;
		}
		s3c2410_gpio_setpin(RC522_SCK, 1);			//上升沿读一位
	}
	s3c2410_gpio_setpin(RC522_SCK, 0);

	return recv;
}


/*
 * 函 数 名：SPI_Write
 * 功能描述：从单片机输出一个字节数据
 * 输入参数：val：要输出的数据
 * 返 回 值：无
 */
void SPI_Write(uint8_t val)
{
	uint8_t i;

	for (i=0; i<8; i++)
	{
		if (val & 0x80)			//从高位开始输出
		{
			s3c2410_gpio_setpin(RC522_MOSI, 1);
		}
		else
		{
			s3c2410_gpio_setpin(RC522_MOSI, 0);
		}
		s3c2410_gpio_setpin(RC522_SCK, 1);
		val <<= 1;
		s3c2410_gpio_setpin(RC522_SCK, 0);;			   //下降沿写一位
	}
}