#ifndef __APP_SONIC_H
#define __APP_SONIC_H

#include "main.h"



/////////////////SONICת��������Ϣ//////////////////

#define SONIC_STK_SIZE 		256  //�����ջ��С	
extern OS_TCB SONIC_TaskTCB;  //������ƿ�
extern CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //�����ջ	


//u32 heigh_mm;
//s16 speed_h;
//extern u8 sonic_lost_time;////������ʧЧ������





OS_ERR sonic_task_create (void);
u32 get_sonic_heightmm();
//void sonic_task (void *p_arg);




#ifdef __cplusplus
extern "C"
{
#endif
/* c��������*/
#ifdef __cplusplus
}
#endif



#endif 
