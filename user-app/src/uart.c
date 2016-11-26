/****************************************************************************************************
*文件名称:    uart.c
*文件描述:	  串口操作函数集
*编写人:	  杨鹏飞
*编写时间:	  2016-9-2
*串口数据通信格式:	帧头(两个字节:0xaa,0x55) + 帧类型(一个字节，保留字段，暂时不添加) + 
*					长度(一个字节) + 数据 + 帧尾(两个字节:0xff,0xff)
*					(不添加校验，长度只包含数据位个数)
*例如:	0xaa 0x55	0x00	0x02 	0x01 0x02 	0xff 0xff
*说明: 数据通信格式是为了解决串口接收数据不完整的情况，对于串口数据的发送暂时不考虑
*		现在只用到了帧头	长度	数据 	帧尾
*****************************************************************************************************/

#include "uart.h"



/* 全局变量 */
uint8_t g_rev_flag = 0;	/* 接收数据完成标志位 */


/****************************************************************************************************
*函数名称:    uart_init
*函数参数:    void
*函数返回值:  成功返回SUCCEE，失败返回FALSE
*函数描述:    串口初始化
*****************************************************************************************************/
int uart_init(void)
{
	return SUCCEE;
}

/****************************************************************************************************
*函数名称:    uart_open
*函数参数:    uart_dev_name(linux下面串口设备名称)
*函数返回值:  打开成功返回fd，失败返回FALSE
*函数描述:    打开对应串口设备并且返回相应的文件描述符
*****************************************************************************************************/
int uart_open(char *uart_dev_name)
{
	int fd; 

	fd = open(uart_dev_name, O_RDWR|O_NOCTTY|O_NDELAY); /*以读写方式打开串口设备*/
	if (FALSE == fd)  
	{  
		perror("Can't Open Serial Port");  
		return FALSE;  
	}  
	                                
	if (fcntl(fd, F_SETFL, 0) < 0)	/*恢复串口为阻塞状态*/
	{  
		printf("fcntl failed!\n");  
		return FALSE;  
	}       
	else   
		printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));  
	   
	if (0 == isatty(STDIN_FILENO)) 	/*测试是否为终端设备*/
	{  
		printf("standard input is not a terminal device\n");  
		return(FALSE);  
	}  
	else    
		printf("isatty success!\n");  

	return fd;
}

