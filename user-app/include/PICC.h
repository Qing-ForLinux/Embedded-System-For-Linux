/******************** (C) COPYRIGHT 2013 ********************
* �ļ���          : PICC.h
* ����            : ������ӿ����Ŷ�
* �汾            : V2.0.1.0
* ����            : 2013-5-10
* ����            : �ļ� PICC.c ��ͷ�ļ�
* ����֧��        : QQ: 526181679
*********************************************************************************/

#ifndef _PICC_H
#define _PICC_H




/***********************************************************************************************
*
***********************************************�궨��*******************************************
*
************************************************************************************************/

typedef struct _CMD
{
  unsigned char ReceiveBuffer[32];
	unsigned char SendBuffer[32];
  unsigned char ReceivePoint;
	unsigned char SendPoint;
	unsigned char SendFlag;
  unsigned char ReceiveFlag;
}CMD;

typedef struct _PICC
{
	unsigned int Type;
	unsigned long UID;
	unsigned long Value;
	unsigned char KeyA[6];
	unsigned char KeyB[6];
	unsigned char BlockData[16];
}PICC;







/***********************************************************************************************
*
***********************************************ȫ�ֱ���*******************************************
*
************************************************************************************************/

extern PICC Picc;
extern CMD Cmd;





/***********************************************************************************************
*
***********************************************��������*******************************************
*
************************************************************************************************/

unsigned char CheckSum(unsigned char *dat, unsigned char num);
void SendCommand(void);
void ReadCard(void);
void ReadBlock(unsigned char block);
void WriteBlock(unsigned char block, unsigned char* data);
void MakeCard(unsigned char block, unsigned char value);
void SetDefault(unsigned char block);
void Inc(unsigned char block, unsigned char value);
void Dec(unsigned char block, unsigned char value);
void LodeKey(unsigned char* KeyA, unsigned char* KeyB, unsigned char mode);
void SetBaud(unsigned long baud);
void SetAddress(unsigned char address);
void SetMode(unsigned char mode, unsigned char block, unsigned long value);
unsigned char CheckSum(unsigned char *dat, unsigned char num);
unsigned char CommandProcess(void);
#endif

