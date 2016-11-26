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

/* 这里需要多线程支持 */

/******************************************************************************************************
* 函数名称:    y13r_init
* 函数参数:    void
* 函数返回值:  成功返回fd,失败返回FALSE
* 函数描述:    y13r模块初始化函数
******************************************************************************************************/
int y13r_init(void)
{
	int ret = 0;
	int fd = 0;
	fd = uart_open(Y13R_DEV);	/* 打开串口设备 */
	if (fd  == FALSE)
		return FALSE;
	ret = uart_set(fd, 9600, 0, 8, 1, 'N');	/* 设置串口 */
	if (ret == FALSE){
		uart_close(fd);	/* 如果串口设置失败，则关闭串口设备 */
		return FALSE;
	}
	return fd;

}


/******************************************************************************************************
* 函数名称:    y13r_recv
* 函数参数:    文件描述符:	fd
* 函数返回值:  无
* 函数描述:    y13r数据接收函数
******************************************************************************************************/

void y13r_recv(int fd)
{
	static uint8_t bTemp = 0;
	static uint8_t flag=0;
	static uint8_t sflag = 0;
	
	bTemp = uart_rec_char(fd);	/* 从串口接收数据 每次接收一个字符*/  
	
	/* 上条命令处理完成才接收 */
	if(Cmd.ReceiveFlag == 0){
		/* 7F 标记，为 0 表示上一个数据不是7F */
		if(flag == 0){
			/* 上一个标记不是7F，这个是，打上标记 */
			if(bTemp == 0x7F)
				flag = 1;
			
			/* 把值存进接收缓存 */
			Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
		}
		else{
			flag = 0;
			
			/* 上一个是7F，这一个不是表示收到【命令头】 */
			if(bTemp != 0x7F){
				Cmd.ReceivePoint = 0;
				
				Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
			}
			/* 上一个是7F，这一个也是，则忽略当前的7F */
		}
		if(Cmd.ReceivePoint >= 32)
			Cmd.ReceivePoint = 0;
		
		/* 接收指针大于 2 个开始对比命令长度和接收指针，一致表示收到完整命令 */
		if(Cmd.ReceivePoint > 2){
			if(Cmd.ReceivePoint == Cmd.ReceiveBuffer[0]+1){
				Cmd.ReceiveFlag = 1;
			}
		}
	}

}


/******************************************************************************************************
* 函数名称:    y13r_send
* 函数参数:    文件描述符:	fd
* 函数返回值:  无
* 函数描述:    y13r数据发送函数
******************************************************************************************************/

void y13r_send(int fd)
{
	static unsigned char sflag = 0;
	uart_send_char(fd, 0x7F); /* 先发送命令头 */
		
	/* 发送指针不为0时继续发送 */
	while(Cmd.SendPoint != 0){
		/* 7F判断 */
		if(sflag == 0){
			
			Cmd.SendPoint--;
			
			uart_send_char(fd, Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint]);

			/* 如果发送的数据中包含了0x7f则再发送一个0x7f */
			if(Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint] == 0x7F)
				sflag = 1;
		}
		else{
			sflag = 0;
			uart_send_char(fd, 0x7F);
		}
	}
	/* 发送指针为0时打上发送标记表示发送完成 */
	Cmd.SendFlag = 0;
}
















