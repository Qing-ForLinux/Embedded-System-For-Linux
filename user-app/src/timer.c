
#include "timer.h"

/* ˵��:settimer���������ǣ��ȶ�it_value����ʱ����it_valueΪ��ʱ�����źţ�Ȼ������Ϊit_interval��������it_value����ʱ��һֱ����ѭ����ȥ�� */

/******************************************************************************************************
* ��������:    timer_init
* ��������:    value_tv_sec:		��ʼ��ʱ������
*			   value_tv_usec:	��ʼ��ʱ��΢����
*			   interval_tv_sec:	ʵ����ʱ������
*			   interval_tv_usec:	ʵ����ʱ��΢����
*			   function:	��ʱ������
* ��������ֵ:  ��ȷ����ΪSUCCESS
* ����������   ��ʱ����������ʼ��
******************************************************************************************************/

int timer_init(uint32_t value_tv_sec, uint32_t value_tv_usec, uint32_t interval_tv_sec, uint32_t interval_tv_usec, void *function(int))
{
	int ret = 0;
	
	struct itimerval tick;	/* ��ʼ���ṹ�� */
	signal(SIGALRM, (sighandler_t)function);	/* ��ʼ���źŴ����� */
	
	memset(&tick, 0, sizeof(tick));
	
	/* ��䶨ʱʱ�� */
    tick.it_value.tv_sec = value_tv_sec;	/* ��ʼ��ʱ������ */
    tick.it_value.tv_usec = value_tv_usec;	/* ��ʼ��ʱ��΢���� */
    tick.it_interval.tv_sec = interval_tv_sec;	/* ʵ����ʱ������ */
    tick.it_interval.tv_usec = interval_tv_usec;	/* ʵ����ʱ��΢���� */
	/* ע��:setitimer��һ��������ֻ��ע��һ�� */
    ret = setitimer(ITIMER_REAL, &tick, NULL);	/* ע�ᶨʱ�� */
	if (ret != 0){
		printf("setitimer is faild!\r\n");
		return FALSE;
	}
	return SUCCEE;
		
}


/******************************************************************************************************
* ��������:    timer_cancle
* ��������:    void
* ��������ֵ:  ��ȷ����ΪSUCCESS,ʧ�ܷ���FALSE
* ����������   ��ʱ�����ٺ���
******************************************************************************************************/

int timer_cancle(void)
{
	int ret = 0;
	
	struct itimerval tick;	/* ��ʼ���ṹ�� */
	//signal(SIGALRM, (sighandler_t)function);	/* ��ʼ���źŴ����� */
	
	memset(&tick, 0, sizeof(tick));
	
	/* ��䶨ʱʱ�� */
    tick.it_value.tv_sec = 0;	/* ��ʼ��ʱ������ */
    tick.it_value.tv_usec = 0;	/* ��ʼ��ʱ��΢���� */
    tick.it_interval.tv_sec = 0;	/* ʵ����ʱ������ */
    tick.it_interval.tv_usec = 0;	/* ʵ����ʱ��΢���� */
	/* ע��:setitimer��һ��������ֻ��ע��һ�� */
    ret = setitimer(ITIMER_REAL, &tick, NULL);	/* ע�ᶨʱ�� */
	if (ret != 0){
		printf("timer_cancle is faild!\r\n");
		return FALSE;
	}
	return SUCCEE;
}

