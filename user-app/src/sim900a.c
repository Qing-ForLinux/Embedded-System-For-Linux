#include "sim900a.h"


static uint8_t __g_MaxMessAdd=50;

/****************************************************************************************************
*函数名称:    sim900a_cmd
*函数参数:    fd        文件描述符
*             cmd       命令字符串
*             replay    发送的内容
*             waittime  等待时间
*函数返回值:  正确返回0    错误返回1
*函数描述:    向SIM900A发送指令
*****************************************************************************************************/
uint8_t sim900a_cmd(int fd, char *cmd, char *reply, uint32_t waittime )
{    
	tcflush(fd, TCIOFLUSH);                 //清空了接收缓冲区数据
                   
    uart_send_buf(fd, cmd, strlen(cmd));    /* 发送命令 */

    if(reply == 0)                     //不需要接收数据
    {
        return SIM900A_TRUE;
    }
    
    SIM900A_DELAY(waittime);                 //延时
    
    return sim900a_cmd_check(fd, reply);    //对接收数据进行处理
}

/****************************************************************************************************
*函数名称:    sim900a_cmd_check
*函数参数:    fd        文件描述符
*             replay    接收的数组
*函数返回值:  正确返回0    错误返回1
*函数描述:    从SIM900A读取串口数据
*****************************************************************************************************/
uint8_t sim900a_cmd_check(int fd,char *reply)
{
    uint8_t len = 0;
    uint8_t n = 0;
    uint8_t off = 0;
    char *redata  = (char *) malloc( 64 );    /* 这里不能定义成数组 因为数组名为常量 不能在下面的程序中调用++ */
    if ( redata == NULL ){
    	return FALSE;
    }

    len = uart_rev_buf( fd, redata );   //接收数据
    
    n = 0;
    off = 0;
    while((n + off)<len)
    {
        if(reply[n] == 0)                 //数据为空或者比较完毕
        {
            return SIM900A_TRUE;
        }
        
        if(redata[ n + off]== reply[n])
        {
            n++;                 //移动到下一个接收数据
        }
        else
        {
            off++;              //进行下一轮匹配
            n=0;                //重来
        }
        //n++;
    }

    if(reply[n]==0)   //刚好匹配完毕
    {
        return SIM900A_TRUE;
    }
    free(redata);
    return SIM900A_FALSE;       //跳出循环表示比较完毕后都没有相同的数据，因此跳出
}

/****************************************************************************************************
*函数名称:    sim900a_waitask
*函数参数:    fd           文件描述符
*             waitask_hook 不知
*函数返回值:  正确返回接收数据的数组   错误返回NULL
*函数描述:    等待有数据应答
*****************************************************************************************************/
uint8_t * sim900a_waitask(int fd, uint8_t waitask_hook(void))       //等待有数据应答
{
    uint8_t len=0;
    uint8_t *redata = (uint8_t *) malloc(64);
    if (redata == NULL)
    	return NULL;
    do{
        len = uart_rev_buf( fd, redata );   //接收数据
        if(waitask_hook!=0)
        {
            if(waitask_hook()==SIM900A_TRUE)           //返回 SIM900A_TRUE 表示检测到事件，需要退出
            {
                return NULL;               
            }
        }
    }while(len==0);                 //接收数据为0时一直等待
    
    SIM900A_DELAY(20);              //延时，确保能接收到全部数据（115200波特率下，每ms能接收11.52个字节）
    return redata;
}


/****************************************************************************************************
*函数名称:    sim900a_cnum
*函数参数:    fd         文件描述符
*             num        电话号码字符串
*函数返回值:  正确返回0  错误返回1
*函数描述:    接收SIM900A的电话号码
*****************************************************************************************************/
uint8_t sim900a_cnum(int fd, char *num)
{
    char *redata  = (char *) malloc( 64 );    /* 这里不能定义成数组 因为数组名为常量 不能在下面的程序中调用++ */
    if ( redata == NULL ){
    	return FALSE;
    }
    uint8_t len;
    
    if(sim900a_cmd(fd, "AT+CNUM\r","OK", 100) != SIM900A_TRUE)
    {
        return SIM900A_FALSE;
    }
    
    len = uart_rev_buf( fd, redata );   /* 接收数据 */
    
    if(len == 0)
    {
        return SIM900A_FALSE;
    }
    
    ///第一个逗号后面的数据为:"本机号码"
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
    *num = 0;               //字符串结尾需要清0
    free(redata);
    return SIM900A_TRUE;
}


