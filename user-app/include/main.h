#ifndef __MAIN_H
#define __MAIN_H

/**********************����ͷ�ļ�*******************/
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




/*********************ȫ�ֱ���*********************/
extern uint8_t g_rev_flag;	/* ����������ɱ�־λ */
extern PICC Picc;
extern CMD Cmd;

uint8_t g_rev_buf[64] = {0};
uint8_t g_send_buf[64] = {0};
uint8_t g_rev_len = 0;
uint8_t g_send_len =0;

/* ��ʱʱ���Լ���ʱ������ */
G_Timer_Function g_dht11_deal_send_data_function;	/* dht11���������ݶ�ʱ���� */
G_Timer_Function g_dht11_timer_function;


/***********************��������********************/
void *dht11_rev_thread_function(void *arg);
void *y13r_rev_thread_function(void *arg);
void multiple_timers_function(int signo);

#endif