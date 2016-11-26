#include "main.h"

/******************************************************************************************************
* ��������:    main
* ��������:    argc:	��������
* 			   argv:	��������
* ��������ֵ:  0
* ����������   ��ں���
******************************************************************************************************/


/* ÿһ����������������һ���������߳��У�����һ����ʱ�����Թ̶���ʱ���ϱ��ɼ��������ݺͷ������¼� */
/* ÿһ���̶߳���ȫ���жϵ��޸ģ����������ͬ�����ƣ���Ȼ������⣬���е��̹߳���ȫ�ֱ��� */
int main(int argc, char const *argv[])
{
	int ret = 0;

	/* �̱߳��� */
	pthread_t dht11_thread;
	pthread_t y13r_thread;

	#if 1
	/* ���������߳� */
	ret = pthread_create(&dht11_thread, NULL, dht11_rev_thread_function, NULL);  
	if (ret != 0)
		printf("pthread_create error!\r\n");
	
	ret = pthread_create(&y13r_thread, NULL, y13r_rev_thread_function, NULL);  
	if (ret != 0)
		printf("pthread_create error!\r\n");
	#endif

	
	timer_init(3, 0, 0, 10000, multiple_timers_function);	/* ��ʼ��ʱ��ֵ5s,�Ժ���10ms���ٶȴ������ݣ��󶨶�ʱ���� */
	while(1){	/* ������߳��˳������߳�Ҳ���˳� */
		sleep(5);
	}
	
	
#if 0
	int uart_fd = 0;
	
	uint8_t *p_rev_str = NULL;	/* ���յ��ַ�����ת�����ַ��� */

	uart_fd = uart_open(uart_dev_name);
	if (uart_fd == FALSE){
		printf("open_uart error!\r\n");
		return -1;
	}
	ret = uart_set(uart_fd, 115200, 0, 8, 1, 'N');	/* ���ô��� */
	if (ret == FALSE){
		perror("uart_set:");
		goto err;
	}
	printf("waiting rev data....\r\n");
	while(1){
		bzero(rev_buf,sizeof(rev_buf));    /* ��մ��ڽ������� */
		rev_len = uart_rev_buf(uart_fd, rev_buf);
		if (g_rev_flag == 1){	/* �������һ֡������� */
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
* ��������:    multiple_timers_function
* ��������:    signo:	�ź�����
* ��������ֵ:  ��
* ����������   �ඨʱ��������
******************************************************************************************************/
void multiple_timers_function(int signo)
{
	switch (signo){  
        case SIGALRM:
			/* ����Լ��Ĵ��� */
			printf("nihao\r\n");
            break;  
   }  
}

/******************************************************************************************************
* ��������:    dht11_rev_thread_function
* ��������:    arg:	���ݵĲ���
* ��������ֵ:  ��
* ����������   dht11�̺߳���  ����ֻ�Ǵ����˽����̣߳����ڽ�����Ϊһֱ���ڣ����Է����̣߳����ݴ���
*			   �Լ����ͷ��ڶ�ʱ���С�
******************************************************************************************************/

void *dht11_rev_thread_function(void *arg)
{
	int ret = 0;
	int len = 0;
	int fd = 0;
	/* �����ڴ�ռ䣬����ʼ��Ϊ0 */
	struct DHT11_Data *dht11_data = (struct DHT11_Data *)calloc(1, sizeof(struct DHT11_Data));
	if (dht11_data == NULL)
		printf("dht11_data is NULL\r\n");
	
	fd = dht11_init();	/* dht11��ʼ�� */
	if (fd == FALSE)
		pthread_exit("dht11_init false, exit dht11_thread_function\r\n");	/* �����߳� */

	/* 5ms��ȡһ��ֵ */
	while(1){
		len = read(fd, dht11_data, sizeof(struct DHT11_Data) );
		if (len > 0){
			printf("temperature is %d  humidity is %d\r\n", dht11_data->temperature, dht11_data->humidity);
			usleep(5000);	/* ��ʱ5������ */
		}
	}
}


/******************************************************************************************************
* ��������:    y13r_rev_thread_function
* ��������:    arg:	���ݵĲ���
* ��������ֵ:  ��
* ����������   y13r�̺߳��� y13rģ���һֱ���ڽ���״̬�����Է���һ���߳��У�y13r�����ڴ������ݣ�
*			   ����û�ж�ʱ����
******************************************************************************************************/
void *y13r_rev_thread_function(void *arg)
{
	int fd = 0;
	uint8_t bTemp = 0;
	
	fd = y13r_init();
	
	/* ��ʼ������ */
	Cmd.SendFlag = 0;
	Cmd.ReceiveFlag = 0;
	Picc.Value = 0;
	/* ��������ģ����յ������� */
	while(1){
		usleep(100);	/* 100ms����һ�� */
		y13r_recv(fd);	/* �������� */
		bTemp = CommandProcess();
		if(bTemp == 0){		/* ������յ���������ȷ */
			printf("picc.uid is %u\r\n", Picc.UID);
			
			//�յ������ɹ�
			//�����￪ʼ��Ĳ���
			//�������������� Picc
			//����	==>Picc.UID
			//������==>Picc.Type
			//���	==>Picc.Value
		}
		else if(bTemp == 0xFF){
			//�޿�
		}
		
		else if(bTemp == 0xFE){
			//����
		}
		else if(bTemp == 0xFD){
			//��������
		}
	
	}
}



