#include "sim900a.h"


static uint8_t __g_MaxMessAdd=50;

/****************************************************************************************************
*��������:    sim900a_cmd
*��������:    fd        �ļ�������
*             cmd       �����ַ���
*             replay    ���͵�����
*             waittime  �ȴ�ʱ��
*��������ֵ:  ��ȷ����0    ���󷵻�1
*��������:    ��SIM900A����ָ��
*****************************************************************************************************/
uint8_t sim900a_cmd(int fd, char *cmd, char *reply, uint32_t waittime )
{    
	tcflush(fd, TCIOFLUSH);                 //����˽��ջ���������
                   
    uart_send_buf(fd, cmd, strlen(cmd));    /* �������� */

    if(reply == 0)                     //����Ҫ��������
    {
        return SIM900A_TRUE;
    }
    
    SIM900A_DELAY(waittime);                 //��ʱ
    
    return sim900a_cmd_check(fd, reply);    //�Խ������ݽ��д���
}

/****************************************************************************************************
*��������:    sim900a_cmd_check
*��������:    fd        �ļ�������
*             replay    ���յ�����
*��������ֵ:  ��ȷ����0    ���󷵻�1
*��������:    ��SIM900A��ȡ��������
*****************************************************************************************************/
uint8_t sim900a_cmd_check(int fd,char *reply)
{
    uint8_t len = 0;
    uint8_t n = 0;
    uint8_t off = 0;
    char *redata  = (char *) malloc( 64 );    /* ���ﲻ�ܶ�������� ��Ϊ������Ϊ���� ����������ĳ����е���++ */
    if ( redata == NULL ){
    	return FALSE;
    }

    len = uart_rev_buf( fd, redata );   //��������
    
    n = 0;
    off = 0;
    while((n + off)<len)
    {
        if(reply[n] == 0)                 //����Ϊ�ջ��߱Ƚ����
        {
            return SIM900A_TRUE;
        }
        
        if(redata[ n + off]== reply[n])
        {
            n++;                 //�ƶ�����һ����������
        }
        else
        {
            off++;              //������һ��ƥ��
            n=0;                //����
        }
        //n++;
    }

    if(reply[n]==0)   //�պ�ƥ�����
    {
        return SIM900A_TRUE;
    }
    free(redata);
    return SIM900A_FALSE;       //����ѭ����ʾ�Ƚ���Ϻ�û����ͬ�����ݣ��������
}

/****************************************************************************************************
*��������:    sim900a_waitask
*��������:    fd           �ļ�������
*             waitask_hook ��֪
*��������ֵ:  ��ȷ���ؽ������ݵ�����   ���󷵻�NULL
*��������:    �ȴ�������Ӧ��
*****************************************************************************************************/
uint8_t * sim900a_waitask(int fd, uint8_t waitask_hook(void))       //�ȴ�������Ӧ��
{
    uint8_t len=0;
    uint8_t *redata = (uint8_t *) malloc(64);
    if (redata == NULL)
    	return NULL;
    do{
        len = uart_rev_buf( fd, redata );   //��������
        if(waitask_hook!=0)
        {
            if(waitask_hook()==SIM900A_TRUE)           //���� SIM900A_TRUE ��ʾ��⵽�¼�����Ҫ�˳�
            {
                return NULL;               
            }
        }
    }while(len==0);                 //��������Ϊ0ʱһֱ�ȴ�
    
    SIM900A_DELAY(20);              //��ʱ��ȷ���ܽ��յ�ȫ�����ݣ�115200�������£�ÿms�ܽ���11.52���ֽڣ�
    return redata;
}


/****************************************************************************************************
*��������:    sim900a_cnum
*��������:    fd         �ļ�������
*             num        �绰�����ַ���
*��������ֵ:  ��ȷ����0  ���󷵻�1
*��������:    ����SIM900A�ĵ绰����
*****************************************************************************************************/
uint8_t sim900a_cnum(int fd, char *num)
{
    char *redata  = (char *) malloc( 64 );    /* ���ﲻ�ܶ�������� ��Ϊ������Ϊ���� ����������ĳ����е���++ */
    if ( redata == NULL ){
    	return FALSE;
    }
    uint8_t len;
    
    if(sim900a_cmd(fd, "AT+CNUM\r","OK", 100) != SIM900A_TRUE)
    {
        return SIM900A_FALSE;
    }
    
    len = uart_rev_buf( fd, redata );   /* �������� */
    
    if(len == 0)
    {
        return SIM900A_FALSE;
    }
    
    ///��һ�����ź��������Ϊ:"��������"
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return SIM900A_FALSE;
        }
        redata++;
    }
    redata+=2;
    
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //�ַ�����β��Ҫ��0
    free(redata);
    return SIM900A_TRUE;
}


