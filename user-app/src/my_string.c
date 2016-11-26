#include "my_string.h"

/**********************************************************************************************
 * 函数名称:    char_to_string
 * 函数参数:    p_source		源数组
 * 函数返回值:  成功返回复制之后的数组, 失败返回NULL
 * 函数描述:    字符数组转换成字符串
 *********************************************************************************************/


uint8_t* char_to_string(uint8_t *p_source)  
{  
    uint8_t i = 0;
    uint8_t p_tmp[4] = {0};
    uint8_t *p_des = (uint8_t *)malloc(sizeof(uint8_t) * 64);
    if (p_des == NULL){ /* ??ˇ??? */
        printf("p_des is NULL\r\n");
        return NULL;
    }
    while (p_source[i] != '\0'){    /* 如果已经是数组的最后一个元素则退出 */
        sprintf(p_tmp, "%x", p_source[i]);  /* 将16进制转换成字符串 */
        strcat(p_des, p_tmp);   /* 连接字符串 */
        bzero(p_tmp,strlen(p_tmp));
        i++;
    }
    return p_des;
}

/**********************************************************************************************
 * 函数名称:    char_table_copy
 * 函数参数:    p_source		源数组
 *		        p_target		   目标数组
 *				num				拷贝的字节数
 * 函数返回值:  void
 * 函数描述:    字符数组拷贝
 *********************************************************************************************/
void char_table_copy(uint8_t *p_source, uint8_t *p_target, uint8_t num)
{
	uint8_t i = num;
	for (i = 0; i < num; i++)
		p_target[i] = p_source[i];
}










