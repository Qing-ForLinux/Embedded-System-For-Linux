/****************************************************************************************************
*文件名称:    rc522.c
*文件描述:    RFID操作函数集
*编写人:      杨鹏飞
*编写时间:    2016-10-16
*****************************************************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "my_string.h"

#include "rc522.h"

uint8_t g_DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
uint8_t g_data[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};

int rc522_init()
{
	int ret = 0;
	int fd = 0;
	int i = 0;
	uint8_t card_buf[20] = {0};
	
	/* 验证卡片密码 */
	g_Auth_State authstate = (g_Auth_State)malloc(sizeof(struct AUTHSTATE));	/* 分配结构 */
	/* 写块 */
	g_Write_Addr write_addr = (g_Write_Addr)malloc(sizeof(struct WRITEDAAR));	/* 分配结构 */
	/* 块备份 */
	g_Bak_Value bak_value = (g_Bak_Value)malloc(sizeof(struct BAKVALUE));
	
	memset(authstate, 0, sizeof(struct AUTHSTATE));	/* 初始化结构体 */
	memset(write_addr, 0 ,sizeof(struct WRITEDAAR));	/* 初始化结构体 */
	memset(bak_value, 0 ,sizeof(struct BAKVALUE));	/* 初始化结构体 */
	
	fd = open(rc522_dev_name, O_RDWR);
	if (fd < 0){
		perror("open error:");
		return FALSE;
	}
