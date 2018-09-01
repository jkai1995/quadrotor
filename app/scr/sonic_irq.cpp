#include "sonic_irq.h"

#define PI 3.14159265358979f
#define TRIGER_OUT PAout(3)
#define TIME_COUNT_MAX 25000
void sonic_task (void *p_arg);
void triger(void);
void set_time(u32 count);



OS_TCB SONIC_TaskTCB;  //任务控制块
CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //任务堆栈	


LEVEL level = LOW;
u32 heigh_mm;
u32 tim_count;//unit us，计时器返回的高电平时间
u8 sonic_time_out;//计算超时次数


//创建此任务			 
//返回参数 : 错误代码
OS_ERR sonic_task_create (void)
{
	  OS_ERR err;
		OSTaskCreate((OS_TCB 	* )&SONIC_TaskTCB,		
				 (CPU_CHAR	* )"Task2 sonic", 		
                 (OS_TASK_PTR )sonic_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )SONIC_TASK_PRIO,     
                 (CPU_STK   * )&SONIC_TASK_STK[0],	
                 (CPU_STK_SIZE)SONIC_STK_SIZE/10,	
                 (CPU_STK_SIZE)SONIC_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
									
		return err;
}	

//任务代码		 
//输入参数 p_arg 首次执行参数地址

void sonic_task (void *p_arg)
{
	OS_ERR 					err;
	void					*	p_msg;
	OS_MSG_SIZE 		msg_size;
	TIM5_PWM_Init(0xFFFFFFFF,84-1);
	Sonic_triger_init();
	
	OSTaskSemPend(0,
								OS_OPT_PEND_BLOCKING,
								NULL,
								&err);

	while(1)
	{
TIM3_time_start();//////////////时间测量开始	
		if(sonic_time_out ==0)  ////上次测量没有发生错误才开始测量
		{
			triger();//开始一次测距
			
			
			p_msg = OSTaskQPend(6,///等待6个周期
													OS_OPT_PEND_BLOCKING,///等待期间阻塞任务
													&msg_size,///消息大小//字节
													NULL,////无时间戳
													&err);

			if(tim_count > 0)//已经检测到了数据
			{
				set_time(tim_count);
			}
			else
			{
				sonic_time_out = 1; /////检测出错
				tim_count = TIME_COUNT_MAX; ///清楚时间测量
				set_time(tim_count);

			}
		}
		else
		{
			tim_count = TIME_COUNT_MAX;
			set_time(tim_count);
			sonic_time_out++;
			if(sonic_time_out>5)///如果连续5个周期出错，表示可能是已经错过了下降沿
			{
				sonic_time_out = 0;///强行重新测量
			}
		}
		
	//	OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}
}
//////



void triger(void)
{
	rising_catch();///开始上升沿捕捉
	level = LOW;
	TRIGER_OUT = 1; ////15us脉冲触发
	delay_us(15);
	TRIGER_OUT = 0;
}
////////////////

////设置上升沿捕捉
void rising_catch (void)
{

	TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); //通道2 设置为上升沿捕获
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC2|TIM_IT_Update); //清除中断标志位
	TIM_ITConfig(TIM5,TIM_IT_CC2,ENABLE);//开启触发中断
	TIM_Cmd(TIM5,DISABLE );///关闭计数器  等到检测到上升沿在打开  
	TIM_SetCounter(TIM5,0);		//清空计数器
	tim_count = 0; ///清楚时间测量
	
}
//////////////

///////设置下降沿捕捉
void falling_catch (void)
{
	
	TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Falling); //通道2 设置为下降沿捕获
	
}
/////////////

		/////////////
		/////////捕捉中断
extern "C"
{

		void TIM5_IRQHandler(void)
		{
			OSIntEnter();
			OS_ERR 					err;
			if(TIM_GetITStatus(TIM5,TIM_IT_CC2) == SET)
			{
				if(level == LOW) ////如果之前是低电平  则此跳变沿是上升沿
				{////捕捉到上升沿，
					run_time_us.time0 = TIM3_time_over();////////////时间测量结束
					TIM_Cmd(TIM5,ENABLE );//开始计数
					falling_catch();//开始捕捉下降沿
					level = HIGH;///电平变为高电平
							
				}
				else if(level == HIGH)////如果之前是高电平  则此跳变沿是下降沿
				{////捕捉到下降沿
					sonic_time_out = 0;///清除检测超时标志
					TIM_Cmd(TIM5,DISABLE ); ///关闭定时器
					tim_count = TIM_GetCapture2(TIM5);//获取当前的捕获值.
					TIM_SetCounter(TIM5,0);		//清空计数器
					level = LOW;////电平变为低电平
					TIM_ITConfig(TIM5,TIM_IT_CC2,DISABLE);//关闭触发中断
					/////////////////////////
					///////////////////向任务发布消息
					run_time_us.time1 = TIM3_time_over();////////////时间测量结束	
					OSTaskQPost(&SONIC_TaskTCB,
											(void *)&tim_count,
											4,///四个字节
											OS_OPT_POST_FIFO,
											&err);
				}
				
			}
			else
			{
				TIM_ITConfig(TIM5,TIM_IT_CC2,DISABLE);//关闭触发中断
			}
			TIM_ClearITPendingBit(TIM5, TIM_IT_CC2|TIM_IT_Update); //清除中断标志位
			OSIntExit();
		}
}
////////////////


s16 speed_h = 0;


#define P1 0.1745
#define P2 (-140.1)
void set_time(u32 us)
{
	if(us < TIME_COUNT_MAX)
	{
		heigh_mm = us * P1 + P2;
	}
}
/////////////