/****************************************************************************************************
*��������:    sim900a_init
*��������:    fd         �ļ�������
*��������ֵ:  ��ȷ����0  ���󷵻�1
*��������:    ��ʼ��SIM900A�����ģ��
*****************************************************************************************************/
uint8_t sim900a_init(int fd)
{
	char redata[64] = {0};
	uint8_t len;
	
	tcflush(fd, TCIOFLUSH);                 //����˽��ջ���������
	
	if(sim900a_cmd(fd, "AT+CGMM\r","OK", 100) != SIM900A_TRUE)
	{
			return SIM900A_FALSE;
	}
	
	len = uart_rev_buf( fd, redata );   //��������
	
	if(len == 0)
	{
			return SIM900A_FALSE;
	}
	if (strstr(redata,"SIMCOM_SIM900A") != 0)
	{
		return SIM900A_TRUE;
	}
	else
		return SIM900A_FALSE;

}

/*---------------------------------------------------------------------*/

/****************************************************************************************************
*��������:    sim900a_call
*��������:    fd         �ļ�������
*             num        �绰�����ַ���
*��������ֵ:  void
*��������:    ����绰
*****************************************************************************************************/
void sim900a_call(int fd,char *num)
{
    tcflush(fd, TCIOFLUSH);                //����˽��ջ���������
    
    sim900a_tx_printf(fd, "ATD%s;\r",num);

    //������ǲ��������ݵ�
    //�����κ�Ӧ�𣬿�ͷ����"\r\n",��������Ч�������Ǵӵ�3���ֽڿ�ʼ
    //�Է��ҵ��绰��û��ͨ�������أ�BUSY
    //�Է������˵绰��+COLP: "555",129,"",0,"9648674F98DE"   OK
    //�Է������˵绰,Ȼ��ҵ���NO CARRIER
    //�Է���ʱû�ӵ绰��NO ANSWER
}

/****************************************************************************************************
*��������:    IsRing
*��������:    fd         �ļ�������
*             num        �绰�����ַ���
*��������ֵ:  void
*��������:    ����绰
*****************************************************************************************************/
uint8_t IsRing(int fd, char *num)
{
    char *redata  = (char *) malloc( 64 );    /* ���ﲻ�ܶ�������� ��Ϊ������Ϊ���� ����������ĳ����е���++ */
    if ( redata == NULL ){
    	return FALSE;
    }
    uint8_t len;
    
    if(sim900a_cmd_check(fd,"RING"))
    {
        return SIM900A_FALSE;
    }
    
    len = uart_rev_buf( fd, redata );   //��������
    if(len == 0)
    {
        return SIM900A_FALSE;
    }
    
    //��һ�����ź��������Ϊ:�����롰
    while(*redata != ':')
    {
        len--;
        if(len==0)
        {
            return SIM900A_FALSE;
        }
        redata++;
    }
    redata+=3;
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //�ַ�����β��Ҫ��0
	tcflush(fd, TCIOFLUSH);	/*��ջ�����*/
	free(redata);
    return SIM900A_TRUE;
}


/**
 * @brief  IsASSIC �ж��Ƿ�ASSIC����
 * @param  str: �ַ���ָ��
 * @retval ��ASSIC���뷵��TRUE���Ǵ�ASSIC���뷵��FALSE
 */
uint8_t IsASSIC(char * str)
{
    while(*str)
    {
        if(*str>0x7F)
        {
            return SIM900A_FALSE;
        }
        str++;
    }
    
    return SIM900A_TRUE;
}


/**
 * @brief  sim900a_char2hex ���ַ�ת����16�����ַ�
 * @param  hex: 16�����ַ��洢��λ��ָ�룬ch���ַ�
 * @retval ��
 */