/****************************************************************************************************
*函数名称:    sim900a_init
*函数参数:    fd         文件描述符
*函数返回值:  正确返回0  错误返回1
*函数描述:    初始化SIM900A并检测模块
*****************************************************************************************************/
uint8_t sim900a_init(int fd)
{
	char redata[64] = {0};
	uint8_t len;
	
	tcflush(fd, TCIOFLUSH);                 //清空了接收缓冲区数据
	
	if(sim900a_cmd(fd, "AT+CGMM\r","OK", 100) != SIM900A_TRUE)
	{
			return SIM900A_FALSE;
	}
	
	len = uart_rev_buf( fd, redata );   //接收数据
	
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
*函数名称:    sim900a_call
*函数参数:    fd         文件描述符
*             num        电话号码字符串
*函数返回值:  void
*函数描述:    拨打电话
*****************************************************************************************************/
void sim900a_call(int fd,char *num)
{
    tcflush(fd, TCIOFLUSH);                //清空了接收缓冲区数据
    
    sim900a_tx_printf(fd, "ATD%s;\r",num);

    //拨打后是不返回数据的
    //不管任何应答，开头都是"\r\n",即真正有效的数据是从第3个字节开始
    //对方挂掉电话（没接通），返回：BUSY
    //对方接听了电话：+COLP: "555",129,"",0,"9648674F98DE"   OK
    //对方接听了电话,然后挂掉：NO CARRIER
    //对方超时没接电话：NO ANSWER
}

/****************************************************************************************************
*函数名称:    IsRing
*函数参数:    fd         文件描述符
*             num        电话号码字符串
*函数返回值:  void
*函数描述:    来点电话
*****************************************************************************************************/
uint8_t IsRing(int fd, char *num)
{
    char *redata  = (char *) malloc( 64 );    /* 这里不能定义成数组 因为数组名为常量 不能在下面的程序中调用++ */
    if ( redata == NULL ){
    	return FALSE;
    }
    uint8_t len;
    
    if(sim900a_cmd_check(fd,"RING"))
    {
        return SIM900A_FALSE;
    }
    
    len = uart_rev_buf( fd, redata );   //接收数据
    if(len == 0)
    {
        return SIM900A_FALSE;
    }
    
    //第一个逗号后面的数据为:”号码“
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
    *num = 0;               //字符串结尾需要清0
	tcflush(fd, TCIOFLUSH);	/*清空缓冲区*/
	free(redata);
    return SIM900A_TRUE;
}


/**
 * @brief  IsASSIC 判断是否纯ASSIC编码
 * @param  str: 字符串指针
 * @retval 纯ASSIC编码返回TRUE，非纯ASSIC编码返回FALSE
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
 * @brief  sim900a_char2hex 把字符转换成16进制字符
 * @param  hex: 16进制字符存储的位置指针，ch：字符
 * @retval 无
 */
void sim900a_char2hex(char *hex,char ch)
{
    const char numhex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    *hex++  = numhex[(ch & 0xF0)>>4];
    *hex    = numhex[ ch & 0x0F];
}

/**
 * @brief  sim900a_sms 发送短信（支持中英文,中文为GBK码）
 * @param  num: 电话号码，smstext：短信内容
 * @retval 无
 */
void sim900a_sms(int fd,char *num,char *smstext)
{
    char ucsbuff[160];
  
  	char end[2] = {0x1A,0x00};
  	char send_buf[64] = {0};

    tcflush(fd, TCIOFLUSH);                //清空了接收缓冲区数据
    
    if(IsASSIC(smstext)==SIM900A_TRUE)
    {
        //英文
        sim900a_tx_printf(fd,"AT+CSCS=\"GSM\"\r");      //"GSM"字符集
        SIM900A_DELAY(100);
        
        sim900a_tx_printf(fd,"AT+CMGF=1\r");           //文本模式
        SIM900A_DELAY(100);
        
        sim900a_tx_printf(fd,"AT+CMGS=\"%s\"\r",num);   //电话号码
        SIM900A_DELAY(100);

        sim900a_tx_printf(fd,"%s",smstext);            //短信内容
        //SIM900A_DELAY(100);          
    }
    else
    {
        //中文
        sim900a_tx_printf(fd,"AT+CSCS=\"UCS2\"\r");    //"UCS2"字符集
        SIM900A_DELAY(100);
        
        sim900a_tx_printf(fd, "AT+CMGF=1\r");           //文本模式
        SIM900A_DELAY(100);
        
        sim900a_tx_printf(fd, "AT+CSMP=17,167,0,8\r");  //
        SIM900A_DELAY(100);
        
        sim900a_gbk2ucs2hex(ucsbuff,num);
        sim900a_tx_printf(fd, "AT+CMGS=\"%s\"\r",ucsbuff);  //UCS2的电话号码(需要转成 ucs2码)
        SIM900A_DELAY(100);
        
        sim900a_gbk2ucs2hex(ucsbuff,smstext);
        sim900a_tx_printf(fd, "%s",ucsbuff);          //UCS2的文本内容(需要转成 ucs2码)
        //SIM900A_DELAY(100);
    }
    SIM900A_DELAY(1); 
	uart_send_char(fd, 0x1A);		//发送十六进制数：0X1A,信息结束符号
}




//查询是否接收到新短信
//0:无新短信，非0：新短信地址
uint8_t IsReceiveMS(int fd)
{
	char address[3]={0};
	uint8_t addressnum=0;
	char *redata  = (char *) malloc( 64 );    /* 这里不能定义成数组 因为数组名为常量 不能在下面的程序中调用++ */
    if ( redata == NULL ){
    	return FALSE;
    }
    uint8_t len;
/*------------- 查询是否有新短信并提取存储位置 ----------------------------*/
    if(sim900a_cmd_check(fd, "+CMTI:"))
    {
        return 0;
    }
   
    len = uart_rev_buf( fd, redata );   //接收数据
	
	
    if(len == 0)
    {
        return 0;
    }
    //printf("CMTI:redata:%s,len=%d\n",redata,len);
    //第一个逗号后面的数据为:”短信存储地址“
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return 0;
        }
        redata++;
    }
	redata+=1;//去掉；','
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

