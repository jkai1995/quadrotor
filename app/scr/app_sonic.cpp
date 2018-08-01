#include "app_sonic.h"

#define TRIGER_OUT PAout(3)
#define SOUND_SPEED_M_S  340///定义声速 340m/s
void sonic_task (void *p_arg);




OS_TCB SONIC_TaskTCB;  //任务控制块
CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //任务堆栈	


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
	
	Sonic sonic;//实例化类
	
	while(1)
	{
		sonic.triger();//开始一次测距
		
		p_msg = OSTaskQPend(0,///无超时时间
												OS_OPT_PEND_BLOCKING,///等待期间阻塞任务
												&msg_size,///消息大小//字节
												NULL,////无时间戳
												&err);
		
		sonic.set_time(*(u32*)p_msg);
		
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}
}






////////
//////构造函数，，初始化TIM5 输入捕捉功能
//////1us计数一次
Sonic::Sonic(void)
{
	TIM5_PWM_Init(0xFFFFFFFF,84-1);
	Sonic_triger_init();
}
//////////
//////析构函数
Sonic::~Sonic(void)
{
}

void Sonic::triger(void)
{
	level = LOW;///此时为低电平
	rising_catch();///开始上升沿捕捉
	TRIGER_OUT = 1; ////15us脉冲触发
	delay_us(15);
	TRIGER_OUT = 0;
}
///

void Sonic::set_time(u32 count)
{
	time_us = count;
	distance_mm = SOUND_SPEED_M_S*time_us/1000/2;
}
////设置上升沿捕捉
void rising_catch (void)
{

	TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); //通道2 设置为上升沿捕获
	TIM_Cmd(TIM5,ENABLE );
}

///////设置下降沿捕捉
void falling_catch (void)
{
	TIM_Cmd(TIM5,DISABLE ); 	//关闭定时器5
	TIM_SetCounter(TIM5,0);		//清空计数器
	TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); //通道2 设置为上升沿捕获
	TIM_Cmd(TIM5,ENABLE );
}

/////////////
/////////捕捉中断
void TIM5_IRQHandler(void)
{
	OSIntEnter();
	OS_ERR 					err;

		if(level == LOW) ////如果之前是低电平  则此跳变沿是上升沿
		{////捕捉到上升沿，
			falling_catch();//开始捕捉下降沿
			level = HIGH;///电平变为高电平
		}
		else if(level == HIGH)////如果之前是高电平  则此跳变沿是下降沿
		{////捕捉到下降沿
			tim_count = TIM_GetCapture1(TIM5);//获取当前的捕获值.
			TIM_Cmd(TIM5,DISABLE ); ///关闭定时器
			
			/////////////////////////
			///////////////////向任务发布消息
			OSTaskQPost(&SONIC_TaskTCB,
									(void *)&tim_count,
									4,///四个字节
									OS_OPT_POST_FIFO,
									&err);
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC2); //清除中断标志位
	OSIntExit();
}

