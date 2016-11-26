#ifndef __MY_STRING_H
#define __MY_STRING_H

#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


uint8_t* char_to_string(uint8_t *p_source);
void char_table_copy(uint8_t *p_source, uint8_t *p_target, uint8_t num);


#endif