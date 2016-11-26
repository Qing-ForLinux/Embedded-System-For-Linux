#ifndef __DHT11_DRIVER_H
#define __DHT11_DRIVER_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <asm/uaccess.h>

#include <mach/regs-gpio.h>
#include <mach/hardware.h>

#define uint32_t unsigned int
#define uint16_t unsigned short int
#define uint8_t unsigned char


void dht11_rst(void);
uint8_t dht11_check(void);
uint8_t dht11_read_bit(void);
uint8_t dht11_read_byte(void);
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi);
uint8_t dht11_init(void);


#define g_DHT_GPIO_INP 	 S3C2410_GPF1_INP
#define g_DHT_GPIO_OUTP  S3C2410_GPF1_OUTP
#define  g_DHT_GPIO      S3C2410_GPF1

#endif