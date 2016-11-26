#ifndef __RC522_DRIVER_H
#define __RC522_DRIVER_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/delay.h>

#include <asm/uaccess.h>
#include <asm/gpio.h>

#include <mach/regs-gpio.h>
#include <mach/hardware.h>

#include "common.h"

typedef struct AUTHSTATE	/* 验证卡片密码用的 */
{
	uint8_t cmd;	/* 命令字 */
	uint8_t addr;	/* 块地址 */
	uint8_t key[10];	/* 键值 */
	uint8_t car_str[20];	/* 卡片序列号 */
} *g_Auth_State;


typedef struct WRITEDAAR	/* 写块 */
{
	uint8_t addr;	/* 块地址 */
	uint8_t data[20];	/* 写入的值 */
} *g_Write_Addr;


typedef struct BAKVALUE
{
	uint8_t s_addr;		/* 源块 */
	uint8_t d_addr;		/* 目的块 */
} *g_Bak_Value;




/* 字符设备操作函数集 */
static ssize_t rc522_read(struct file *filp, char __user * buf, size_t size, loff_t *pos);
static ssize_t rc522_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos);
static int rc522_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int rc522_init(void);
static void rc522_exit(void);



#endif