/******************************************************************************************************
* 函数名称:    uart_set
* 函数参数:    fd  串口文件描述符
*              speed      串口速度(9500或者115200)
*              flow_ctrl  数据流控制(0不使用流控,1使用软件流,2使用硬件流)
*              databits   数据位   取值为 7 或者8
*              stopbits   停止位   取值为 1 或者2
*              parity     效验类型 取值为N,E,O
* 函数返回值:  正确返回为SUCCEE，错误返回为FALSE
* 函数描述：   设置串口数据位，停止位和效验位
******************************************************************************************************/
int uart_set(int fd, int speed, int flow_ctrl, int databits, int stopbits, char parity)
{
	int i;
	int status;
	struct termios options;


	/*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数,还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.
	*/
	if  ( tcgetattr( fd, &options)  !=  0)
	{
		perror("set_uart error:");    
		return FALSE; 
	}

	//设置串口输入波特率和输出波特率
	switch (speed)
    {
    case 9600:
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        break;
    case 115200:
        cfsetispeed(&options, B115200);
        cfsetospeed(&options, B115200);
        break;
    default:
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        break;
    }  

	options.c_cflag |= CLOCAL;	/*修改控制模式，保证程序不会占用串口*/

	options.c_cflag |= CREAD;	/*修改控制模式，使得能够从串口中读取输入数据*/

	options.c_lflag &= ~(ICANON | ECHO);	/* 关闭回显 */

	switch(flow_ctrl)	/*设置数据流控制*/
	{
		case 0://不使用流控制
			options.c_cflag &= ~CRTSCTS;
			break;   

		case 1://使用硬件流控制
			options.c_cflag |= CRTSCTS;
			break;
		case 2://使用软件流控制
			options.c_cflag |= IXON | IXOFF | IXANY;
			break;
		default:
			printf("Unsupported flow_ctrl\r\n");
			return FALSE;
	}
	
	options.c_cflag &= ~CSIZE; /*屏蔽其他标志位*/
	switch (databits)	/*设置数据位*/
	{  
		case 5:
			options.c_cflag |= CS5;
			break;
		case 6    :
			options.c_cflag |= CS6;
			break;
		case 7    :    
			options.c_cflag |= CS7;
			break;
		case 8:    
			options.c_cflag |= CS8;
			break;  
		default:   
			printf("Unsupported data size\r\n");
			return FALSE; 
	}

	switch (parity) /*设置校验位*/
	{  
		case 'n':
		case 'N':	/*无奇偶校验位*/
			options.c_cflag &= ~PARENB; 
			options.c_iflag &= ~INPCK;    
			break; 
		case 'o':  
		case 'O':	/*设置为奇校验*/
			options.c_cflag |= (PARODD | PARENB); 
			options.c_iflag |= INPCK;             
			break; 
		case 'e': 
		case 'E':	/*设置为偶校验*/ 
			options.c_cflag |= PARENB;       
			options.c_cflag &= ~PARODD;       
			options.c_iflag |= INPCK;       
			break;
		default:  
			printf("Unsupported parity\r\n");   
			return FALSE; 
	} 
	
	switch (stopbits)	/*设置停止位 */
	{  
		case 1:   
			options.c_cflag &= ~CSTOPB;	/*1位停止位*/
			break; 
		case 2:   
			options.c_cflag |= CSTOPB;	/*2位停止位*/
			break;
		default:   
			printf("Unsupported stop bits\r\n"); 
			return FALSE;
	}


	options.c_oflag &= ~OPOST;	/*修改输出模式，原始数据输出*/

	/*设置等待时间和最小接收字符*/
	options.c_cc[VTIME] = 0; /* 读取一个字符等待1*(1/10)s */  
	options.c_cc[VMIN] = 0; /* 读取字符的最少个数为1 */

	tcflush(fd, TCIFLUSH);	/*如果发生数据溢出，接收数据，但是不再读取*/

	if (tcsetattr(fd,TCSANOW, &options) != 0)	/*激活配置 (将修改后的termios数据设置到串口中）*/
	{
		perror("com set error!/n");  
		return FALSE; 
	}
	return SUCCEE; 
}


/******************************************************************************************************
* 函数名称:    uart_rev_buf
* 函数参数:    fd  串口文件描述符
*              rev_buf     :接收串口中数据存入rcv_buf缓冲区中
* 函数返回值:  正确返回为len，错误返回为FALSE
* 函数描述：   接收串口数据(接收一帧的数据)  每次都是分为8个字节接收 会被截断
* 说明:	接收的数据存储在rev_buf数组中，由于是一个一个的字符 
* 		所以如果需要打印的话需要先调用
******************************************************************************************************/
int uart_rev_buf(int fd, uint8_t *rev_buf)
{

	/* 0xaa 0x55 +长度 +数据 0xff 0xff */
	uint8_t ret = 0;
	uint8_t sum_num = 0;	/* 接收的数据总数 */
    uint8_t len = 0;	/* 接收数据的长度 */
    uint8_t frame_header1 = 0xaa;	/* 帧头1 */
    uint8_t frame_header2 = 0x55;	/* 帧头2 */

    uint8_t frame_tail1 = 0;	/* 帧尾1 */
    uint8_t frame_tail2 = 0;	/* 帧尾2 */

    uint8_t data = 0;	/* 接收数据 */

    fd_set fs_read;
    int fs_sel = 0;
    struct timeval time;


    time.tv_sec = 5;	/*设置超时时间*/
    time.tv_usec = 0;

    FD_ZERO(&fs_read);
    FD_SET(fd, &fs_read);   
    fs_sel = select(fd+1, &fs_read, NULL, NULL, NULL);	/*使用select实现串口的多路通信*/ 

    if (fs_sel) {
	    if ( (ret = read(fd, &data, 1)) > 0 ){	/* 读取一个字节 寻找帧头 */ 
	    	if ( data == frame_header1 ){	/* 如果是第一个帧头 */
	    		if ( (ret = read(fd, &data, 1)) > 0 ){	/* 读取一个字节 寻找帧头 */
	    			if ( data == frame_header2){		/* 如果接收到第二个帧头 */
	    				if ( (ret = read(fd, &len, 1)) > 0 ){	/* 数据的长度 */
    						while (ret = read(fd, &data, 1) > 0){	/* 接收一个字符 */
    							rev_buf[sum_num] = data;	/* 存储字符 */
    							sum_num++;	/* 接收的个数++ */
    							if (sum_num >= len){
    								rev_buf[sum_num] = '\0';	/* 添加字符串结束符 */
    								g_rev_flag = 1;	/* 接收数据完成标志位置位 */
    								return len;	/* 返回接收的数组 */
    							}
    						}
	    				}
	    			}
	    		}
	    	}
   		}
   	}
   	else
   		return FALSE;
}

