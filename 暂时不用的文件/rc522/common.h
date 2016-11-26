#ifndef __COMMON_H
#define __COMMON_H

#include <asm/gpio.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>

#define     RC522_SDA  	S3C2410_GPB5		    //SDA
#define     RC522_SCK  	S3C2410_GPB6         //SCK
#define     RC522_MISO 	S3C2410_GPB8         //MOSI
#define     RC522_MOSI 	S3C2410_GPB7         //MISO
#define     RC522_RST  	S3C2410_GPF1         //RST


#define uint32_t unsigned int
#define uint16_t unsigned short int
#define uint8_t uint8_t

#endif