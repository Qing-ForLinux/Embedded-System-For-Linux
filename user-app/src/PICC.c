/******************** (C) COPYRIGHT 2013 ********************
* �ļ���          : PICC.c
* ����            : ������ӿ����Ŷӡ�
* �汾            : V2.0.1.0
* ����            : 2013-5-10
* ����            : ���жԿ��Ĳ�������ģ�鷢�͵����
* ����֧��        : QQ: 526181679
********************************************************************************/

/* �����ļ� -------------------------------------------------------------------*/
#include "PICC.h"
/* �궨�� ---------------------------------------------------------------------*/
/* ���ļ�ʹ�õı��� -----------------------------------------------------------*/
PICC Picc;
CMD Cmd;
/* ���ļ�ʹ�õĺ������� -------------------------------------------------------*/
void SendCommand(void);
/* ���ļ������� ---------------------------------------------------------------*/



/*******************************************************************************
* ������         : CheckSum
* ����           : ����У�顣
* ����           : 	dat : ҪУ������顣
					num : У����ֽ�����
* ���           : ��
* ����           : У��ֵ��
*******************************************************************************/
unsigned char CheckSum(unsigned char *dat, unsigned char num)
{
  unsigned char bTemp = 0, i;
  
  for(i = 0; i < num; i ++){bTemp ^= dat[i];}
  return bTemp;
}

/*******************************************************************************
* ������         : SendCommand
* ����           : ���ʼ���ͺ�����
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void SendCommand(void)
{
	while(Cmd.SendFlag != 0);
	Cmd.SendFlag = 1;
	Cmd.SendBuffer[Cmd.SendBuffer[0]] = CheckSum(Cmd.SendBuffer, Cmd.SendBuffer[0]);
	Cmd.SendPoint = Cmd.SendBuffer[0] + 1;
}


/*******************************************************************************
* ������         : ReadCard
* ����           : ������
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void ReadCard(void)
{
	Cmd.SendBuffer[0] = 03;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x10;
	SendCommand();
}

/*******************************************************************************
* ������         : ReadBlock
* ����           : ��������
* ����           : block-���
* ���           : ��
* ����           : ��
*******************************************************************************/
void ReadBlock(unsigned char block)
{
	Cmd.SendBuffer[0] = 04;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x11;
	Cmd.SendBuffer[3] = block;
	SendCommand();
}

/*******************************************************************************
* ������         : WriteBlock
* ����           : д������
* ����           : block-���
									 data -������
* ���           : ��
* ����           : ��
*******************************************************************************/
void WriteBlock(unsigned char block, unsigned char* data)
{
	unsigned char i;
	Cmd.SendBuffer[0] = 20;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x12;
	Cmd.SendBuffer[3] = block;
	for(i = 0; i < 16; i++)
	{
		Cmd.SendBuffer[i + 4] = *(data + i);
	}
	SendCommand();
}

/*******************************************************************************
* ������         : MakeCard
* ����           : �쿨
* ����           : block-���
									 value-��ʼֵ
* ���           : ��
* ����           : ��
*******************************************************************************/
void MakeCard(unsigned char block, unsigned char value)
{
	Cmd.SendBuffer[0] = 8;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x13;
	Cmd.SendBuffer[3] = block;
	Cmd.SendBuffer[4] = value >> 24;
	Cmd.SendBuffer[5] = value >> 16;
	Cmd.SendBuffer[6] = value >> 8;
	Cmd.SendBuffer[7] = value;
	SendCommand();
}

/*******************************************************************************
* ������         : SetDefault
* ����           : �ָ���
* ����           : block-���
* ���           : ��
* ����           : ��
*******************************************************************************/
void SetDefault(unsigned char block)
{
	Cmd.SendBuffer[0] = 4;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x14;
	Cmd.SendBuffer[3] = block;
	SendCommand();
}

/*******************************************************************************
* ������         : Inc
* ����           : ��ֵ
* ����           : block-���
									 value-���
* ���           : ��
* ����           : ��
*******************************************************************************/
void Inc(unsigned char block, unsigned char value)
{
	Cmd.SendBuffer[0] = 8;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x15;
	Cmd.SendBuffer[3] = block;
	Cmd.SendBuffer[4] = value >> 24;
	Cmd.SendBuffer[5] = value >> 16;
	Cmd.SendBuffer[6] = value >> 8;
	Cmd.SendBuffer[7] = value;
	SendCommand();
}

/*******************************************************************************
* ������         : Dec
* ����           : �ۿ�
* ����           : block-���
									 value-���
* ���           : ��
* ����           : ��
*******************************************************************************/
void Dec(unsigned char block, unsigned char value)
{
	Cmd.SendBuffer[0] = 8;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x16;
	Cmd.SendBuffer[3] = block;
	Cmd.SendBuffer[4] = value >> 24;
	Cmd.SendBuffer[5] = value >> 16;
	Cmd.SendBuffer[6] = value >> 8;
	Cmd.SendBuffer[7] = value;
	SendCommand();
}