/******************************************************************************************************
* 函数名称:    uart_send_buf
* 函数参数:    fd  串口文件描述符
*              send_buf 存放串口发送数据
*              data_len 一帧数据的个数
* 函数返回值:  正确返回为len，错误返回为FALSE
* 函数描述：   发送串口数据
******************************************************************************************************/
int uart_send_buf(int fd, const uint8_t *send_buf, int data_len)
{
    int len = 0;
   
	len = write(fd, send_buf, data_len);
	if (len == data_len )
	{
		return len;
	}     
	else   
	{
		tcflush(fd,TCOFLUSH);
		return FALSE;
	}
}



/******************************************************************************************************
* 函数名称:    uart_send_char
* 函数参数:    fd  串口文件描述符
* 			   data 要发送的字符
* 函数返回值:  正确返回为len，错误返回为FALSE
* 函数描述：   发送串口单个数据
******************************************************************************************************/
int uart_send_char(int fd, const uint8_t data)
{
    int len = 0;
   
	len = write(fd, &data, 1);
	if (len > 0 )
	{
		return len;
	}     
	else  
		return FALSE;
}


/******************************************************************************************************
* 函数名称:    uart_rec_char
* 函数参数:    fd  串口文件描述符
* 函数返回值:  正确返回为data，错误返回为FALSE
* 函数描述：   接收串口单个数据
******************************************************************************************************/
uint8_t uart_rec_char(int fd)
{
    int len = 0;
    uint8_t data;
   
	len = read(fd, &data, 1);
	if ( len > 0)
		return data;
	else
		return FALSE;
}


/******************************************************************************************************
* 函数名称:    uart_close
* 函数参数:    fd  串口文件描述符
* 函数返回值:  void
* 函数描述:    关闭串口
******************************************************************************************************/
 
void uart_close(int fd)
{
    close(fd);
}



/**********************************************************************************************
 * 函数名称:    uart_printf
 * 函数参数:    -USARTx 串口通道，这里只用到了串口2，即USART2
 *		        -Data   要发送到串口的内容的指针
 *			    -...    其他参数
 * 函数返回值:  void
 * 函数描述:    外部调用 格式化输出，类似于C库中的printf，但这里没有用到C库
 *              典型应用  uart_printf( fd, "\r\n this is a demo \r\n" );
 *            		      uart_printf( fd, "\r\n %d \r\n", i );
 *            		      uart_printf( fd, "\r\n %s \r\n", j );
 *********************************************************************************************/
void uart_printf(int fd, uint8_t *Data,...)
{
	const char *s;
	int d;   
	char buf[16];

	va_list ap;
	va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					uart_send_char(fd, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					uart_send_char(fd, 0x0d);	
					Data ++;
					break;

				default:
					Data ++;
					break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
					uart_send_buf(fd, s, strlen(s));
					break;

				case 'd':										//十进制
					d = va_arg(ap, int);
					sprintf(buf, "%d", d);
					s = buf;
					uart_send_buf(fd, s, strlen(s));
					Data++;
					break;
				default:
					Data++;
					break;
			}		 
		} /* end of else if */
		uart_send_char(fd, *Data++);
	}
}
