#ifndef __UART_H
#define __UART_H

/***********************************************************************************************
*
***********************************************头文件*******************************************
*
************************************************************************************************/
#include <stdio.h>      /*标准输入输出定义*/  
#include <stdlib.h>     /*标准函数库定义*/  
#include <unistd.h>     /*Unix 标准函数定义*/  
#include <sys/types.h>   
#include <sys/stat.h>     
#include <fcntl.h>      /*文件控制定义*/  
#include <termios.h>    /*PPSIX 终端控制定义*/  
#include <errno.h>      /*错误号定义*/  
#include <string.h>  
#include <stdarg.h>
#include <sys/time.h>
#include <sys/select.h>

#include "common.h"








/***********************************************************************************************
*
***********************************************函数声明*******************************************
*
************************************************************************************************/
extern int uart_init(void);
extern int uart_open(char *uart_dev_name);
extern int uart_set(int fd, int speed, int flow_ctrl, int databits, int stopbits, char parity);
extern int uart_rev_buf(int fd, uint8_t *rev_buf);
extern int uart_send_buf(int fd, const uint8_t *send_buf, int data_len);
extern int uart_send_char(int fd,  const uint8_t data);
extern uint8_t uart_rec_char(int fd);
extern void uart_printf(int fd, uint8_t *Data,...);
extern void uart_close(int fd);





#endif