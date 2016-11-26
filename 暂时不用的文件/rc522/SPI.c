/*
 * �� �� ����SPI.c
 * о    Ƭ��STC12C5A60S2
 * ��    ��12MHz
 * �� �� �ߣ�����
 * �������ڣ�2010.10.15
 * �� �� �ߣ�
 * �޸����ڣ�
 * ����������SPI���߶�д
 */

#include "SPI.h"
#include "common.h"


/*
 * �� �� ����SPI_Init
 * ����������SPI��ʼ��
 * �����������
 * �� �� ֵ����
 */
void SPI_Init(void)
{
		/* ��ʼ��GPIO */
	s3c2410_gpio_cfgpin(RC522_SDA, S3C2410_GPIO_OUTPUT);	/* ����Ϊ��� */
	s3c2410_gpio_cfgpin(RC522_SCK, S3C2410_GPIO_OUTPUT);	/* ����Ϊ��� */
	s3c2410_gpio_cfgpin(RC522_MISO, S3C2410_GPIO_INPUT);	/* ����Ϊ���� */
	s3c2410_gpio_cfgpin(RC522_MOSI, S3C2410_GPIO_OUTPUT);	/* ����Ϊ��� */
	s3c2410_gpio_cfgpin(RC522_RST, S3C2410_GPIO_OUTPUT);	/* ����Ϊ��� */
	
	s3c2410_gpio_setpin(RC522_SCK, 0);
	s3c2410_gpio_setpin(RC522_SDA, 1);
	
}


/*
 * �� �� ����SPI_Read
 * ������������һ���ֽڵ���Ƭ��
 * �����������
 * �� �� ֵ��recv�����յ�һ���ֽ�����
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
		s3c2410_gpio_setpin(RC522_SCK, 1);			//�����ض�һλ
	}
	s3c2410_gpio_setpin(RC522_SCK, 0);

	return recv;
}


/*
 * �� �� ����SPI_Write
 * �����������ӵ�Ƭ�����һ���ֽ�����
 * ���������val��Ҫ���������
 * �� �� ֵ����
 */
void SPI_Write(uint8_t val)
{
	uint8_t i;

	for (i=0; i<8; i++)
	{
		if (val & 0x80)			//�Ӹ�λ��ʼ���
		{
			s3c2410_gpio_setpin(RC522_MOSI, 1);
		}
		else
		{
			s3c2410_gpio_setpin(RC522_MOSI, 0);
		}
		s3c2410_gpio_setpin(RC522_SCK, 1);
		val <<= 1;
		s3c2410_gpio_setpin(RC522_SCK, 0);;			   //�½���дһλ
	}
}