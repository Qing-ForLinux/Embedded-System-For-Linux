#ifndef _SIM900A_H_
#define _SIM900A_H_


/***********************************************************************************************
*
***********************************************ͷ�ļ�*******************************************
*
************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>    /* PPSIX �ն˿��ƶ��� */  
#include <string.h>  
#include <unistd.h>

#include "uart.h"
#include "common.h"
#include "cc936.h"          /* firecc936.c�ļ��У�ff_convert()����ʵ��unicode��gbk��ת���� */








/***********************************************************************************************
*
***********************************************�궨��*******************************************
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
    SIM900A_DATA_SEND           = 0x1A,         //��������(ctrl + z)
    SIM900A_DATA_CANCLE         = 0x1B,         //��������(Esc)    
}sim900a_cmd_end_e;

//                  ָ��             ��������
//��������          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //�ܶ�SIM��Ĭ�϶���û���ñ�������ģ������������ http://www.multisilicon.com/blog/a21234642.html
//SIMӪ����         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM��״̬         AT+CPIN?\r        +CPIN: READY   OK
//SIM���ź�ǿ��     AT+CSQ\r          +CSQ: 8,0   OK

#define     sim900a_tx_printf(fd, fmt, ...)     uart_printf(fd, fmt,##__VA_ARGS__)     //printf��ʽ������������ͣ��������κν������ݴ���
#define   SIM900A_DELAY(time)                               usleep(1000*time)                 //��ʱms

/*************************** �绰 ���� ***************************/
#define  	SIM900A_HANGON(fd)				SIM900A_TX(fd,"ATA\r");								
#define  	SIM900A_HANGOFF(fd)				SIM900A_TX(fd,"ATH\r");	//�Ҷϵ绰	








/***********************************************************************************************
*
***********************************************��������*******************************************
*
************************************************************************************************/


extern uint8_t sim900a_cmd(int fd, char *cmd, char *reply,uint32_t waittime );
extern uint8_t sim900a_cmd_check(int fd, char *reply);
extern uint8_t IsASSIC(char * str);
extern void sim900a_char2hex(char *hex, char ch);
extern void sim900a_gbk2ucs2hex(char * ucs2hex, char * gbk);

extern 	uint8_t 	sim900a_init			(int fd);															//��ʼ�������ģ��
extern  uint8_t     sim900a_cnum            (int fd,char *num);                        //��ȡ��������
extern  void        sim900a_call           	(int fd, char *num);                        //���𲦴�绰�����ܽӲ���ͨ��
extern	uint8_t 	IsRing					(int fd, char *num);						//��ѯ�Ƿ����磬�������������

/***************************  ���Ź���  ****************************/
extern  void        sim900a_sms             (int fd,char *num,char *smstext);          //���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
extern  uint8_t *   sim900a_waitask         (int fd,uint8_t waitask_hook(void));       //�ȴ�������Ӧ�𣬷��ؽ��ջ�������ַ
extern  void        sim900a_gbk2ucs2        (char * ucs2,char * gbk);           

extern	uint8_t 	IsReceiveMS				(int fd);
extern	uint8_t 	readmessage				(int fd,uint8_t messadd,char *num,char *str);
extern	uint8_t 	hexuni2gbk				(int fd,char *hexuni,char *chgbk);

/*************************** GPRS ���� ***************************/
extern  void        sim900a_gprs_init           (int fd);                                                 //GPRS��ʼ������
extern  void        sim900a_gprs_tcp_link       (int fd,char *localport,char * serverip,char * serverport);    //TCP����
extern  void        sim900a_gprs_udp_link       (int fd,char *localport,char * serverip,char * serverport);    //UDP����
extern  void        sim900a_gprs_send           (int fd,char *str);                                            //��������
extern  void        sim900a_gprs_link_close     (int fd);                                                 //IP���ӶϿ�

#endif