#if 0
	
	/* 复位卡 */
	ret = ioctl(fd, RC522_RESET_CMD);	/* 发送防冲撞命令 */
	if (ret != MI_OK)
		goto ioctl_error;
	
	/* 关闭天线 */
	ret = ioctl(fd, RC522_AntennaOff_CMD);	/* 发送防冲撞命令 */
	if (ret != MI_OK)
		goto ioctl_error;
	
	sleep(1);
	/* 打开天线 */
	ret = ioctl(fd, RC522_AntennaON_CMD);	/* 发送防冲撞命令 */
	if (ret != MI_OK)
		goto ioctl_error;
	
	/* 寻卡 */
	while (1){
		sleep(5);
		ret = ioctl(fd, RC522_Request_CMD);	/* 发送寻卡命令 */

		ret = read(fd, card_buf, sizeof(card_buf));	/* 读取卡片信息 */
		if (ret != MI_OK){
			/* 复位卡 */
			ret = ioctl(fd, RC522_RESET_CMD);	/* 发送防冲撞命令 */
			if (ret != MI_OK)
				goto ioctl_error;
			
			/* 关闭天线 */
			ret = ioctl(fd, RC522_AntennaOff_CMD);	/* 发送防冲撞命令 */
			if (ret != MI_OK)
				goto ioctl_error;
			
			sleep(1);
			/* 打开天线 */
			ret = ioctl(fd, RC522_AntennaON_CMD);	/* 发送防冲撞命令 */
			if (ret != MI_OK)
				goto ioctl_error;
			continue;	/* 如果没有读到就移植读取 */
		}
	}
	
	printf("card type is:");	/* 打印卡的类型 */
	for (i = 0; i < 2; i++)
		printf("%X", card_buf[i]);
	
	
	while(1);
	
	/* 防冲撞 */
	ret = ioctl(fd, RC522_Anticoll_CMD);	/* 发送防冲撞命令 */
	if (ret != MI_OK)
		goto ioctl_error;
	ret = read(fd, card_buf, sizeof(card_buf));
	if (ret != MI_OK)
		goto read_error;
	
	
	
	printf("card serial number:");	/* 打印卡序列号 */
	for(i=0;i<4;i++)
		printf("%X", card_buf[i]);
	
	/* 选定卡片 */
	ret = ioctl(fd, RC522_Select_CMD);	/* 发送选定卡片命令 */
	if (ret != MI_OK)
		goto ioctl_error;
	ret = write(fd, card_buf, sizeof(card_buf));
	if (ret != MI_OK)
		goto write_error;
	
	
	
	/* 验证卡片密码 */
	ret = ioctl(fd, RC522_AuthState_CMD);	/* 发送选定卡片命令 */
	if (ret != MI_OK)
		goto ioctl_error;
	/* 填充验证卡片密码的结构体成员 */
	authstate->cmd = PICC_AUTHENT1A;	/* 验证A卡片密码 */
	authstate->addr = 1;	/* 块地址 */
	/* 这里的copy要采用函数的方式，不能直接赋值 */
	char_table_copy(g_DefaultKey, authstate->key, sizeof(g_DefaultKey));	/* 密码 */
	char_table_copy(card_buf, authstate->car_str, sizeof(card_buf));	/* 卡片序列号 */
	ret = write(fd, authstate, sizeof(struct AUTHSTATE));
	if (ret != MI_OK)
		goto write_error;
	
	
	/* 写块 */
	ret = ioctl(fd, RC522_Write_CMD);	/* 发送写块命令 */
	/* 填充写块的结构体成员 */
	write_addr->addr = 1;	/* 块地址为1 */
	/* 这里的不能直接赋值，需要拷贝 */
	char_table_copy(g_data, write_addr->data, sizeof(g_data));	/* 要写入的数据 */
	ret = write(fd, write_addr, sizeof(struct WRITEDAAR));
	if (ret != MI_OK)
		goto write_error;
	
	
	
	/* 寻卡 */
	while (1){
		ret = ioctl(fd, RC522_Request_CMD);	/* 发送寻卡命令 */

		ret = read(fd, card_buf, sizeof(card_buf));	/* 读取卡片信息 */
		if (ret != MI_OK){
			perror("read error:");
			continue;	/* 如果没有读到就移植读取 */
		}
	}
	
	printf("card type is:");	/* 打印卡的类型 */
	for (i = 0; i < 2; i++)
		printf("%X", card_buf[i]);
	
	
	
	
	/* 防冲撞 */
	ret = ioctl(fd, RC522_Anticoll_CMD);	/* 发送防冲撞命令 */
	if (ret != MI_OK)
		goto ioctl_error;
	ret = read(fd, card_buf, sizeof(card_buf));
	if (ret != MI_OK)
		goto read_error;
	
	
	
	printf("card serial number:");	/* 打印卡序列号 */
	for(i=0;i<4;i++)
		printf("%X", card_buf[i]);
	
	/* 选定卡片 */
	ret = ioctl(fd, RC522_Select_CMD);	/* 发送选定卡片命令 */
	if (ret != MI_OK)
		goto ioctl_error;
	ret = write(fd, card_buf, sizeof(card_buf));
	if (ret != MI_OK)
		goto write_error;
	
	
	
	/* 验证卡片密码 */
	ret = ioctl(fd, RC522_AuthState_CMD);	/* 发送选定卡片命令 */
	if (ret != MI_OK)
		goto ioctl_error;
	/* 填充验证卡片密码的结构体成员 */
	authstate->cmd = PICC_AUTHENT1A;	/* 验证A卡片密码 */
	authstate->addr = 1;	/* 块地址 */
	/* 这里的copy要采用函数的方式，不能直接赋值 */
	char_table_copy(g_DefaultKey, authstate->key , sizeof(g_DefaultKey));	/* 密码 */
	char_table_copy(card_buf, authstate->car_str , sizeof(card_buf));	 /* 卡片序列号 */
	ret = write(fd, authstate, sizeof(struct AUTHSTATE));
	if (ret != MI_OK)
		goto write_error;
	
	
	
	/* 块备份 */
	ret = ioctl(fd, RC522_BakValue_CMD);	/* 发送块备份命令*/
	if (ret != MI_OK)
		goto ioctl_error;
	
	/* 填充块备份的结构体成员 */
	bak_value->s_addr = 1;		/* 源块 */
	bak_value->d_addr = 2;		/* 目标块 */
	ret = write(fd, bak_value, sizeof(struct BAKVALUE));
	if (ret != MI_OK)
		goto write_error;
	
	
	
	/* 读块 */
	ret = ioctl(fd, 2, RC522_Read_CMD);	/* 发送读块命令  读取第2个块地址的值*/
	if (ret != MI_OK)
		goto ioctl_error;
	ret = read(fd, card_buf, sizeof(card_buf));
	if (ret != MI_OK)
		goto read_error;
	
	
	
	/* 打印块信息 */
	printf("card block data is:");	/* 打印块信息 */
	for(i=0;i<16;i++)
		printf("%X", card_buf[i]);

	

ioctl_error:
	perror("ioctrl error:");
	return FALSE;
	
read_error:
	perror("read error:");
	return FALSE;
	
write_error:
	perror("write error:");
	return FALSE;
#endif

	return SUCCEE;
}










