#ifndef __APP_SONIC_H
#define __APP_SONIC_H

#include "main.h"



/////////////////SONICת��������Ϣ//////////////////

#define SONIC_STK_SIZE 		256  //�����ջ��С	
extern OS_TCB SONIC_TaskTCB;  //������ƿ�
extern CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //�����ջ	


OS_ERR sonic_task_create (void);
void sonic_task (void *p_arg);

void falling_catch (void);
void rising_catch (void);



#ifdef __cplusplus
extern "C"
{
#endif
/* c��������*/
#ifdef __cplusplus
}
#endif



#endif 