void sim900a_char2hex(char *hex,char ch)
{
    const char numhex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    *hex++  = numhex[(ch & 0xF0)>>4];
    *hex    = numhex[ ch & 0x0F];
}

/**
 * @brief  sim900a_sms ���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
 * @param  num: �绰���룬smstext����������
 * @retval ��
 */
void sim900a_sms(int fd,char *num,char *smstext)
{
    char ucsbuff[160];
  
  	char end[2] = {0x1A,0x00};
  	char send_buf[64] = {0};

    tcflush(fd, TCIOFLUSH);                //����˽��ջ���������
    
    if(IsASSIC(smstext)==SIM900A_TRUE)
    {
        //Ӣ��
        sim900a_tx_printf(fd,"AT+CSCS=\"GSM\"\r");      //"GSM"�ַ���
        SIM900A_DELAY(100);
        
        sim900a_tx_printf(fd,"AT+CMGF=1\r");           //�ı�ģʽ
        SIM900A_DELAY(100);
        
        sim900a_tx_printf(fd,"AT+CMGS=\"%s\"\r",num);   //�绰����
        SIM900A_DELAY(100);

        sim900a_tx_printf(fd,"%s",smstext);            //��������
        //SIM900A_DELAY(100);          
    }
    else
    {
        //����
        sim900a_tx_printf(fd,"AT+CSCS=\"UCS2\"\r");    //"UCS2"�ַ���
        SIM900A_DELAY(100);
        
        sim900a_tx_printf(fd, "AT+CMGF=1\r");           //�ı�ģʽ
        SIM900A_DELAY(100);
        
        sim900a_tx_printf(fd, "AT+CSMP=17,167,0,8\r");  //
        SIM900A_DELAY(100);
        
        sim900a_gbk2ucs2hex(ucsbuff,num);
        sim900a_tx_printf(fd, "AT+CMGS=\"%s\"\r",ucsbuff);  //UCS2�ĵ绰����(��Ҫת�� ucs2��)
        SIM900A_DELAY(100);
        
        sim900a_gbk2ucs2hex(ucsbuff,smstext);
        sim900a_tx_printf(fd, "%s",ucsbuff);          //UCS2���ı�����(��Ҫת�� ucs2��)
        //SIM900A_DELAY(100);
    }
    SIM900A_DELAY(1); 
	uart_send_char(fd, 0x1A);		//����ʮ����������0X1A,��Ϣ��������
}




//��ѯ�Ƿ���յ��¶���
//0:���¶��ţ���0���¶��ŵ�ַ
uint8_t IsReceiveMS(int fd)
{
	char address[3]={0};
	uint8_t addressnum=0;
	char *redata  = (char *) malloc( 64 );    /* ���ﲻ�ܶ�������� ��Ϊ������Ϊ���� ����������ĳ����е���++ */
    if ( redata == NULL ){
    	return FALSE;
    }
    uint8_t len;
/*------------- ��ѯ�Ƿ����¶��Ų���ȡ�洢λ�� ----------------------------*/
    if(sim900a_cmd_check(fd, "+CMTI:"))
    {
        return 0;
    }
   
    len = uart_rev_buf( fd, redata );   //��������
	
	
    if(len == 0)
    {
        return 0;
    }
    //printf("CMTI:redata:%s,len=%d\n",redata,len);
    //��һ�����ź��������Ϊ:�����Ŵ洢��ַ��
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return 0;
        }
        redata++;
    }
	redata+=1;//ȥ����','
	address[0]= *redata++;
	address[1]= *redata++;
	address[2]= *redata++;
	if((address[2]>='0')&&(address[2]<='9'))
		addressnum=(address[0]-'0')*100+(address[1]-'0')*10+(address[2]-'0');
	else
		addressnum=(address[0]-'0')*10+(address[1]-'0');
	
	//printf("address:%c%c%c->%d\n",address[0],address[1],address[2],addressnum);	
	free(redata);
    return addressnum;
}	

