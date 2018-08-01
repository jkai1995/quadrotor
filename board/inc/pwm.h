#ifndef _PWM_H
#define _PWM_H
#include "stm32f4xx_conf.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
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
extern ST_RunTime_us_TypeDef run_time_us; //������������ʱ��
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