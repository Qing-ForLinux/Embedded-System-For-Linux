#ifndef __MQ_H
#define __MQ_H

#include "common.h"


#define mq_dev_name "/dev/MQ_2"


int mq_read(uint8_t *gas_concentration);

#endif