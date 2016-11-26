#include "my_string.h"

/**********************************************************************************************
 * ��������:    char_to_string
 * ��������:    p_source		Դ����
 * ��������ֵ:  �ɹ����ظ���֮�������, ʧ�ܷ���NULL
 * ��������:    �ַ�����ת�����ַ���
 *********************************************************************************************/


uint8_t* char_to_string(uint8_t *p_source)  
{  
    uint8_t i = 0;
    uint8_t p_tmp[4] = {0};
    uint8_t *p_des = (uint8_t *)malloc(sizeof(uint8_t) * 64);
    if (p_des == NULL){ /* ??��??? */
        printf("p_des is NULL\r\n");
        return NULL;
    }
    while (p_source[i] != '\0'){    /* ����Ѿ�����������һ��Ԫ�����˳� */
        sprintf(p_tmp, "%x", p_source[i]);  /* ��16����ת�����ַ��� */
        strcat(p_des, p_tmp);   /* �����ַ��� */
        bzero(p_tmp,strlen(p_tmp));
        i++;
    }
    return p_des;
}

/**********************************************************************************************
 * ��������:    char_table_copy
 * ��������:    p_source		Դ����
 *		        p_target		   Ŀ������
 *				num				�������ֽ���
 * ��������ֵ:  void
 * ��������:    �ַ����鿽��
 *********************************************************************************************/
void char_table_copy(uint8_t *p_source, uint8_t *p_target, uint8_t num)
{
	uint8_t i = num;
	for (i = 0; i < num; i++)
		p_target[i] = p_source[i];
}










