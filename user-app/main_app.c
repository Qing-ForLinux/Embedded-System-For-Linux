#include "main.h"

/******************************************************************************************************
* 函数名称:    main
* 函数参数:    argc:	参数个数
* 			   argv:	参数名称
* 函数返回值:  0
* 函数描述：   入口函数
******************************************************************************************************/


/* 每一个传感器都运行在一个单独的线程中，启动一个定时器，以固定的时间上报采集到的数据和发生的事件 */
/* 每一个线程对于全局中断的修改，都必须采用同步机制，不然会出问题，所有的线程共享全局变量 */
int main(int argc, char const *argv[])
{
	int ret = 0;

	/* 线程变量 */
	pthread_t dht11_thread;
	pthread_t y13r_thread;

	#if 1
	/* 创建两个线程 */
	ret = pthread_create(&dht11_thread, NULL, dht11_rev_thread_function, NULL);  
	if (ret != 0)
		printf("pthread_create error!\r\n");
	
	ret = pthread_create(&y13r_thread, NULL, y13r_rev_thread_function, NULL);  
	if (ret != 0)
		printf("pthread_create error!\r\n");
	#endif

	
	timer_init(3, 0, 0, 10000, multiple_timers_function);	/* 初始定时器值5s,以后按照10ms的速度传输数据，绑定定时函数 */
	while(1){	/* 如果主线程退出，子线程也会退出 */
		sleep(5);
	}
	
	
#if 0
	int uart_fd = 0;
	
	uint8_t *p_rev_str = NULL;	/* 接收的字符数组转换成字符串 */

	uart_fd = uart_open(uart_dev_name);
	if (uart_fd == FALSE){
		printf("open_uart error!\r\n");
		return -1;
	}
	ret = uart_set(uart_fd, 115200, 0, 8, 1, 'N');	/* 设置串口 */
	if (ret == FALSE){
		perror("uart_set:");
		goto err;
	}
	printf("waiting rev data....\r\n");
	while(1){
		bzero(rev_buf,sizeof(rev_buf));    /* 清空串口接收数组 */
		rev_len = uart_rev_buf(uart_fd, rev_buf);
		if (g_rev_flag == 1){	/* 如果接收一帧数据完成 */
	    	p_rev_str = char_to_string(rev_buf);
	    	g_rev_flag = 0;
	    	if (p_rev_str != NULL)
	    		printf("rev_buf is %s\r\n", p_rev_str);
	    }
	}


err:
	close(uart_fd);
	return -1;
#endif
	
	return 0;
}


/******************************************************************************************************
* 函数名称:    multiple_timers_function
* 函数参数:    signo:	信号类型
* 函数返回值:  无
* 函数描述：   多定时器处理函数
******************************************************************************************************/
void multiple_timers_function(int signo)
{
	switch (signo){  
        case SIGALRM:
			/* 添加自己的代码 */
			printf("nihao\r\n");
            break;  
   }  
}

/******************************************************************************************************
* 函数名称:    dht11_rev_thread_function
* 函数参数:    arg:	传递的参数
* 函数返回值:  无
* 函数描述：   dht11线程函数  这里只是创建了接收线程，由于接收行为一直存在，所以放在线程，数据处理
*			   以及发送放在定时器中。
******************************************************************************************************/

void *dht11_rev_thread_function(void *arg)
{
	int ret = 0;
	int len = 0;
	int fd = 0;
	/* 分配内存空间，并初始化为0 */
	struct DHT11_Data *dht11_data = (struct DHT11_Data *)calloc(1, sizeof(struct DHT11_Data));
	if (dht11_data == NULL)
		printf("dht11_data is NULL\r\n");
	
	fd = dht11_init();	/* dht11初始化 */
	if (fd == FALSE)
		pthread_exit("dht11_init false, exit dht11_thread_function\r\n");	/* 结束线程 */

	/* 5ms读取一次值 */
	while(1){
		len = read(fd, dht11_data, sizeof(struct DHT11_Data) );
		if (len > 0){
			printf("temperature is %d  humidity is %d\r\n", dht11_data->temperature, dht11_data->humidity);
			usleep(5000);	/* 延时5个毫秒 */
		}
	}
}


/******************************************************************************************************
* 函数名称:    y13r_rev_thread_function
* 函数参数:    arg:	传递的参数
* 函数返回值:  无
* 函数描述：   y13r线程函数 y13r模块会一直处于接收状态，所以发在一个线程中，y13r不存在处理数据，
*			   所以没有定时任务
******************************************************************************************************/
void *y13r_rev_thread_function(void *arg)
{
	int fd = 0;
	uint8_t bTemp = 0;
	
	fd = y13r_init();
	
	/* 初始化变量 */
	Cmd.SendFlag = 0;
	Cmd.ReceiveFlag = 0;
	Picc.Value = 0;
	/* 持续监听模块接收到的数据 */
	while(1){
		usleep(100);	/* 100ms接收一次 */
		y13r_recv(fd);	/* 接收数据 */
		bTemp = CommandProcess();
		if(bTemp == 0){		/* 如果接收到的数据正确 */
			printf("picc.uid is %u\r\n", Picc.UID);
			
			//收到操作成功
			//在这里开始你的操作
			//所有有用数据在 Picc
			//卡号	==>Picc.UID
			//卡类型==>Picc.Type
			//余额	==>Picc.Value
		}
		else if(bTemp == 0xFF){
			//无卡
		}
		
		else if(bTemp == 0xFE){
			//错误
		}
		else if(bTemp == 0xFD){
			//参数错误
		}
	
	}
}



