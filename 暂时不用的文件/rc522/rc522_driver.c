#include "rc522_driver.h"
#include "SPI.h"
#include "MFRC522.h"

#if 0
int g_read_cmd = -1;	/* 读取控制命令 */
int g_write_cmd = -1;	/* 写控制命令 */

uint32_t read_block_num = 0;	/* 读取的块编号 */
#endif


static int rc522_open(struct inode *inode, struct file *file)
{
	uint8_t str[MAX_LEN];
	uint8_t writeData[16]={1, 2, 3, 4, 0};
	uint8_t i;
	uint8_t status;
	uint8_t size;
	uint8_t blockAddr;	//选择操作的块地址0～63
	//扇区A密码，16个扇区，每个扇区密码6Byte
	uint8_t sectorKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},};	

	printk("rc522_open\r\n");
	
	SPI_Init();
	MFRC522_Init();

	while (1)
	{
		//寻卡，返回卡类型	
		status = MFRC522_Request(PICC_REQIDL, str);	
		if (status == MI_OK)
		{
			printk("%d \r\n", str[0]);
			printk("%d \r\n", str[1]);
		}

		//防冲撞，返回卡的序列号 4字节
		status = MFRC522_Anticoll(str);
		memcpy(serNum, str, 5);
		if (status == MI_OK)
		{
			printk("%d \r\n", serNum[0]);
			printk("%d \r\n", serNum[1]);
			printk("%d \r\n", serNum[2]);
			printk("%d \r\n", serNum[3]);
			printk("%d \r\n", serNum[4]);
		}

		//选卡，返回卡容量
		size = MFRC522_SelectTag(serNum);
		if (size != 0)
		{
			printk("size is %d", size);
		}

		//写
		blockAddr = 1;		//数据块		
		status = MFRC522_Auth(PICC_AUTHENT1A, blockAddr, sectorKeyA[blockAddr/4], serNum);	//认证
		if (status == MI_OK)
		{
			//写数据
			status = MFRC522_Write(blockAddr, writeData);
		}

		//读
		blockAddr = 1;		//数据块		
		status = MFRC522_Auth(PICC_AUTHENT1A, blockAddr, sectorKeyA[blockAddr/4], serNum);	//认证
		if (status == MI_OK)
		{
			//读数据
			status = MFRC522_Read(blockAddr, str);
			if (status == MI_OK)
			{
				for (i=0; i<16; i++)
				{
					printk("%d ", str[i]);
				}
			}
		}

		MFRC522_Halt();			//命令卡片进入休眠状态*/
	}	
} 


/****************************************************************************************************
*函数名称:    rc522_read
*函数参数:    filp:		文件指针
*			  buf:		数据读取存储数组
*			  size:		需要读取的数据长度
*			  pos:		文件偏移
*函数返回值:  成功返回0 错误返回-1
*函数描述:    spi读取数据函数
****************************************************************************************************/
static ssize_t rc522_read(struct file *filp, char __user * buf, size_t size, loff_t *pos)
{
	int ret = 0;
#if 0
	uint8_t read_buf[20] = {0};  /* 读取数据暂存数组 */
	
	switch (g_read_cmd)
	{
		case RC522_Request_CMD:
			ret = PcdRequest(PICC_REQALL, read_buf);	/* 寻找卡 */
			printk("PcdRequest = %d\r\n",ret);
			if (ret < 0)
				printk(KERN_ERR "PcdRequest error!\r\n");
			
			break;	
			
		case RC522_Anticoll_CMD:
			ret = PcdAnticoll(read_buf);//防冲撞
			if (ret != MI_OK)
				printk(KERN_ERR "PcdAnticoll error!\r\n");
			
			copy_to_user( buf, read_buf, size );
			break;
		
		
		case RC522_Read_CMD:
			ret = PcdRead( read_block_num, read_buf );//读块
			if (ret < 0)
				printk(KERN_ERR "PcdRead error!\r\n");

			copy_to_user( buf, read_buf, size );	
			
		default:
			printk(KERN_ERR "cmd is Invalid!\r\n");
	}
#endif
	
	return ret;
}


