/*
 * 文 件 名：MFRC522.c
 * 芯    片：STC12C5A60S2
 * 晶    振：12MHz
 * 创 建 者：冷月
 * 创建日期：2010.11.12
 * 修 改 者：
 * 修改日期：
 * 功能描述：Mifare1 寻卡→防冲突→选卡→读写 实现
 */

#include "MFRC522.h"
#include "SPI.h"



//4字节卡序列号，第5字节为校验字节
uint8_t serNum[5];


//函数原型申明
void SetBitMask(uint8_t reg, uint8_t mask);
void ClearBitMask(uint8_t reg, uint8_t mask);
void AntennaOn(void);
void AntennaOff(void);


/*
 * 函 数 名：Write_MFRC5200
 * 功能描述：向MFRC522的某一寄存器写一个字节数据
 * 输入参数：addr--寄存器地址；val--要写入的值
 * 返 回 值：无
 */
void Write_MFRC522(uint8_t addr, uint8_t val)
{
	s3c2410_gpio_setpin(RC522_SDA, 0);

	//地址格式：0XXXXXX0
	SPI_Write((addr<<1)&0x7E);	
	SPI_Write(val);
	
	s3c2410_gpio_setpin(RC522_SDA, 1);
}


/*
 * 函 数 名：Read_MFRC522
 * 功能描述：从MFRC522的某一寄存器读一个字节数据
 * 输入参数：addr--寄存器地址
 * 返 回 值：返回读取到的一个字节数据
 */
uint8_t Read_MFRC522(uint8_t addr)
{
	uint8_t val;

	s3c2410_gpio_setpin(RC522_SDA, 0);

	//地址格式：1XXXXXX0
	SPI_Write(((addr<<1)&0x7E) | 0x80);	
	val = SPI_Read();
	
	s3c2410_gpio_setpin(RC522_SDA, 1);
	
	return val;	
}

/*
 * 函 数 名：SetBitMask
 * 功能描述：置RC522寄存器位
 * 输入参数：reg--寄存器地址;mask--置位值
 * 返 回 值：无
 */
void SetBitMask(uint8_t reg, uint8_t mask)  
{
    uint8_t tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask);  // set bit mask
}


/*
 * 函 数 名：ClearBitMask
 * 功能描述：清RC522寄存器位
 * 输入参数：reg--寄存器地址;mask--清位值
 * 返 回 值：无
 */
void ClearBitMask(uint8_t reg, uint8_t mask)  
{
    uint8_t tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask));  // clear bit mask
} 


/*
 * 函 数 名：AntennaOn
 * 功能描述：开启天线,每次启动或关闭天险发射之间应至少有1ms的间隔
 * 输入参数：无
 * 返 回 值：无
 */
void AntennaOn(void)
{
	uint8_t temp;

	temp = Read_MFRC522(TxControlReg);
	if (!(temp & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}


/*
 * 函 数 名：AntennaOff
 * 功能描述：关闭天线,每次启动或关闭天险发射之间应至少有1ms的间隔
 * 输入参数：无
 * 返 回 值：无
 */
void AntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}


/*
 * 函 数 名：ResetMFRC522
 * 功能描述：复位RC522
 * 输入参数：无
 * 返 回 值：无
 */
void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);
}


/*
 * 函 数 名：InitMFRC522
 * 功能描述：初始化RC522
 * 输入参数：无
 * 返 回 值：无
 */
