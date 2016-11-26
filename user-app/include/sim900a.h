#ifndef _SIM900A_H_
#define _SIM900A_H_


/***********************************************************************************************
*
***********************************************头文件*******************************************
*
************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>    /* PPSIX 终端控制定义 */  
#include <string.h>  
#include <unistd.h>

#include "uart.h"
#include "common.h"
#include "cc936.h"          /* firecc936.c文件中：ff_convert()函数实现unicode与gbk互转功能 */








/***********************************************************************************************
*
***********************************************宏定义*******************************************
*
************************************************************************************************/

typedef enum{
    SIM900A_TRUE,
    SIM900A_FALSE,
    
}sim900a_res_e;

typedef enum
{
    SIM900A_NULL                = 0,
    SIM900A_CMD_SEND            = '\r',         
    SIM900A_DATA_SEND           = 0x1A,         //发送数据(ctrl + z)
    SIM900A_DATA_CANCLE         = 0x1B,         //发送数据(Esc)    
}sim900a_cmd_end_e;

//                  指令             正常返回
//本机号码          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //很多SIM卡默认都是没设置本机号码的，解决方法如下 http://www.multisilicon.com/blog/a21234642.html
//SIM营运商         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM卡状态         AT+CPIN?\r        +CPIN: READY   OK
//SIM卡信号强度     AT+CSQ\r          +CSQ: 8,0   OK

#define     sim900a_tx_printf(fd, fmt, ...)     uart_printf(fd, fmt,##__VA_ARGS__)     //printf格式发送命令（纯发送，不进行任何接收数据处理）
#define   SIM900A_DELAY(time)                               usleep(1000*time)                 //延时ms

/*************************** 电话 功能 ***************************/
#define  	SIM900A_HANGON(fd)				SIM900A_TX(fd,"ATA\r");								
#define  	SIM900A_HANGOFF(fd)				SIM900A_TX(fd,"ATH\r");	//挂断电话	








/***********************************************************************************************
*
***********************************************函数声明*******************************************
*
************************************************************************************************/


extern uint8_t sim900a_cmd(int fd, char *cmd, char *reply,uint32_t waittime );
extern uint8_t sim900a_cmd_check(int fd, char *reply);
extern uint8_t IsASSIC(char * str);
extern void sim900a_char2hex(char *hex, char ch);
extern void sim900a_gbk2ucs2hex(char * ucs2hex, char * gbk);

extern 	uint8_t 	sim900a_init			(int fd);															//初始化并检测模块
extern  uint8_t     sim900a_cnum            (int fd,char *num);                        //获取本机号码
extern  void        sim900a_call           	(int fd, char *num);                        //发起拨打电话（不管接不接通）
extern	uint8_t 	IsRing					(int fd, char *num);						//查询是否来电，并保存来电号码

/***************************  短信功能  ****************************/
extern  void        sim900a_sms             (int fd,char *num,char *smstext);          //发送短信（支持中英文,中文为GBK码）
extern  uint8_t *   sim900a_waitask         (int fd,uint8_t waitask_hook(void));       //等待有数据应答，返回接收缓冲区地址
extern  void        sim900a_gbk2ucs2        (char * ucs2,char * gbk);           

extern	uint8_t 	IsReceiveMS				(int fd);
extern	uint8_t 	readmessage				(int fd,uint8_t messadd,char *num,char *str);
extern	uint8_t 	hexuni2gbk				(int fd,char *hexuni,char *chgbk);

/*************************** GPRS 功能 ***************************/
extern  void        sim900a_gprs_init           (int fd);                                                 //GPRS初始化环境
extern  void        sim900a_gprs_tcp_link       (int fd,char *localport,char * serverip,char * serverport);    //TCP连接
extern  void        sim900a_gprs_udp_link       (int fd,char *localport,char * serverip,char * serverport);    //UDP连接
extern  void        sim900a_gprs_send           (int fd,char *str);                                            //发送数据
extern  void        sim900a_gprs_link_close     (int fd);                                                 //IP链接断开

#endif

