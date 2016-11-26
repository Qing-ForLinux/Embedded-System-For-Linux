/****************************************************************************************************
*�ļ�����:    uart.c
*�ļ�����:	  ���ڲ���������
*��д��:	  ������
*��дʱ��:	  2016-9-2
*��������ͨ�Ÿ�ʽ:	֡ͷ(�����ֽ�:0xaa,0x55) + ֡����(һ���ֽڣ������ֶΣ���ʱ�����) + 
*					����(һ���ֽ�) + ���� + ֡β(�����ֽ�:0xff,0xff)
*					(�����У�飬����ֻ��������λ����)
*����:	0xaa 0x55	0x00	0x02 	0x01 0x02 	0xff 0xff
*˵��: ����ͨ�Ÿ�ʽ��Ϊ�˽�����ڽ������ݲ���������������ڴ������ݵķ�����ʱ������
*		����ֻ�õ���֡ͷ	����	���� 	֡β
*****************************************************************************************************/

#include "uart.h"



/* ȫ�ֱ��� */
uint8_t g_rev_flag = 0;	/* ����������ɱ�־λ */


/****************************************************************************************************
*��������:    uart_init
*��������:    void
*��������ֵ:  �ɹ�����SUCCEE��ʧ�ܷ���FALSE
*��������:    ���ڳ�ʼ��
*****************************************************************************************************/
int uart_init(void)
{
	return SUCCEE;
}

/****************************************************************************************************
*��������:    uart_open
*��������:    uart_dev_name(linux���洮���豸����)
*��������ֵ:  �򿪳ɹ�����fd��ʧ�ܷ���FALSE
*��������:    �򿪶�Ӧ�����豸���ҷ�����Ӧ���ļ�������
*****************************************************************************************************/
int uart_open(char *uart_dev_name)
{
	int fd; 

	fd = open(uart_dev_name, O_RDWR|O_NOCTTY|O_NDELAY); /*�Զ�д��ʽ�򿪴����豸*/
	if (FALSE == fd)  
	{  
		perror("Can't Open Serial Port");  
		return FALSE;  
	}  
	                                
	if (fcntl(fd, F_SETFL, 0) < 0)	/*�ָ�����Ϊ����״̬*/
	{  
		printf("fcntl failed!\n");  
		return FALSE;  
	}       
	else   
		printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));  
	   
	if (0 == isatty(STDIN_FILENO)) 	/*�����Ƿ�Ϊ�ն��豸*/
	{  
		printf("standard input is not a terminal device\n");  
		return(FALSE);  
	}  
	else    
		printf("isatty success!\n");  

	return fd;
}

/******************************************************************************************************
* ��������:    uart_set
* ��������:    fd  �����ļ�������
*              speed      �����ٶ�(9500����115200)
*              flow_ctrl  ����������(0��ʹ������,1ʹ�������,2ʹ��Ӳ����)
*              databits   ����λ   ȡֵΪ 7 ����8
*              stopbits   ֹͣλ   ȡֵΪ 1 ����2
*              parity     Ч������ ȡֵΪN,E,O
* ��������ֵ:  ��ȷ����ΪSUCCEE�����󷵻�ΪFALSE
* ����������   ���ô�������λ��ֹͣλ��Ч��λ
******************************************************************************************************/
int uart_set(int fd, int speed, int flow_ctrl, int databits, int stopbits, char parity)
{
	int i;
	int status;
	struct termios options;


	/*tcgetattr(fd,&options)�õ���fdָ��������ز������������Ǳ�����options,�ú���,�����Բ��������Ƿ���ȷ���ô����Ƿ���õȡ������óɹ�����������ֵΪ0��������ʧ�ܣ���������ֵΪ1.
	*/
	if  ( tcgetattr( fd, &options)  !=  0)
	{
		perror("set_uart error:");    
		return FALSE; 
	}

	//���ô������벨���ʺ����������
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

	options.c_cflag |= CLOCAL;	/*�޸Ŀ���ģʽ����֤���򲻻�ռ�ô���*/

	options.c_cflag |= CREAD;	/*�޸Ŀ���ģʽ��ʹ���ܹ��Ӵ����ж�ȡ��������*/

	options.c_lflag &= ~(ICANON | ECHO);	/* �رջ��� */

	switch(flow_ctrl)	/*��������������*/
	{
		case 0://��ʹ��������
			options.c_cflag &= ~CRTSCTS;
			break;   

		case 1://ʹ��Ӳ��������
			options.c_cflag |= CRTSCTS;
			break;
		case 2://ʹ�����������
			options.c_cflag |= IXON | IXOFF | IXANY;
			break;
		default:
			printf("Unsupported flow_ctrl\r\n");
			return FALSE;
	}
	
	options.c_cflag &= ~CSIZE; /*����������־λ*/
	switch (databits)	/*��������λ*/
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

	switch (parity) /*����У��λ*/
	{  
		case 'n':
		case 'N':	/*����żУ��λ*/
			options.c_cflag &= ~PARENB; 
			options.c_iflag &= ~INPCK;    
			break; 
		case 'o':  
		case 'O':	/*����Ϊ��У��*/
			options.c_cflag |= (PARODD | PARENB); 
			options.c_iflag |= INPCK;             
			break; 
		case 'e': 
		case 'E':	/*����ΪżУ��*/ 
			options.c_cflag |= PARENB;       
			options.c_cflag &= ~PARODD;       
			options.c_iflag |= INPCK;       
			break;
		default:  
			printf("Unsupported parity\r\n");   
			return FALSE; 
	} 
	
	switch (stopbits)	/*����ֹͣλ */
	{  
		case 1:   
			options.c_cflag &= ~CSTOPB;	/*1λֹͣλ*/
			break; 
		case 2:   
			options.c_cflag |= CSTOPB;	/*2λֹͣλ*/
			break;
		default:   
			printf("Unsupported stop bits\r\n"); 
			return FALSE;
	}


	options.c_oflag &= ~OPOST;	/*�޸����ģʽ��ԭʼ�������*/

	/*���õȴ�ʱ�����С�����ַ�*/
	options.c_cc[VTIME] = 0; /* ��ȡһ���ַ��ȴ�1*(1/10)s */  
	options.c_cc[VMIN] = 0; /* ��ȡ�ַ������ٸ���Ϊ1 */

	tcflush(fd, TCIFLUSH);	/*�����������������������ݣ����ǲ��ٶ�ȡ*/

	if (tcsetattr(fd,TCSANOW, &options) != 0)	/*�������� (���޸ĺ��termios�������õ������У�*/
	{
		perror("com set error!/n");  
		return FALSE; 
	}
	return SUCCEE; 
}