//��ȡ��������
//�βΣ�	messadd��	���ŵ�ַ
//			num��		���淢���˺���(unicode�����ʽ���ַ���)
//			str��		�����������(unicode�����ʽ���ַ���)
//����ֵ��	0��ʾʧ��
//			1��ʾ�ɹ���ȡ�����ţ��ö���δ�����˴��ǵ�һ�ζ����������Զ�����׼Ϊ�Ѷ���
//			2��ʾ�ɹ���ȡ�����ţ��ö����Ѷ�
uint8_t readmessage(int fd, uint8_t messadd,char *num,char *str)
{
	char *redata  = (char *) malloc( 64 );    /* ���ﲻ�ܶ�������� ��Ϊ������Ϊ���� ����������ĳ����е���++ */
    if ( redata == NULL ){
    	return FALSE;
    }
    char cmd[20]={0};
    uint8_t len;
	char result=0;
	tcflush(fd, TCIOFLUSH);                 //����˽��ջ���������
	if(messadd>__g_MaxMessAdd)return 0;
	
/*------------- ��ȡ�������� ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);	
	if(sim900a_cmd(fd, cmd,"+CMGR:",500) != SIM900A_TRUE)
	{
		return 0;
	}
	len = uart_rev_buf( fd, redata );   //��������
	if(len == 0)
	{
		return 0;
	}
	//printf("CMGR:redata:%s\nlen=%d\n",redata,len);

	if(strstr(redata,"UNREAD")==0)result=2;
	else	result=1;
	//��һ�����ź��������Ϊ:�������˺��롰
	while(*redata != ',')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//ȥ��',"'
	while(*redata != '"')
	{
		*num++ = *redata++;
		len--;
	}
	*num = 0;               //�ַ�����β��Ҫ��0
	
	while(*redata != '+')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
	}
	redata+=6;//ȥ��'+32"\r'
	while(*redata != '\r')
	{
		*str++ = *redata++;
	}
	*str = 0;               //�ַ�����β��Ҫ��0
	free(redata);
	//printf("CMGR:result:%d\n",result);
	return result;
}

/**
 * @brief  sim900a_gbk2ucs2hex GBK����תucs2������ת��16�����ַ�
 * @param  ucs2hex: 16�������洢��λ��ָ�룬gbk���ַ�
 * @retval ��
 */
void sim900a_gbk2ucs2hex(char * ucs2hex,char * gbk)
{
    WCHAR   tmp = 0;
    uint8_t buf[2] = {0};
    
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //Ӣ��
        {
            
            *ucs2hex++ = '0';
            *ucs2hex++ = '0';
            sim900a_char2hex(ucs2hex,*gbk);
            ucs2hex+=2;
            gbk++;  
        }
        else                        //����
        {
        	buf[1]=*gbk++;
		    buf[0]=*gbk++; 
            tmp = ff_convert((WCHAR)*(uint16_t*)buf,1);
            sim900a_char2hex(ucs2hex,(char)(tmp>>8));
            ucs2hex+=2;
            sim900a_char2hex(ucs2hex,(char)tmp);
            ucs2hex+=2;
            gbk+=2;
        }
    }
    *ucs2hex=0;
}


/**
 * @brief  hexuni2gbk 16����ת����ucs2��ת����GBK����
 * @param  ucs2hex: 16�������洢��λ��ָ�룬gbk���ַ�
 * @retval ��
 */
uint8_t hexuni2gbk(int fd, char *hexuni,char *chgbk)
{
	uint8_t len=0,i=0;
	WCHAR wgbk=0;
	WCHAR tmp=0;
	uint8_t unitmp[4]={0};
	tcflush(fd, TCIOFLUSH);
	len=strlen(hexuni);
	if(!len)return SIM900A_FALSE;
	//printf("hexuni:%s::len:%d\n",hexuni,len);
	for(i=0;i<len/4;++i)
	{
		if(hexuni[4*i]>=0x41)	unitmp[0]=hexuni[4*i]-0x41+10;
		else	unitmp[0]=hexuni[4*i]-0x30;
		if(hexuni[4*i+1]>=0x41)unitmp[1]=hexuni[4*i+1]-0x41+10;
		else	unitmp[1]=hexuni[4*i+1]-0x30;
		if(hexuni[4*i+2]>=0x41)unitmp[2]=hexuni[4*i+2]-0x41+10;
		else	unitmp[2]=hexuni[4*i+2]-0x30;
		if(hexuni[4*i+3]>=0x41)unitmp[3]=hexuni[4*i+3]-0x41+10;
		else	unitmp[3]=hexuni[4*i+3]-0x30;
		
		tmp=unitmp[0]*0x1000+unitmp[1]*0x100+unitmp[2]*16+unitmp[3];
		wgbk=ff_convert(tmp,0);
		//printf("tmp:%X->wgbk:%X\n",tmp,wgbk);
		
		if(wgbk<0x80)
		{
			*chgbk=(char)wgbk;
			chgbk++;
		}
		else
		{
			*chgbk=(char)(wgbk>>8);
			chgbk++;
			*chgbk=(char)wgbk;
			chgbk++;
		}		
	}	
	*chgbk=0;
	return SIM900A_TRUE;		
}