//读取短信内容
//形参：	messadd：	短信地址
//			num：		保存发件人号码(unicode编码格式的字符串)
//			str：		保存短信内容(unicode编码格式的字符串)
//返回值：	0表示失败
//			1表示成功读取到短信，该短信未读（此处是第一次读，读完后会自动被标准为已读）
//			2表示成功读取到短信，该短信已读
uint8_t readmessage(int fd, uint8_t messadd,char *num,char *str)
{
	char *redata  = (char *) malloc( 64 );    /* 这里不能定义成数组 因为数组名为常量 不能在下面的程序中调用++ */
    if ( redata == NULL ){
    	return FALSE;
    }
    char cmd[20]={0};
    uint8_t len;
	char result=0;
	tcflush(fd, TCIOFLUSH);                 //清空了接收缓冲区数据
	if(messadd>__g_MaxMessAdd)return 0;
	
/*------------- 读取短信内容 ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);	
	if(sim900a_cmd(fd, cmd,"+CMGR:",500) != SIM900A_TRUE)
	{
		return 0;
	}
	len = uart_rev_buf( fd, redata );   //接收数据
	if(len == 0)
	{
		return 0;
	}
	//printf("CMGR:redata:%s\nlen=%d\n",redata,len);

	if(strstr(redata,"UNREAD")==0)result=2;
	else	result=1;
	//第一个逗号后面的数据为:”发件人号码“
	while(*redata != ',')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//去掉',"'
	while(*redata != '"')
	{
		*num++ = *redata++;
		len--;
	}
	*num = 0;               //字符串结尾需要清0
	
	while(*redata != '+')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
	}
	redata+=6;//去掉'+32"\r'
	while(*redata != '\r')
	{
		*str++ = *redata++;
	}
	*str = 0;               //字符串结尾需要清0
	free(redata);
	//printf("CMGR:result:%d\n",result);
	return result;
}

/**
 * @brief  sim900a_gbk2ucs2hex GBK编码转ucs2编码再转成16进制字符
 * @param  ucs2hex: 16进制数存储的位置指针，gbk：字符
 * @retval 无
 */
void sim900a_gbk2ucs2hex(char * ucs2hex,char * gbk)
{
    WCHAR   tmp = 0;
    uint8_t buf[2] = {0};
    
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //英文
        {
            
            *ucs2hex++ = '0';
            *ucs2hex++ = '0';
            sim900a_char2hex(ucs2hex,*gbk);
            ucs2hex+=2;
            gbk++;  
        }
        else                        //中文
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
 * @brief  hexuni2gbk 16进制转换成ucs2再转换成GBK编码
 * @param  ucs2hex: 16进制数存储的位置指针，gbk：字符
 * @retval 无
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
*函数名称:    sim900a_gprs_init
*函数参数:    fd         文件描述符
*函数返回值:  void
*函数描述:    初始化GPRS
*****************************************************************************************************/
void sim900a_gprs_init(int fd)
{
    sim900a_tx_printf(fd, "AT+CGCLASS=\"B\"\r");                       //设置移动台类别为"B"
    SIM900A_DELAY(100);
    sim900a_tx_printf(fd, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r");         //PDP上下文标识1，互联网协议，接入点名称：CMNET
    SIM900A_DELAY(100);
    sim900a_tx_printf(fd, "AT+CGATT=1\r");                             //附着 GPRS 网络
    SIM900A_DELAY(100);
    sim900a_tx_printf(fd, "AT+CIPCSGP=1,\"CMNET\"\r");                  //设置为 GPRS 连接模式 ，接入点为CMNET
}

/****************************************************************************************************
*函数名称:    sim900a_gprs_tcp_link
*函数参数:    fd         文件描述符
*             localport  本地端口号
*             serverip   服务器ip
*             serverport 服务器端口号
*函数返回值:  void
*函数描述:    SIM900A TCP连接初始化
*****************************************************************************************************/
void sim900a_gprs_tcp_link(int fd, char *localport,char * serverip,char * serverport)
{
    sim900a_tx_printf(fd, "AT+CLPORT=\"TCP\",\"%s\"\r",localport);         //获取本地端口
    SIM900A_DELAY(100);

     //设置服务器IP和端口
    sim900a_tx_printf(fd, "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
}

/****************************************************************************************************
*函数名称:    sim900a_gprs_udp_link
*函数参数:    fd         文件描述符
*             localport  本地端口号
*             serverip   服务器ip
*             serverport 服务器端口号
*函数返回值:  void
*函数描述:    SIM900A UDP连接初始化
*****************************************************************************************************/
void sim900a_gprs_udp_link(int fd, char *localport,char * serverip,char * serverport)
{
    sim900a_tx_printf(fd, "AT+CLPORT=\"UDP\", \"%s\"\r", localport);              //获取本地端口
    SIM900A_DELAY(100);

    sim900a_tx_printf(fd, "AT+CIPSTART=\"UDP\", \"%s\", \"%s\"\r", serverip,serverport);   //设置服务器IP和端口
}

/****************************************************************************************************
*函数名称:    sim900a_gprs_send
*函数参数:    fd         文件描述符
*             str        发送的字符串
*函数返回值:  void
*函数描述:    SIM900 发送函数
*****************************************************************************************************/
void sim900a_gprs_send(int fd, char * str)
{
    sim900a_tx_printf(fd, "AT+CIPSEND\r");
    SIM900A_DELAY(100);
        
    sim900a_tx_printf(fd, "%s%c",str,0x1A);
}


/****************************************************************************************************
*函数名称:    sim900a_gprs_link_close
*函数参数:    fd         文件描述符
*函数返回值:  void
*函数描述:    SIM900 GPRS连接关闭
*****************************************************************************************************/
void sim900a_gprs_link_close(int fd)               //IP链接断开
{
    sim900a_tx_printf(fd, "AT+CIPCLOSE=1\r");
}