/****************************************************************************************************
*函数名称:    rc522_write
*函数参数:    filp:		文件指针
*			  buf:		数据写入的数组
*			  size:		需要读取的数据长度
*			  pos:		文件偏移
*函数返回值:  成功返回0 错误返回-1
*函数描述:    spi写入数据函数
****************************************************************************************************/
static ssize_t rc522_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{	
	int ret = 0;
#if 0
	uint8_t write_buf[20] = {0};
	
	/* 验证卡片密码 */
	g_Auth_State authstate = (g_Auth_State)kmalloc(sizeof(struct AUTHSTATE), GFP_KERNEL);	/* 分配结构 */
	/* 写块 */
	g_Write_Addr write_addr = (g_Write_Addr)kmalloc(sizeof(struct WRITEDAAR), GFP_KERNEL);	/* 分配结构 */
	/* 块备份 */
	g_Bak_Value bak_value = (g_Bak_Value)kmalloc(sizeof(struct BAKVALUE), GFP_KERNEL);
	
	memset(authstate, 0, sizeof(struct AUTHSTATE));	/* 初始化结构体 */
	memset(write_addr, 0 ,sizeof(struct WRITEDAAR));	/* 初始化结构体 */
	memset(bak_value, 0 ,sizeof(struct BAKVALUE));	/* 初始化结构体 */
	
	switch (g_write_cmd)
	{
		case RC522_Select_CMD:	/* 选定卡片 */
			copy_from_user( write_buf, buf, size );
			ret = PcdSelect(write_buf);//选定卡片
			if (ret != MI_OK)
				printk(KERN_ERR "PcdSelect error!\r\n");
			break;
			
		case RC522_AuthState_CMD:	/* 验证卡片密码 */
			copy_from_user( authstate, buf, size );
			ret = PcdAuthState(authstate->cmd, authstate->addr, authstate->key, authstate->car_str);//验证卡片密码
			if (ret != MI_OK)
				printk(KERN_ERR "PcdSelect error!\r\n");
			break;
			
		case RC522_Write_CMD:	/* 写块 */
			copy_from_user( write_addr, buf, size );
			ret = PcdWrite(write_addr->addr, write_addr->data);//写块
			if (ret != MI_OK)
				printk(KERN_ERR "PcdSelect error!\r\n");
			break;
			
		case RC522_BakValue_CMD:	/* 块备份 */
			copy_from_user( bak_value, buf, size );
			ret = PcdBakValue(bak_value->s_addr, bak_value->d_addr);//块备份
			if (ret != MI_OK)
				printk(KERN_ERR "PcdSelect error!\r\n");
			break;
			
		default:
			printk(KERN_ERR "cmd is Invalid!\r\n");
	}
	
	kfree(authstate);
	kfree(write_addr);
	kfree(bak_value);

#endif
	
	return ret;
}


/****************************************************************************************************
*函数名称:    rc522_ioctl
*函数参数:    filp:		文件指针
*			  cmd:		控制命令
*			  arg:		传递的参数
*函数返回值:  成功返回0 错误返回-1
*函数描述:    spi控制函数
****************************************************************************************************/
static int rc522_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	
#if 0
	read_block_num = 0;		/* 初始化为0 */
	g_read_cmd = -1;		/* 初始化为-1 */
	g_write_cmd = -1;

	printk("cmd is %x\r\n", cmd);
	
	switch(cmd)
	{
		case RC522_RESET_CMD:	/* 复位 */
			PcdReset();
			break;
			
		case RC522_AntennaOff_CMD:	/* 关闭天线 */
			PcdAntennaOff();
			break;
			
		case RC522_AntennaON_CMD:	/* 开启天线 */
			PcdAntennaOn(); 
			break;
			
		case RC522_Request_CMD:		/* 寻卡 */
			g_read_cmd  = RC522_Request_CMD;	/* 寻卡标志 */
			break;
			
		case RC522_Anticoll_CMD:
			g_read_cmd  = RC522_Anticoll_CMD;	/* 防冲撞 */

			break;
		
		case RC522_Select_CMD:
			g_write_cmd = RC522_Select_CMD;	/* 选定卡片 */
			break;
		
		case RC522_AuthState_CMD:
			g_write_cmd = RC522_AuthState_CMD;	/* 验证卡片密码 */
			break;
			
		case RC522_Write_CMD: 
			g_write_cmd = RC522_Write_CMD;	/* 写块 */
			break;
			
		case RC522_Read_CMD: /* 读块 */
			g_read_cmd = RC522_Read_CMD;
			read_block_num = arg;	/* 需要读取的块 */
			break;
			
		case RC522_BakValue_CMD:	/* 块备份 */
			g_write_cmd = RC522_BakValue_CMD;
			break;
			
		default:
			printk(KERN_ERR "cmd is Invalid!\r\n");
			ret = -1;
	}
#endif
	
	return ret;
}


struct file_operations spi_fops = {
	.owner = THIS_MODULE,
	.open = rc522_open,
	.read = rc522_read,
	.write = rc522_write,
	.ioctl = rc522_ioctl,		/* 保留 先不使用  以后写成通用的设置 */
};


struct miscdevice spi_miscdev = {
    .minor = 212,
    .name = "rc522_spi",
    .fops = &spi_fops,
};


/****************************************************************************************************
*函数名称:    rc522_init
*函数参数:    void
*函数返回值:  int
*函数描述:    模块加载函数
****************************************************************************************************/
static int rc522_init(void)
{
	int ret = 0;

    ret = misc_register(&spi_miscdev);	/*创建混杂设备*/

	if (ret < 0)
	{
		printk(KERN_ERR "misc_register error!\r\n");
		return -1;
	}
	
	return 0;
}


/****************************************************************************************************
*函数名称:    rc522_exit
*函数参数:    void
*函数返回值:  无
*函数描述:    模块卸载函数
****************************************************************************************************/ 
static void rc522_exit(void)
{
	int ret = 0;
    ret = misc_deregister(&spi_miscdev);	/*注销混杂设备*/
	if (ret < 0)
		printk(KERN_ERR"misc_register error!\r\n");
}

module_init(rc522_init);
module_exit(rc522_exit);
MODULE_LICENSE("GPL");

