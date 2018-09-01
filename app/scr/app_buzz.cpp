#include "app_buzz.h"


OS_TCB BUZZ_TaskTCB;  //任务控制块
CPU_STK BUZZ_TASK_STK[BUZZ_STK_SIZE];  //任务堆栈	

BUZZ_MODE lastb_mode = BUZZ_NONE;
BUZZ_MODE buzz_mode = BUZZ_NONE;


OS_ERR BUZZ_task_create (void)
{
		OS_ERR err;
		OSTaskCreate((OS_TCB 	* )&BUZZ_TaskTCB,		
				 (CPU_CHAR	* )"Task3 BUZZ", 		
                 (OS_TASK_PTR )BUZZ_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )BUZZ_TASK_PRIO,     
                 (CPU_STK   * )&BUZZ_TASK_STK[0],	
                 (CPU_STK_SIZE)BUZZ_STK_SIZE/10,	
                 (CPU_STK_SIZE)BUZZ_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
									
		return err;
}
///////

//const u16 low_half_per[7]={1911,1702,1516,1431,1275,1136,1012};
const u16 hig_half_per[7]={477,425,379,357,318,284,253};
const u16 low_half_per_rever[7]={1012,1136,1275,1431,1516,1702,1911};

const u16 low_power_per[2] = {379,477};
//float volume[7] = {0.1,0.1,0.1,0.1,0.1,0.1,0.1};

void BUZZ_task (void *p_arg)
{
	OS_ERR err;
	Buzz buzz;

	
	while(1)
	{

		if(buzz_mode == BUZZ_TURN_ON)
		{
			buzz.rhythm_ms = 100;
			buzz.volume = 0.1;
			buzz.play(hig_half_per,4);///播放hig_half_per列表声音 4个音符
			buzz_mode = BUZZ_NONE;
		}
		else if(buzz_mode == BUZZ_ERR)
		{
			buzz.rhythm_ms = 100;
			buzz.volume = 0.1;
			buzz.play(hig_half_per+5,1);
			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //延时1s
		}
		else if(buzz_mode == BUZZ_LOCK)
		{
			buzz.rhythm_ms = 300;
			buzz.volume = 0.1;
			buzz.play(low_half_per_rever,3);
			buzz_mode = BUZZ_NONE;
		}
		else if(buzz_mode == BUZZ_UNLOCK)
		{
			buzz.rhythm_ms = 200;
			buzz.volume = 0.1;
			buzz.play(hig_half_per+4,3);
			buzz_mode = BUZZ_NONE;
		}
		else if( buzz_mode == BUZZ_OUT_CONTR)
		{
			buzz.rhythm_ms = 400;
			buzz.volume = 0.02;
			buzz.play(low_half_per_rever+2,5);
			buzz_mode = BUZZ_NONE;
		}
		else if(buzz_mode == BUZZ_LOW_POWER)
		{
			buzz.rhythm_ms = 600;
			buzz.volume = 0.8;
			buzz.play(low_power_per,2);
			buzz_mode = BUZZ_NONE;
		}
		

		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err); //延时5ms

	}
}
//////
////设置蜂鸣器响声模式
void set_buzz_mod(BUZZ_MODE md)
{
	lastb_mode = buzz_mode;
	buzz_mode = md;
}


Buzz::Buzz(void)
{
	//TIM5_PWM_Init(1000-1,84-1);
	
	period_us = 1000;
}

void Buzz::set_buzz(u16  h_pe,float vo)
{
	half_period_us = h_pe;     //半周期
	period_us = 2*half_period_us; //周期
	volume = vo;            //音量
	drive_buzz(period_us,volume);
}


void Buzz::drive_buzz(u16 per,float vo)
{
	TIM_SetAutoreload(TIM1,per-1); //设置周期
	TIM_SetCompare1(TIM1,per*vo);//设置音量
}



/////////////播放  表格式
void Buzz::play (u16 const *pt_halfper,float const *pt_vo,u16 const *Rhy,u8 const num)//音符半周期表 音量表 节奏表   响几声
{
	OS_ERR err;
	u8 i;
	for(i = 0; i < num; i++)
	{
		set_buzz(*(pt_halfper + i),*(pt_vo + i));
		OSTimeDlyHMSM(0,0,0,*(Rhy+i),OS_OPT_TIME_PERIODIC,&err); //延时  ms
	}
	set_buzz(1000,0);//关闭
}


/////////////播放  固定 音量 和 节奏
void Buzz::play (u16 const *pt_halfper,u8 num)//音符半周期表 响几声 
{
	OS_ERR err;
	u8 i;
	for(i = 0; i < num; i++)
	{
		set_buzz(*(pt_halfper + i),volume);
		OSTimeDlyHMSM(0,0,0,rhythm_ms,OS_OPT_TIME_PERIODIC,&err); ///延时rhythm_ms
	}
	set_buzz(1000,0);
}
//////////





