/*
 * �� �� ����SPI.h
 * о    Ƭ��STC12C5A60S2
 * ��    ��12MHz
 * �� �� �ߣ�����
 * �������ڣ�2010.10.15
 * �� �� �ߣ�
 * �޸����ڣ�
 * ����������SPI���߶�д
 */

#ifndef	SPI_H_
#define	SPI_H_

#include "common.h"


/*
 * �� �� ����SPI_Init
 * ��    �ã�SPI_Init();
 * ����������SPI��ʼ��
 */
void SPI_Init(void);


/*
 * �� �� ����SPI_Read
 * ��    �ã�val = SPI_Read();
 * ������������SPI�豸����һ���ֽ�
 */
uint8_t SPI_Read(void);


/*
 * �� �� ����SPI_Write
 * ��    �ã�SPI_Write(val);
 * ������������SPI�豸дһ���ֽ�
 */
void SPI_Write(uint8_t val);

#endif