#ifndef _PWM_H
#define _PWM_H
#include "stm32f4xx_conf.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
typedef struct
{
	u16 time0;
	u16 time1;
	u16 time2;
	u16 time3;
	u16 time4;
}ST_RunTime_us_TypeDef;
extern ST_RunTime_us_TypeDef run_time_us; //计量程序运行时间
void TIM14_PWM_Init(u32 arr,u32 psc);
void TIM2_PWM_Init(u32 arr,u32 psc);
void TIM5_PWM_Init(u32 arr,u32 psc);
void TIM9_PWM_Init(u32 arr,u32 psc);
void TIM12_PWM_Init(u32 arr,u32 psc);
void TIM1_PWM_Init(u32 arr,u32 psc);
void Sonic_triger_init (void);
void TIM3_time_start (void);
u16 TIM3_time_over (void);
#endif