/******************************************************************************************************
* ��������:    uart_rev_buf
* ��������:    fd  �����ļ�������
*              rev_buf     :���մ��������ݴ���rcv_buf��������
* ��������ֵ:  ��ȷ����Ϊlen�����󷵻�ΪFALSE
* ����������   ���մ�������(����һ֡������)  ÿ�ζ��Ƿ�Ϊ8���ֽڽ��� �ᱻ�ض�
* ˵��:	���յ����ݴ洢��rev_buf�����У�������һ��һ�����ַ� 
* 		���������Ҫ��ӡ�Ļ���Ҫ�ȵ���
******************************************************************************************************/
int uart_rev_buf(int fd, uint8_t *rev_buf)
{

	/* 0xaa 0x55 +���� +���� 0xff 0xff */
	uint8_t ret = 0;
	uint8_t sum_num = 0;	/* ���յ��������� */
    uint8_t len = 0;	/* �������ݵĳ��� */
    uint8_t frame_header1 = 0xaa;	/* ֡ͷ1 */
    uint8_t frame_header2 = 0x55;	/* ֡ͷ2 */

    uint8_t frame_tail1 = 0;	/* ֡β1 */
    uint8_t frame_tail2 = 0;	/* ֡β2 */

    uint8_t data = 0;	/* �������� */

    fd_set fs_read;
    int fs_sel = 0;
    struct timeval time;


    time.tv_sec = 5;	/*���ó�ʱʱ��*/
    time.tv_usec = 0;

    FD_ZERO(&fs_read);
    FD_SET(fd, &fs_read);   
    fs_sel = select(fd+1, &fs_read, NULL, NULL, NULL);	/*ʹ��selectʵ�ִ��ڵĶ�·ͨ��*/ 

    if (fs_sel) {
	    if ( (ret = read(fd, &data, 1)) > 0 ){	/* ��ȡһ���ֽ� Ѱ��֡ͷ */ 
	    	if ( data == frame_header1 ){	/* ����ǵ�һ��֡ͷ */
	    		if ( (ret = read(fd, &data, 1)) > 0 ){	/* ��ȡһ���ֽ� Ѱ��֡ͷ */
	    			if ( data == frame_header2){		/* ������յ��ڶ���֡ͷ */
	    				if ( (ret = read(fd, &len, 1)) > 0 ){	/* ���ݵĳ��� */
    						while (ret = read(fd, &data, 1) > 0){	/* ����һ���ַ� */
    							rev_buf[sum_num] = data;	/* �洢�ַ� */
    							sum_num++;	/* ���յĸ���++ */
    							if (sum_num >= len){
    								rev_buf[sum_num] = '\0';	/* ����ַ��������� */
    								g_rev_flag = 1;	/* ����������ɱ�־λ��λ */
    								return len;	/* ���ؽ��յ����� */
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
* ��������:    uart_send_buf
* ��������:    fd  �����ļ�������
*              send_buf ��Ŵ��ڷ�������
*              data_len һ֡���ݵĸ���
* ��������ֵ:  ��ȷ����Ϊlen�����󷵻�ΪFALSE
* ����������   ���ʹ�������
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
* ��������:    uart_send_char
* ��������:    fd  �����ļ�������
* 			   data Ҫ���͵��ַ�
* ��������ֵ:  ��ȷ����Ϊlen�����󷵻�ΪFALSE
* ����������   ���ʹ��ڵ�������
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
* ��������:    uart_rec_char
* ��������:    fd  �����ļ�������
* ��������ֵ:  ��ȷ����Ϊdata�����󷵻�ΪFALSE
* ����������   ���մ��ڵ�������
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
* ��������:    uart_close
* ��������:    fd  �����ļ�������
* ��������ֵ:  void
* ��������:    �رմ���
******************************************************************************************************/
 
void uart_close(int fd)
{
    close(fd);
}



/**********************************************************************************************
 * ��������:    uart_printf
 * ��������:    -USARTx ����ͨ��������ֻ�õ��˴���2����USART2
 *		        -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			    -...    ��������
 * ��������ֵ:  void
 * ��������:    �ⲿ���� ��ʽ�������������C���е�printf��������û���õ�C��
 *              ����Ӧ��  uart_printf( fd, "\r\n this is a demo \r\n" );
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

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					uart_send_char(fd, 0x0d);
					Data ++;
					break;

				case 'n':							          //���з�
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
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
					uart_send_buf(fd, s, strlen(s));
					break;

				case 'd':										//ʮ����
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
