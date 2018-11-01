#ifndef __APP_SONIC_H
#define __APP_SONIC_H

#include "main.h"



/////////////////SONIC转换任务信息//////////////////

#define SONIC_STK_SIZE 		256  //任务堆栈大小	
extern OS_TCB SONIC_TaskTCB;  //任务控制块
extern CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //任务堆栈	


//u32 heigh_mm;
//s16 speed_h;
//extern u8 sonic_lost_time;////超声波失效周期数





OS_ERR sonic_task_create (void);
u32 get_sonic_heightmm();
//void sonic_task (void *p_arg);




#ifdef __cplusplus
extern "C"
{
#endif
/* c语言内容*/
#ifdef __cplusplus
}
#endif



#endif 
