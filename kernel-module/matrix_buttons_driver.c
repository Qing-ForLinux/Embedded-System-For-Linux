#include "matrix_buttons_driver.h"

static struct timer_list scan_timer;    /* 定义一个内核定时器 */
struct input_dev *button_dev;    /* 定义一个input结构 */
struct work_struct *scan_work;    /* 定义一个工作队列 */
static int g_key_num = -1;    /* 按键编号 */	
static int g_key_line_val = -1;    /* 按键行号 */
static int g_key_row_val = -1;    /* 按键列号 */


/****************************************************************************************************
*函数名称:    work1_func
*函数参数:    work        work_struct类型的指针
*函数返回值:  无
*函数描述:    根据行号以及列号得到需要上报的按键编号，并且上报
*****************************************************************************************************/
static void work_func(struct work_struct *work)
{
	unsigned int value = 1;
    del_timer(&scan_timer);              /* 关闭定时器 */
	msleep(5);     /* 延时5个ms */
	value = s3c2410_gpio_getpin(button_io_line[g_key_row_val].pin);    /* 再次读取列按键的值 */
	if (value != 0){
		mod_timer(&scan_timer, jiffies + (HZ/20));				/* 重新打开定时器 */
		return ;    /* 退出程序 */
	}

	/* 等待列按键弹起 如果不弹起该工作队列就睡觉 */
	while ( !s3c2410_gpio_getpin(button_io_line[g_key_line_val].pin) ){    
		msleep(1);
	}
	g_key_num = ZROW_NUM * g_key_line_val + g_key_row_val;    /* 根据号号以及列号 上报按键的编号值 */
	input_report_key(button_dev, g_key_num, BUTTON_DOWN);    /* 上报按键的值 */	
	input_sync(button_dev);									/* 同步事件报告 */
	mod_timer(&scan_timer, jiffies + (HZ/20));				/* 重新打开定时器 */
}
		

/****************************************************************************************************
*函数名称:    matrix_button_irq
*函数参数:    irq        中断号
              dev_id     由request_irq的最后一个参数传递
*函数返回值:  正常返回IRQ_NONE    异常返回IRQ_HANDLED
*函数描述:    按键下降沿中断函数 (该中断主要是检测列信号)
*****************************************************************************************************/
static irqreturn_t matrix_button_irq(int irq, void *dev_id)
{
	g_key_row_val = (int)dev_id;    /* 获取发生中断的列号 */
	if ( (g_key_row_val >=0) && (g_key_row_val < ZLINE_NUM) ){
		
		/* 调用中断处理下半部 */
		schedule_work(scan_work);
		return IRQ_NONE;
	}
	else{
		g_key_row_val = -1;    /* 重新初始化g_key_num的值 */
		goto out;
	}
	
 out:
	return IRQ_HANDLED;
}


/****************************************************************************************************
*函数名称:    timer_function
*函数参数:    由input_dev.data传递
*函数返回值:  void
*函数描述:    定时器中断函数  主要是周期性的控制行信号拉低拉高
*****************************************************************************************************/
void timer_function(unsigned long para)
{
	static unsigned int line = 0;    /* 行号 */

	s3c2410_gpio_setpin(button_io_line[line].pin,1);		//当前扫描行置 1

	if(line == ZLINE_NUM){    /*如果行全部扫描完毕*/
		line = 0;    /*从第一行开始扫描*/
		s3c2410_gpio_setpin(button_io_line[line].pin,0);    /* 重新扫描第一行 */
	}
	else{
		line++;
		s3c2410_gpio_setpin(button_io_line[line].pin,0);    /* 下一扫描行置 0 */
	}
	g_key_row_val = line;    /* 保存行号 */

	/* 重新启动定时器 */
	mod_timer(&scan_timer, jiffies + (HZ/20));				//50ms	

}