/*******************************************************************************
* ������         : LodeKey
* ����           : װ������
* ����           : KeyA -��ԿA
									 KeyB -��ԿB
									 mode -����ģʽ
* ���           : ��
* ����           : ��
*******************************************************************************/
void LodeKey(unsigned char* KeyA, unsigned char* KeyB, unsigned char mode)
{
	unsigned char i;
	
	Cmd.SendBuffer[0] = 21;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x2B;
	for(i = 0; i < 6; i++)
	{
		Cmd.SendBuffer[3 + i] = *(KeyA + i);
		Cmd.SendBuffer[9 + i] = *(KeyB + i);
	}
	Cmd.SendBuffer[15] = mode;
	Cmd.SendBuffer[16] = 0x03;
	Cmd.SendBuffer[17] = 0x08;
	Cmd.SendBuffer[18] = 0x05;
	Cmd.SendBuffer[19] = 0x02;
	Cmd.SendBuffer[20] = 0x07;
	
	SendCommand();
}

/*******************************************************************************
* ������         : SetBaud
* ����           : ���ò�����
* ����           : baud -������
* ���           : ��
* ����           : ��
*******************************************************************************/
void SetBaud(unsigned long baud)
{
	Cmd.SendBuffer[0] = 10;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x2C;
	Cmd.SendBuffer[3] = baud >> 24;
	Cmd.SendBuffer[4] = baud >> 16;
	Cmd.SendBuffer[5] = baud >> 8;
	Cmd.SendBuffer[6] = baud;
	Cmd.SendBuffer[7] = 0x98;
	Cmd.SendBuffer[8] = 0x24;
	Cmd.SendBuffer[9] = 0x31;
	
	SendCommand();
}

/*******************************************************************************
* ������         : SetAddress
* ����           : ���õ�ַ
* ����           : address -��ַ
* ���           : ��
* ����           : ��
*******************************************************************************/
void SetAddress(unsigned char address)
{
	Cmd.SendBuffer[0] = 10;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x2D;
	Cmd.SendBuffer[3] = address;
	Cmd.SendBuffer[4] = 0x37;
	Cmd.SendBuffer[5] = 0x21;
	Cmd.SendBuffer[6] = 0x56;
	SendCommand();
}

/*******************************************************************************
* ������         : SetMode
* ����           : ���õ�ַ
* ����           : address -��ַ
* ���           : ��
* ����           : ��
*******************************************************************************/
void SetMode(unsigned char mode, unsigned char block, unsigned long value)
{
	Cmd.SendBuffer[0] = 10;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x2E;
	Cmd.SendBuffer[3] = mode;
	Cmd.SendBuffer[4] = mode + 0x0A;
	Cmd.SendBuffer[5] = block;
	Cmd.SendBuffer[6] = value >> 24;
	Cmd.SendBuffer[7] = value >> 16;
	Cmd.SendBuffer[8] = value >> 8;
	Cmd.SendBuffer[9] = value;
	
	Cmd.SendBuffer[10] = 0x23;
	Cmd.SendBuffer[11] = 0x12;
	Cmd.SendBuffer[12] = 0x54;
	SendCommand();
}

/*******************************************************************************
* ������         : CommandProcess
* ����           : �����
* ����           : ��
* ���           : ��
* ����           : ִ�н����
                   -0   : ��ȷ��
                   -0xFF: �޿���
                   -0xFE: ����������
                   -0xFD: �������
                   -0xFC: û���յ����
*******************************************************************************/
unsigned char CommandProcess(void)
{
	unsigned char status = 0xFC, i;
	
	if(Cmd.ReceiveFlag)
	{
		if(Cmd.ReceiveBuffer[Cmd.ReceiveBuffer[0]] == CheckSum(Cmd.ReceiveBuffer, Cmd.ReceiveBuffer[0]))
		{
			switch(Cmd.ReceiveBuffer[2])
			{
				/* ������ */
				case 0x90:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					break;
				/* �������� */
				case 0x91:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					
					for(i = 0; i < 16; i++)
					{
						Picc.BlockData[i] = Cmd.ReceiveBuffer[10 + i];
					}
					break;
				/* д������ */
				case 0x92:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					break;
				/* �쿨 */
				case 0x93:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					break;
				/* �ָ��� */
				case 0x94:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					break;
				/* ��ֵ */
				case 0x95:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					
					Picc.Value = Cmd.ReceiveBuffer[10];
					Picc.Value <<= 8;
					Picc.Value |= Cmd.ReceiveBuffer[11];
					Picc.Value <<= 8;
					Picc.Value |= Cmd.ReceiveBuffer[12];
					Picc.Value <<= 8;
					Picc.Value |= Cmd.ReceiveBuffer[13];
					break;
				/* �ۿ� */
				case 0x96:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					
					Picc.Value = Cmd.ReceiveBuffer[10];
					Picc.Value <<= 8;
					Picc.Value |= Cmd.ReceiveBuffer[11];
					Picc.Value <<= 8;
					Picc.Value |= Cmd.ReceiveBuffer[12];
					Picc.Value <<= 8;
					Picc.Value |= Cmd.ReceiveBuffer[13];
					break;
				/* ���� */
				case 0xAA:
					break;
				/* װ������ */
				case 0xAB:
					break;
				/* ���ò����� */
				case 0xAC:
					break;
				/* ���õ�ַ */
				case 0xAD:
					break;
				/* ���ù���ģʽ */	
				case 0x2E:
					break;
					default:status = 0xFD; break;
			}
			status = Cmd.ReceiveBuffer[3];
		}
		else{status = 0xFD;}
		Cmd.ReceiveFlag = 0;
	}
	return status;
}


