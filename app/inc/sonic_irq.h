#ifndef __APP_SONIC_H
#define __APP_SONIC_H

#include "main.h"



/////////////////SONIC转换任务信息//////////////////

#define SONIC_STK_SIZE 		256  //任务堆栈大小	
extern OS_TCB SONIC_TaskTCB;  //任务控制块
extern CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //任务堆栈	


extern u32 heigh_mm;
extern s16 speed_h;
extern float acc_yaw_zero;
extern float speed_h_diff;
extern float speed_h_real;
extern u32 sonic_time_raw[10]; /////////超声波测量原始数据
extern u32 sonic_time[10];   ///////////滤波后的超生波测量时间 
extern u8 sonic_lost_time;////超声波失效周期数


typedef enum
{
	LOW,
	HIGH
}LEVEL;


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