/****************************************************************************************************
*��������:    sim900a_gprs_init
*��������:    fd         �ļ�������
*��������ֵ:  void
*��������:    ��ʼ��GPRS
*****************************************************************************************************/
void sim900a_gprs_init(int fd)
{
    sim900a_tx_printf(fd, "AT+CGCLASS=\"B\"\r");                       //�����ƶ�̨���Ϊ"B"
    SIM900A_DELAY(100);
    sim900a_tx_printf(fd, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r");         //PDP�����ı�ʶ1��������Э�飬��������ƣ�CMNET
    SIM900A_DELAY(100);
    sim900a_tx_printf(fd, "AT+CGATT=1\r");                             //���� GPRS ����
    SIM900A_DELAY(100);
    sim900a_tx_printf(fd, "AT+CIPCSGP=1,\"CMNET\"\r");                  //����Ϊ GPRS ����ģʽ �������ΪCMNET
}

/****************************************************************************************************
*��������:    sim900a_gprs_tcp_link
*��������:    fd         �ļ�������
*             localport  ���ض˿ں�
*             serverip   ������ip
*             serverport �������˿ں�
*��������ֵ:  void
*��������:    SIM900A TCP���ӳ�ʼ��
*****************************************************************************************************/
void sim900a_gprs_tcp_link(int fd, char *localport,char * serverip,char * serverport)
{
    sim900a_tx_printf(fd, "AT+CLPORT=\"TCP\",\"%s\"\r",localport);         //��ȡ���ض˿�
    SIM900A_DELAY(100);

     //���÷�����IP�Ͷ˿�
    sim900a_tx_printf(fd, "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
}

/****************************************************************************************************
*��������:    sim900a_gprs_udp_link
*��������:    fd         �ļ�������
*             localport  ���ض˿ں�
*             serverip   ������ip
*             serverport �������˿ں�
*��������ֵ:  void
*��������:    SIM900A UDP���ӳ�ʼ��
*****************************************************************************************************/
void sim900a_gprs_udp_link(int fd, char *localport,char * serverip,char * serverport)
{
    sim900a_tx_printf(fd, "AT+CLPORT=\"UDP\", \"%s\"\r", localport);              //��ȡ���ض˿�
    SIM900A_DELAY(100);

    sim900a_tx_printf(fd, "AT+CIPSTART=\"UDP\", \"%s\", \"%s\"\r", serverip,serverport);   //���÷�����IP�Ͷ˿�
}

/****************************************************************************************************
*��������:    sim900a_gprs_send
*��������:    fd         �ļ�������
*             str        ���͵��ַ���
*��������ֵ:  void
*��������:    SIM900 ���ͺ���
*****************************************************************************************************/
void sim900a_gprs_send(int fd, char * str)
{
    sim900a_tx_printf(fd, "AT+CIPSEND\r");
    SIM900A_DELAY(100);
        
    sim900a_tx_printf(fd, "%s%c",str,0x1A);
}


/****************************************************************************************************
*��������:    sim900a_gprs_link_close
*��������:    fd         �ļ�������
*��������ֵ:  void
*��������:    SIM900 GPRS���ӹر�
*****************************************************************************************************/
void sim900a_gprs_link_close(int fd)               //IP���ӶϿ�
{
    sim900a_tx_printf(fd, "AT+CIPCLOSE=1\r");
}