void MFRC522_Init(void)
{
	s3c2410_gpio_setpin(RC522_RST, 1);

	MFRC522_Reset();
	 	
	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    Write_MFRC522(TModeReg, 0x8D);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_MFRC522(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg
    Write_MFRC522(TReloadRegL, 30);           
    Write_MFRC522(TReloadRegH, 0);
	
	Write_MFRC522(TxAutoReg, 0x40);		//100%ASK
	Write_MFRC522(ModeReg, 0x3D);		//CRC初始值0x6363	???

	//ClearBitMask(Status2Reg, 0x08);		//MFCrypto1On=0
	//Write_MFRC522(RxSelReg, 0x86);		//RxWait = RxSelReg[5..0]
	//Write_MFRC522(RFCfgReg, 0x7F);   		//RxGain = 48dB

	AntennaOn();		//打开天线
}


/*
 * 函 数 名：MFRC522_Request
 * 功能描述：寻卡，读取卡类型号
 * 输入参数：reqMode--寻卡方式，
 *			 TagType--返回卡片类型
 *			 	0x4400 = Mifare_UltraLight
 *				0x0400 = Mifare_One(S50)
 *				0x0200 = Mifare_One(S70)
 *				0x0800 = Mifare_Pro(X)
 *				0x4403 = Mifare_DESFire
 * 返 回 值：成功返回MI_OK
 */
uint8_t MFRC522_Request(uint8_t reqMode, uint8_t *TagType)
{
	uint8_t status;  
	uint backBits;			//接收到的数据位数

	Write_MFRC522(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	???
	
	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10))
	{    
		status = MI_ERR;
	}
   
	return status;
}


/*
 * 函 数 名：MFRC522_ToCard
 * 功能描述：RC522和ISO14443卡通讯
 * 输入参数：command--MF522命令字，
 *			 sendData--通过RC522发送到卡片的数据, 
 *			 sendLen--发送的数据长度		 
 *			 backData--接收到的卡片返回数据，
 *			 backLen--返回数据的位长度
 * 返 回 值：成功返回MI_OK
 */
uint8_t MFRC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint *backLen)
{
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint i;

    switch (command)
    {
        case PCD_AUTHENT:		//认证卡密
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE:	//发送FIFO中数据
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
    }
   
    Write_MFRC522(CommIEnReg, irqEn|0x80);	//允许中断请求
    ClearBitMask(CommIrqReg, 0x80);			//清除所有中断请求位
    SetBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1, FIFO初始化
    
	Write_MFRC522(CommandReg, PCD_IDLE);	//NO action;取消当前命令	???

	//向FIFO中写入数据
    for (i=0; i<sendLen; i++)
    {   
		Write_MFRC522(FIFODataReg, sendData[i]);    
	}

	//执行命令
	Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {    
		SetBitMask(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts  
	}   
    
	//等待接收数据完成
	i = 2000;	//i根据时钟频率调整，操作M1卡最大等待时间25ms	???
    do 
    {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80);			//StartSend=0
	
    if (i != 0)
    {    
        if(!(Read_MFRC522(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   
				status = MI_NOTAGERR;			//??   
			}

            if (command == PCD_TRANSCEIVE)
            {
               	n = Read_MFRC522(FIFOLevelReg);
              	lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {   
					*backLen = (n-1)*8 + lastBits;   
				}
                else
                {   
					*backLen = n*8;   
				}

                if (n == 0)
                {   
					n = 1;    
				}
                if (n > MAX_LEN)
                {   
					n = MAX_LEN;   
				}
				
				//读取FIFO中接收到的数据
                for (i=0; i<n; i++)
                {   
					backData[i] = Read_MFRC522(FIFODataReg);    
				}
            }
        }
        else
        {   
			status = MI_ERR;  
		}
        
    }
	
    //SetBitMask(ControlReg,0x80);           //timer stops
    //Write_MFRC522(CommandReg, PCD_IDLE); 

    return status;
}


/*
 * 函 数 名：MFRC522_Anticoll
 * 功能描述：防冲突检测，读取选中卡片的卡序列号
 * 输入参数：serNum--返回4字节卡序列号,第5字节为校验字节
 * 返 回 值：成功返回MI_OK
 */
uint8_t MFRC522_Anticoll(uint8_t *serNum)
{
    uint8_t status;
    uint8_t i;
	uint8_t serNumCheck=0;
    uint unLen;
    

    //ClearBitMask(Status2Reg, 0x08);		//TempSensclear
    //ClearBitMask(CollReg,0x80);			//ValuesAfterColl
	Write_MFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]
 
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
	{
		//校验卡序列号
		for (i=0; i<4; i++)
		{   
		 	serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{   
			status = MI_ERR;    
		}
    }

    //SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1

    return status;
} 


/*
 * 函 数 名：CalulateCRC
 * 功能描述：用MF522计算CRC
 * 输入参数：pIndata--要读数CRC的数据，len--数据长度，pOutData--计算的CRC结果
 * 返 回 值：无
 */
void CalulateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
    uint8_t i, n;

    ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);			//清FIFO指针
    //Write_MFRC522(CommandReg, PCD_IDLE);

	//向FIFO中写入数据	
    for (i=0; i<len; i++)
    {   
		Write_MFRC522(FIFODataReg, *(pIndata+i));   
	}
    Write_MFRC522(CommandReg, PCD_CALCCRC);

	//等待CRC计算完成
    i = 0xFF;
    do 
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//读取CRC计算结果
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/*
 * 函 数 名：MFRC522_SelectTag
 * 功能描述：选卡，读取卡存储器容量
 * 输入参数：serNum--传入卡序列号
 * 返 回 值：成功返回卡容量
 */
uint8_t MFRC522_SelectTag(uint8_t *serNum)
{
    uint8_t i;
	uint8_t status;
	uint8_t size;
    uint recvBits;
    uint8_t buffer[9]; 

	//ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
    	buffer[i+2] = *(serNum+i);
    }
	CalulateCRC(buffer, 7, &buffer[7]);		//??
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    
    if ((status == MI_OK) && (recvBits == 0x18))
    {   
		size = buffer[0]; 
	}
    else
    {   
		size = 0;    
	}

    return size;
}


