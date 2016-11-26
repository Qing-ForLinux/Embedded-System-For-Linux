#ifndef _MATRIX_BUTTONS_DRIVER_H_
#define _MATRIX_BUTTONS_DRIVER_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/regs-gpio.h>
#include <linux/miscdevice.h>
#include <linux/timer.h>  /*timer*/
#include <asm/uaccess.h>  /*jiffies*/
#include <linux/input.h>

#define ZLINE_NUM	4    /* 定义键盘的行数 */
#define ZROW_NUM	4    /*矩阵键盘的列*/
#define ZKEY_NUM	ZLINE_NUM * ZROW_NUM

#define BUTTON_DOWN 	1
#define BUTTON_UP		0

struct button_desc {
    int num;
    int code;
    char value;	
};

struct button_irq_desc {
    int pin;
	unsigned int irq;
    int pin_setting1;
	int pin_setting2;
    int number;
    char *name;	
};


static struct button_desc button_info[] = {
    {1,KEY_F1, BUTTON_UP},  
    {2,KEY_F2, BUTTON_UP},
    {3,KEY_F3, BUTTON_UP},
    {4,KEY_F4, BUTTON_UP},

    {5,KEY_F5, BUTTON_UP},
    {6,KEY_F6, BUTTON_UP},
    {7,KEY_1, BUTTON_UP},  
    {8,KEY_4, BUTTON_UP},

    {9,KEY_7, BUTTON_UP},
    {10,KEY_C,BUTTON_UP},
    {11,KEY_G, BUTTON_UP},
    {12,KEY_K, BUTTON_UP},

    {13,KEY_2, BUTTON_UP},  
    {14,KEY_5, BUTTON_UP},
    {15,KEY_8, BUTTON_UP},
    {16,KEY_D, BUTTON_UP},

    {17,KEY_H, BUTTON_UP},
    {18,KEY_L, BUTTON_UP},
    {19,KEY_3, BUTTON_UP},  
    {20,KEY_6, BUTTON_UP},

    
};


/* 矩阵按键的列 */
static struct button_irq_desc button_io_row[] = {
	{S3C2410_GPG5, IRQ_EINT0, S3C2410_GPIO_INPUT, S3C2410_GPIO_OUTPUT, 0, "ROW1"},  
	{S3C2410_GPG6, IRQ_EINT0, S3C2410_GPIO_INPUT, S3C2410_GPIO_OUTPUT, 1, "ROW2"},
	{S3C2410_GPG7, IRQ_EINT0, S3C2410_GPIO_INPUT, S3C2410_GPIO_OUTPUT, 2, "ROW3"},
	{S3C2410_GPG11, IRQ_EINT0, S3C2410_GPIO_INPUT, S3C2410_GPIO_OUTPUT, 3, "ROW4"},
};

/* 矩阵按键的行 */
static struct button_irq_desc button_io_line[] = {
    {S3C2410_GPB5, IRQ_EINT0, S3C2410_GPIO_INPUT  ,S3C2410_GPIO_OUTPUT  ,0, "LINE1"},  
	{S3C2410_GPB6, IRQ_EINT0, S3C2410_GPIO_INPUT  ,S3C2410_GPIO_OUTPUT  ,1, "LINE2"},
	{S3C2410_GPB7, IRQ_EINT0, S3C2410_GPIO_INPUT  ,S3C2410_GPIO_OUTPUT  ,2, "LINE3"},
	{S3C2410_GPB8, IRQ_EINT0, S3C2410_GPIO_INPUT  ,S3C2410_GPIO_OUTPUT  ,3, "LINE4"},
};


/******************************************函数声明**********************************/
static void work_func(struct work_struct *work);
static irqreturn_t matrix_button_irq(int irq, void *dev_id);
void timer_function(unsigned long para);
static int input_open(struct input_dev *dev);

#endif /* _MYBUTTON_ */
