#ifndef __MAIN_H
#define __MAIN_H

/**********************包含头文件*******************/
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

#include "common.h"
#include "uart.h"
#include "my_string.h"
#include "dht11.h"
#include "Y13R.h"
#include "PICC.h"
#include "timer.h"




/*********************全局变量*********************/
extern uint8_t g_rev_flag;	/* 接收数据完成标志位 */
extern PICC Picc;
extern CMD Cmd;

uint8_t g_rev_buf[64] = {0};
uint8_t g_send_buf[64] = {0};
uint8_t g_rev_len = 0;
uint8_t g_send_len =0;

/* 定时时间以及定时处理函数 */
G_Timer_Function g_dht11_deal_send_data_function;	/* dht11处理发送数据定时函数 */
G_Timer_Function g_dht11_timer_function;


/***********************函数声明********************/
void *dht11_rev_thread_function(void *arg);
void *y13r_rev_thread_function(void *arg);
void multiple_timers_function(int signo);

#endif