/*
 * 函 数 名：MFRC522_Auth
 * 功能描述：验证卡片密码
 * 输入参数：authMode--密码验证模式
                 0x60 = 验证A密钥
                 0x61 = 验证B密钥 
             BlockAddr--块地址
             Sectorkey--扇区密码
             serNum--卡片序列号，4字节
 * 返 回 值：成功返回MI_OK
 */
uint8_t MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum)
{
    uint8_t status;
    uint recvBits;
    uint8_t i;
	uint8_t buff[12]; 

	//验证指令+块地址＋扇区密码＋卡序列号
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {    
		buff[i+2] = *(Sectorkey+i);   
	}
    for (i=0; i<4; i++)
    {    
		buff[i+8] = *(serNum+i);   
	}
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
    {   
		status = MI_ERR;   
	}
    
    return status;
}


/*
 * 函 数 名：MFRC522_Read
 * 功能描述：读块数据
 * 输入参数：blockAddr--块地址;recvData--读出的块数据
 * 返 回 值：成功返回MI_OK
 */
uint8_t MFRC522_Read(uint8_t blockAddr, uint8_t *recvData)
{
    uint8_t status;
    uint unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }
    
    return status;
}


/*
 * 函 数 名：MFRC522_Write
 * 功能描述：写块数据
 * 输入参数：blockAddr--块地址;writeData--向块写16字节数据
 * 返 回 值：成功返回MI_OK
 */
uint8_t MFRC522_Write(uint8_t blockAddr, uint8_t *writeData)
{
    uint8_t status;
    uint recvBits;
    uint8_t i;
	uint8_t buff[18]; 
    
    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {   
		status = MI_ERR;   
	}
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++)		//向FIFO写16Byte数据
        {    
        	buff[i] = *(writeData+i);   
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
        
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {   
			status = MI_ERR;   
		}
    }
    
    return status;
}


/*
 * 函 数 名：MFRC522_Halt
 * 功能描述：命令卡片进入休眠状态
 * 输入参数：无
 * 返 回 值：无
 */
void MFRC522_Halt(void)
{
	uint8_t status;
    uint unLen;
    uint8_t buff[4]; 

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);
 
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}