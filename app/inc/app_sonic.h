#ifndef __APP_SONIC_H
#define __APP_SONIC_H

#include "main.h"



/////////////////SONIC转换任务信息//////////////////

#define SONIC_STK_SIZE 		256  //任务堆栈大小	
extern OS_TCB SONIC_TaskTCB;  //任务控制块
extern CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //任务堆栈	


OS_ERR sonic_task_create (void);
void sonic_task (void *p_arg);

void falling_catch (void);
void rising_catch (void);



#ifdef __cplusplus
extern "C"
{
#endif
/* c语言内容*/
#ifdef __cplusplus
}
#endif



#endif 
