#include <stdio.h>      /*标准输入输出定义*/   
#include <fcntl.h>      /*文件控制定义*/  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "MQ.h"

int mq_open()
{
	int fd = 0;

	fd = open(mq_dev_name, O_RDWR);		/* 打开MQ设备 */
	if (-1 == fd)  
	{  
		perror("open mq_dev_name error!\r\n");  
		return FALSE;  
	}  
	return fd;
}

/****************************************************************************************************
*函数名称:    mq_read
*函数参数:    fd:	文件描述符
*			  gas_concentration:	气体浓度
*函数返回值:  返回SUCCEE
*函数描述:    mq读取气体浓度状态函数
*****************************************************************************************************/
int mq_read(uint8_t *gas_concentration)
{
	int fd = 0;
	fd = mq_open();
	if (fd < 0)
		return FALSE;

	while (read(fd, gas_concentration, sizeof(uint8_t)));		/* 一直读取直到浓度超标 */
	printf("gas_concentration exceeding!!\r\n");

	return SUCCEE;
}
