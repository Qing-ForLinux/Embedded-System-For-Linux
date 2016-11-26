#include <stdio.h>      /*��׼�����������*/   
#include <fcntl.h>      /*�ļ����ƶ���*/  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "MQ.h"

int mq_open()
{
	int fd = 0;

	fd = open(mq_dev_name, O_RDWR);		/* ��MQ�豸 */
	if (-1 == fd)  
	{  
		perror("open mq_dev_name error!\r\n");  
		return FALSE;  
	}  
	return fd;
}

/****************************************************************************************************
*��������:    mq_read
*��������:    fd:	�ļ�������
*			  gas_concentration:	����Ũ��
*��������ֵ:  ����SUCCEE
*��������:    mq��ȡ����Ũ��״̬����
*****************************************************************************************************/
int mq_read(uint8_t *gas_concentration)
{
	int fd = 0;
	fd = mq_open();
	if (fd < 0)
		return FALSE;

	while (read(fd, gas_concentration, sizeof(uint8_t)));		/* һֱ��ȡֱ��Ũ�ȳ��� */
	printf("gas_concentration exceeding!!\r\n");

	return SUCCEE;
}
