#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


#include "common.h"
#include "uart.h"
#include "PICC.h"
#include "Y13R.h"

extern PICC Picc;
extern CMD Cmd;

/* ������Ҫ���߳�֧�� */

/******************************************************************************************************
* ��������:    y13r_init
* ��������:    void
* ��������ֵ:  �ɹ�����fd,ʧ�ܷ���FALSE
* ��������:    y13rģ���ʼ������
******************************************************************************************************/
int y13r_init(void)
{
	int ret = 0;
	int fd = 0;
	fd = uart_open(Y13R_DEV);	/* �򿪴����豸 */
	if (fd  == FALSE)
		return FALSE;
	ret = uart_set(fd, 9600, 0, 8, 1, 'N');	/* ���ô��� */
	if (ret == FALSE){
		uart_close(fd);	/* �����������ʧ�ܣ���رմ����豸 */
		return FALSE;
	}
	return fd;

}


/******************************************************************************************************
* ��������:    y13r_recv
* ��������:    �ļ�������:	fd
* ��������ֵ:  ��
* ��������:    y13r���ݽ��պ���
******************************************************************************************************/

void y13r_recv(int fd)
{
	static uint8_t bTemp = 0;
	static uint8_t flag=0;
	static uint8_t sflag = 0;
	
	bTemp = uart_rec_char(fd);	/* �Ӵ��ڽ������� ÿ�ν���һ���ַ�*/  
	
	/* �����������ɲŽ��� */
	if(Cmd.ReceiveFlag == 0){
		/* 7F ��ǣ�Ϊ 0 ��ʾ��һ�����ݲ���7F */
		if(flag == 0){
			/* ��һ����ǲ���7F������ǣ����ϱ�� */
			if(bTemp == 0x7F)
				flag = 1;
			
			/* ��ֵ������ջ��� */
			Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
		}
		else{
			flag = 0;
			
			/* ��һ����7F����һ�����Ǳ�ʾ�յ�������ͷ�� */
			if(bTemp != 0x7F){
				Cmd.ReceivePoint = 0;
				
				Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
			}
			/* ��һ����7F����һ��Ҳ�ǣ�����Ե�ǰ��7F */
		}
		if(Cmd.ReceivePoint >= 32)
			Cmd.ReceivePoint = 0;
		
		/* ����ָ����� 2 ����ʼ�Ա�����Ⱥͽ���ָ�룬һ�±�ʾ�յ��������� */
		if(Cmd.ReceivePoint > 2){
			if(Cmd.ReceivePoint == Cmd.ReceiveBuffer[0]+1){
				Cmd.ReceiveFlag = 1;
			}
		}
	}

}


/******************************************************************************************************
* ��������:    y13r_send
* ��������:    �ļ�������:	fd
* ��������ֵ:  ��
* ��������:    y13r���ݷ��ͺ���
******************************************************************************************************/

void y13r_send(int fd)
{
	static unsigned char sflag = 0;
	uart_send_char(fd, 0x7F); /* �ȷ�������ͷ */
		
	/* ����ָ�벻Ϊ0ʱ�������� */
	while(Cmd.SendPoint != 0){
		/* 7F�ж� */
		if(sflag == 0){
			
			Cmd.SendPoint--;
			
			uart_send_char(fd, Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint]);

			/* ������͵������а�����0x7f���ٷ���һ��0x7f */
			if(Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint] == 0x7F)
				sflag = 1;
		}
		else{
			sflag = 0;
			uart_send_char(fd, 0x7F);
		}
	}
	/* ����ָ��Ϊ0ʱ���Ϸ��ͱ�Ǳ�ʾ������� */
	Cmd.SendFlag = 0;
}
















