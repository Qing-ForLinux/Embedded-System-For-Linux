#include "dht11_driver.h"

/* 数据结构 */
struct DHT11_Data
{
	uint8_t temperature;
	uint8_t humidity;
};


/****************************************************************************************************
*函数名称:    dht11_rst
*函数参数:    void
*函数返回值:  void
*函数描述:    复位dht11
*****************************************************************************************************/
void dht11_rst(void)     
{                 
	/* 设置引脚为输入输出模式 */
	s3c2410_gpio_cfgpin(g_DHT_GPIO, g_DHT_GPIO_OUTP);
	s3c2410_gpio_setpin(g_DHT_GPIO, 0); //拉低DQ
	msleep(20);     //拉低至少18ms
	s3c2410_gpio_setpin(g_DHT_GPIO, 1);   //DQ=1 
	udelay(30);       //主机拉高20~40us
}


/****************************************************************************************************
*函数名称:    dht11_check
*函数参数:    void
*函数返回值:  成功返回0，失败返回1
*函数描述:    检测dht11硬件是否存在
*****************************************************************************************************/
uint8_t dht11_check(void)     
{   
	uint8_t retry=0;

	s3c2410_gpio_cfgpin(g_DHT_GPIO, g_DHT_GPIO_INP); //SET INPUT 

	while (s3c2410_gpio_getpin(g_DHT_GPIO) && retry < 100){		//dht11会拉低40~80us
		retry++;
		udelay(1);
	}   

	if (retry>=100)
		return 1;
	else 
		retry=0;

	while (!s3c2410_gpio_getpin(g_DHT_GPIO) && retry < 100){	//dht11拉低后会再次拉高40~80us
		retry++;
		udelay(1);
	};

	if(retry >= 100)
		return 1; 

	return 0;
}


/****************************************************************************************************
*函数名称:    dht11_read_bit
*函数参数:    void
*函数返回值:  成功返回0，失败返回1
*函数描述:    从dht11读取一位的数据
*****************************************************************************************************/
uint8_t dht11_read_bit(void)        
{
	uint8_t retry=0;
	uint32_t pin_val = 0; /* 引脚电平值 */

	while (s3c2410_gpio_getpin(g_DHT_GPIO) && retry < 100){		//等待变为低电平
		retry++;
		udelay(1);
	}

	retry = 0;
	while (!s3c2410_gpio_getpin(g_DHT_GPIO) && retry < 100){	//等待变高电平
		retry++;
		udelay(1);
	}
	udelay(40);//等待40us
	pin_val = s3c2410_gpio_getpin(g_DHT_GPIO);
	if (pin_val)
		return 1;
	else 
		return 0;       
}


/****************************************************************************************************
*函数名称:    dht11_read_byte
*函数参数:    void
*函数返回值:  成功返回读取的数据，失败返回0
*函数描述:    从dht11读取一个字节的数据
*****************************************************************************************************/
uint8_t dht11_read_byte(void)    
{        
	uint8_t i,dat;
	dat=0;
	for (i=0; i<8; i++){
		dat <<= 1; 
		dat |= dht11_read_bit();
	}               
	return dat;
}

/****************************************************************************************************
*函数名称:    dht11_read_data
*函数参数:    temp: 温度值
*             humi: 湿度值
*函数返回值:  成功返回0，失败返回1
*函数描述:    从dht11读取一次数据(温度和湿度)
*****************************************************************************************************/
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)    
{        
	uint8_t buf[5];
	uint8_t i;
	dht11_rst();
	if (dht11_check() == 0){
		for (i=0; i<5; i++){	//读取40位数据
			buf[i] = dht11_read_byte();
		}
		if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]){
			*humi = buf[0];
			*temp = buf[2];
		}
	}
	else 
		return 1;

	return 0;     
}

/****************************************************************************************************
*函数名称:    dht11_init
*函数参数:    void
*函数返回值:  成功返回0，失败返回1
*函数描述:    dht初始化函数
*****************************************************************************************************/
uint8_t dht11_init(void)
{ 
	s3c2410_gpio_cfgpin(g_DHT_GPIO, g_DHT_GPIO_OUTP);
	s3c2410_gpio_setpin(g_DHT_GPIO, 1);             //PG11 输出高
	dht11_rst();  //复位dht11
	return dht11_check();//等待dht11的回应
} 

/****************************************************************************************************
*函数名称:    dht_read
*函数参数:    file: 文件描述符
*             buf:  返回的接收数组
*             count:  读取的字节数
*             ppos: 偏移量
*函数返回值:  成功返回读取的字节数，失败返回-1
*函数描述:    dht读取温湿度函数
*****************************************************************************************************/
static ssize_t dht_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	int ret = -1; 
	uint32_t len = 0;

	struct DHT11_Data *dht11_data = (struct DHT11_Data *)kmalloc(sizeof(struct DHT11_Data), GFP_KERNEL);
	if (dht11_data == NULL){
		printk(KERN_ERR "dht11_data is NULL!!\n");
		return -1;
	}
	/* 读取温湿度值 */
	dht11_read_data( &(dht11_data->temperature), &(dht11_data->humidity) );

	dht11_data = (char *)dht11_data; 

	ret = copy_to_user( buf, dht11_data, sizeof(struct DHT11_Data) );

	if (ret > 0)
		len = sizeof(struct DHT11_Data);

	if (len < 0){
		printk(KERN_ERR "copy_to_user error!\r\n");
		return -1;
	}

	return len;
}


struct file_operations dht_fops=
{
   .owner = THIS_MODULE,
   .read = dht_read,
};

static struct miscdevice dht_miscdev =
{
    .minor = 210,
    .name = "dht_dev",
    .fops = &dht_fops,
};

/****************************************************************************************************
*函数名称:    dth_driver_init
*函数参数:    void
*函数返回值:  成功返回0，失败返回ENOMEM
*函数描述:    注册平台驱动
*****************************************************************************************************/
static int __init dth_driver_init(void)
{
	int ret = 0;
	/* 创建混杂设备 */
	ret = misc_register(&dht_miscdev);    /* 设备初始化 */
	if (ret < 0){
		printk(KERN_ERR "Unable to creat misc_dev !!\n");
		return -ENOMEM;
	}
	/* 循环检测dth11硬件是否已经连接 */
	while ( dht11_init() ){
		msleep(100);  /* 睡眠100ms */ 
	}
	printk("\r\n*********************************************************\r\n");
	printk("\r\n*****************Detect dht11 is connected***************\r\n");
	printk("\r\n*********************************************************\r\n");

	return 0;
}


/****************************************************************************************************
*函数名称:    dht_driver_exit
*函数参数:    void
*函数返回值:  void
*函数描述:    模块卸载函数
*****************************************************************************************************/
static void __exit dht_driver_exit(void)
{
	/*释放混杂设备*/
	misc_deregister(&dht_miscdev); 
}


module_init(dth_driver_init);
module_exit(dht_driver_exit);
MODULE_LICENSE("GPL");