
#include "timer.h"

/* 说明:settimer工作机制是，先对it_value倒计时，当it_value为零时触发信号，然后重置为it_interval，继续对it_value倒计时，一直这样循环下去。 */

/******************************************************************************************************
* 函数名称:    timer_init
* 函数参数:    value_tv_sec:		开始延时的秒速
*			   value_tv_usec:	开始延时的微妙数
*			   interval_tv_sec:	实际延时的秒速
*			   interval_tv_usec:	实际延时的微妙数
*			   function:	定时处理函数
* 函数返回值:  正确返回为SUCCESS
* 函数描述：   定时器处理函数初始化
******************************************************************************************************/

int timer_init(uint32_t value_tv_sec, uint32_t value_tv_usec, uint32_t interval_tv_sec, uint32_t interval_tv_usec, void *function(int))
{
	int ret = 0;
	
	struct itimerval tick;	/* 初始化结构体 */
	signal(SIGALRM, (sighandler_t)function);	/* 初始化信号处理函数 */
	
	memset(&tick, 0, sizeof(tick));
	
	/* 填充定时时间 */
    tick.it_value.tv_sec = value_tv_sec;	/* 开始延时的秒速 */
    tick.it_value.tv_usec = value_tv_usec;	/* 开始延时的微妙数 */
    tick.it_interval.tv_sec = interval_tv_sec;	/* 实际延时的秒速 */
    tick.it_interval.tv_usec = interval_tv_usec;	/* 实际延时的微妙数 */
	/* 注意:setitimer在一个进程中只能注册一个 */
    ret = setitimer(ITIMER_REAL, &tick, NULL);	/* 注册定时器 */
	if (ret != 0){
		printf("setitimer is faild!\r\n");
		return FALSE;
	}
	return SUCCEE;
		
}


/******************************************************************************************************
* 函数名称:    timer_cancle
* 函数参数:    void
* 函数返回值:  正确返回为SUCCESS,失败返回FALSE
* 函数描述：   定时器销毁函数
******************************************************************************************************/

int timer_cancle(void)
{
	int ret = 0;
	
	struct itimerval tick;	/* 初始化结构体 */
	//signal(SIGALRM, (sighandler_t)function);	/* 初始化信号处理函数 */
	
	memset(&tick, 0, sizeof(tick));
	
	/* 填充定时时间 */
    tick.it_value.tv_sec = 0;	/* 开始延时的秒速 */
    tick.it_value.tv_usec = 0;	/* 开始延时的微妙数 */
    tick.it_interval.tv_sec = 0;	/* 实际延时的秒速 */
    tick.it_interval.tv_usec = 0;	/* 实际延时的微妙数 */
	/* 注意:setitimer在一个进程中只能注册一个 */
    ret = setitimer(ITIMER_REAL, &tick, NULL);	/* 注册定时器 */
	if (ret != 0){
		printf("timer_cancle is faild!\r\n");
		return FALSE;
	}
	return SUCCEE;
}

