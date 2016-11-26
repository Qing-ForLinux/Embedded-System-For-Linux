/******************** (C) COPYRIGHT 2013 ********************
* 文件名          : PICC.c
* 作者            : 优灵电子开发团队。
* 版本            : V2.0.1.0
* 日期            : 2013-5-10
* 描述            : 所有对卡的操作，给模块发送的命令。
* 技术支持        : QQ: 526181679
********************************************************************************/

/* 包含文件 -------------------------------------------------------------------*/
#include "PICC.h"
/* 宏定义 ---------------------------------------------------------------------*/
/* 本文件使用的变量 -----------------------------------------------------------*/
PICC Picc;
CMD Cmd;
/* 本文件使用的函数声明 -------------------------------------------------------*/
void SendCommand(void);
/* 本文件函数体 ---------------------------------------------------------------*/



/*******************************************************************************
* 函数名         : CheckSum
* 描述           : 命令校验。
* 输入           : 	dat : 要校验的数组。
					num : 校验的字节数。
* 输出           : 无
* 返回           : 校验值。
*******************************************************************************/
unsigned char CheckSum(unsigned char *dat, unsigned char num)
{
  unsigned char bTemp = 0, i;
  
  for(i = 0; i < num; i ++){bTemp ^= dat[i];}
  return bTemp;
}

/*******************************************************************************
* 函数名         : SendCommand
* 描述           : 命令开始发送函数。
* 输入           : 无
* 输出           : 无
* 返回           : 无
*******************************************************************************/
void SendCommand(void)
{
	while(Cmd.SendFlag != 0);
	Cmd.SendFlag = 1;
	Cmd.SendBuffer[Cmd.SendBuffer[0]] = CheckSum(Cmd.SendBuffer, Cmd.SendBuffer[0]);
	Cmd.SendPoint = Cmd.SendBuffer[0] + 1;
}


/*******************************************************************************
* 函数名         : ReadCard
* 描述           : 读卡号
* 输入           : 无
* 输出           : 无
* 返回           : 无
*******************************************************************************/
void ReadCard(void)
{
	Cmd.SendBuffer[0] = 03;
	Cmd.SendBuffer[1] = 00;
	Cmd.SendBuffer[2] = 0x10;
	SendCommand();
}

/*******************************************************************************
* 函数名         : ReadBlock
* 描述           : 读块数据
* 输入           : block-块号
* 输出           : 无
* 返回           : 无
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
* 函数名         : WriteBlock
* 描述           : 写块数据
* 输入           : block-块号
									 data -块数据
* 输出           : 无
* 返回           : 无
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
* 函数名         : MakeCard
* 描述           : 办卡
* 输入           : block-块号
									 value-初始值
* 输出           : 无
* 返回           : 无
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
* 函数名         : SetDefault
* 描述           : 恢复卡
* 输入           : block-块号
* 输出           : 无
* 返回           : 无
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
* 函数名         : Inc
* 描述           : 充值
* 输入           : block-块号
									 value-金额
* 输出           : 无
* 返回           : 无
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
* 函数名         : Dec
* 描述           : 扣款
* 输入           : block-块号
									 value-金额
* 输出           : 无
* 返回           : 无
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
* 函数名         : LodeKey
* 描述           : 装载密码
* 输入           : KeyA -密钥A
									 KeyB -密钥B
									 mode -加密模式
* 输出           : 无
* 返回           : 无
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
* 函数名         : SetBaud
* 描述           : 设置波特率
* 输入           : baud -波特率
* 输出           : 无
* 返回           : 无
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
* 函数名         : SetAddress
* 描述           : 设置地址
* 输入           : address -地址
* 输出           : 无
* 返回           : 无
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
* 函数名         : SetMode
* 描述           : 设置地址
* 输入           : address -地址
* 输出           : 无
* 返回           : 无
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
* 函数名         : CommandProcess
* 描述           : 命令处理。
* 输入           : 无
* 输出           : 无
* 返回           : 执行结果。
                   -0   : 正确。
                   -0xFF: 无卡。
                   -0xFE: 卡操作错误。
                   -0xFD: 命令错误。
                   -0xFC: 没有收到命令。
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
				/* 读卡号 */
				case 0x90:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					break;
				/* 读块数据 */
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
				/* 写块数据 */
				case 0x92:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					break;
				/* 办卡 */
				case 0x93:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					break;
				/* 恢复卡 */
				case 0x94:
					Picc.UID = Cmd.ReceiveBuffer[9];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[8];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[7];
					Picc.UID <<= 8;
					Picc.UID |= Cmd.ReceiveBuffer[6];
					break;
				/* 充值 */
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
				/* 扣款 */
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
				/* 保留 */
				case 0xAA:
					break;
				/* 装载密码 */
				case 0xAB:
					break;
				/* 设置波特率 */
				case 0xAC:
					break;
				/* 设置地址 */
				case 0xAD:
					break;
				/* 设置工作模式 */	
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