/****************************************************************************************************
*函数名称:    input_open
*函数参数:    dev        input_dev结构体指针
*函数返回值:  成功返回0，失败返回-1
*函数描述:    input设备打开函数
*****************************************************************************************************/
static int input_open(struct input_dev *dev)
{	
	char ret = 0;
	unsigned char i;
	/******列配置为内部上拉\输入*********/
	for(i = 0;i < ZROW_NUM;i++){
		s3c2410_gpio_cfgpin(button_io_row[i].pin , button_io_row[i].pin_setting1);//配置为输入
		writel(0x0,S3C2410_GPGUP);	/* 配置成内部上拉,默认上拉 需要根据具体的引脚修改 */
		/* 注册行按键中断服务程序 这里共用一个中断服务程序 通过dev_id字段识别不同的中断源 */
		ret = request_irq(button_io_row[i].irq, matrix_button_irq, IRQF_TRIGGER_FALLING, 
		            button_io_row[i].name, (void *)button_io_row[i].number);
		if (ret < 0){
			printk(KERN_ERR "request_irq error and ret = %d\r\n",ret);
			return -1;
		}
	}

	/*************行配置成输出***************/
	for(i = 0;i < ZLINE_NUM;i++){
		s3c2410_gpio_cfgpin(button_io_line[i].pin, button_io_line[i].pin_setting2);//配置为输出
		s3c2410_gpio_setpin(button_io_line[i].pin, 1);									//初始化各行输出1
	}
	/* 首先设置第一行为0 */
	s3c2410_gpio_setpin(button_io_line[0].pin,0);    /* 先将第一列设置成低电平 */								
	return 0;
}


/* 混杂设备 */
static struct miscdevice button_miscdev =
{
    .minor = 210,
    .name = "matrix_button",
};


/****************************************************************************************************
*函数名称:    matrix_button_init
*函数参数:    void
*函数返回值:  成功返回0，失败返回-1
*函数描述:    模块加载函数(还需要创建一个字符设备)
*****************************************************************************************************/

static int __init matrix_button_init(void)
{
	int i,err;

	/* 创建混杂设备 */
	err = misc_register(&button_miscdev);    /* 设备初始化 */
	if (err < 0){
		printk(KERN_ERR "Unable to creat misc_dev !!\n");
		return -ENOMEM;
	}

	/********初始化内核定时器*******/
	init_timer(&scan_timer);
	scan_timer.data = 0;
	scan_timer.expires = jiffies + (HZ / 20);//HZ=1000(1s的指令周期，不随主频变化，单位ms) 
	scan_timer.function = timer_function;    /* 定时器回调函数 */
	add_timer(&scan_timer);

	/****** 初始化工作队列 ******/
	INIT_WORK(scan_work, work_func);

	/********初始化input子系统******/
	button_dev = input_allocate_device();    /* 分配一个input_dev结构 */
	if (!button_dev) {
		printk(KERN_ERR "Unable to allocate the input device !!\n");
		return -ENOMEM;
	}
	button_dev->name = "ET35_button_event";    /* input设备的名称 */
	button_dev->open = input_open;    /* 打开函数 */
	button_dev->close = NULL;
	/* 上报的事件类型 */
	button_dev->evbit[0] = BIT_MASK(EV_KEY);			// | EV_SYN);支持EV_KEY事件
	/* 设置上报的按键编码 */
	for(i = 0;i < ZKEY_NUM; i++)							//ZKEY_NUM=16
		set_bit(button_info[i].code, button_dev->keybit);	//支持具体按键键码

	err = input_register_device(button_dev);   				//注册input设备
	if(err){
		printk(KERN_ERR "input_register_device\n");
		return -ENOMEM;
	}

	return 0;
}
 
/****************************************************************************************************
*函数名称:    matrix_button_exit
*函数参数:    void
*函数返回值:  void
*函数描述:    模块卸载函数
*****************************************************************************************************/
static void __exit matrix_button_exit(void)
{
	del_timer(&scan_timer);    /* 停止定时器 */
	input_unregister_device(button_dev);    /* 删除字符设备 */
	misc_deregister(&button_miscdev);	/*释放混杂设备*/
}

module_init(matrix_button_init);
module_exit(matrix_button_exit);
MODULE_LICENSE("GPL");
