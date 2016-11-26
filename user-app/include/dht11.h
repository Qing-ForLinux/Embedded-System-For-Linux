#ifndef __DHT11_H
#define __DHT11_H

#include "common.h"

#define DHT11_DEV	"/dev/dht_dev"

/* 数据结构 */
struct DHT11_Data
{
  uint8_t temperature;
  uint8_t humidity;
};

int dht11_init();
int dht11_open(char *dev_name);
int dht11_read(int fd, struct DHT11_Data *dht11_data);
int dht11_close(int fd);


#endif