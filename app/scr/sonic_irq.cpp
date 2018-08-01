#include "sonic_irq.h"

#define PI 3.14159265358979f
#define TRIGER_OUT PAout(3)
#define SOUND_SPEED_M_S  340///定义声速 340m/s
void sonic_task (void *p_arg);
void triger(void);
void set_time(u32 count);



OS_TCB SONIC_TaskTCB;  //任务控制块
CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //任务堆栈	

LEVEL level = LOW;
u32 heigh_mm;
u32 tim_count;
u8 sonic_time_out;
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
		run_time_us.time3 = TIM3_time_over();////////////时间测量结束
		TIM3_time_start();//////////////时间测量开始	
//		run_time_us.time0 = 0;
//		run_time_us.time1 = 0;
//		run_time_us.time2 = 0;
//		run_time_us.time4 = 0;
		if(sonic_time_out ==0)  ////上次测量没有发生错误才开始测量
		{
			triger();//开始一次测距
			run_time_us.time4 = TIM3_time_over();////////////时间测量结束
			
			p_msg = OSTaskQPend(3,///等待3个周期
													OS_OPT_PEND_BLOCKING,///等待期间阻塞任务
													&msg_size,///消息大小//字节
													NULL,////无时间戳
													&err);
run_time_us.time2 = TIM3_time_over();////////////时间测量结束
			if(tim_count > 0)//已经检测到了数据
			{
				set_time(tim_count);
			}
			else
			{
				sonic_time_out = 1; /////检测出错
				tim_count = 25000; ///清楚时间测量
				set_time(tim_count);

			}
		}
		else
		{
			tim_count = 30000;
			set_time(tim_count);
			sonic_time_out++;
			if(sonic_time_out>5)///如果连续5个周期出错，表示可能是已经错过了下降沿
			{
				sonic_time_out = 0;///强行重新测量
			}
		}

		//	run_time_us.time0 = 0;
			//run_time_us.time1 = 0;
		///	run_time_us.time2 = 0;
		
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
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

///
float speed_h_diff; ///由超声波差分得到的速度
s16 speed_h = 0;
float speed_h_real;
float acc_yaw_zero;
u32 sonic_time_raw[10]; /////////超声波测量原始数据
u32 sonic_time[10]; 
s32 err_diff[6];
s32 distance_mem_real[5];
u8 sonic_lost_time;////超声波失效周期数
s16 fault_range;///由于超生波失效时间延长，极值滤波容错范围需要扩大
void set_time(u32 count)
{
	static u8 init = 0;
	static u32 distance_mem[3];
	u16 i;
	
	init++;
	if(init>50)
		init = 50;
//	float weight;
	
	for(i = 9;i > 0;i--)
	{
		sonic_time_raw[i] = sonic_time_raw[i-1];
	}
	sonic_time_raw[0] = count;
////////////////////
//////第一步 极值滤波//////////
////////////////	
	err_diff[5] = err_diff[4];
	err_diff[4] = err_diff[3];
	err_diff[3] = err_diff[2];
	err_diff[2] = err_diff[1];
	err_diff[1] = err_diff[0];
	err_diff[0] = (s16)(sonic_time_raw[0] - (sonic_time[0] + (s16)(speed_h_real*0.62)));///新数据和旧数据差距
	

	
	
////	///极值滤波  //////极值越大  新数据的可靠性就越小，是噪声的可能性越大，所以比重就越小
////	if(err_diff[0]>300) //极值超过一定数值以后，新数据完全没有可靠性
////		weight = 0;
////	else if(err_diff[0]<200)
////		weight = 1; ////极值够小，可靠性为1
////	else
////	{
////		weight = 1 - (float)(err_diff[0] - 200)/100;
////	}
////	
////	if(err_diff[0]>200 && err_diff[1]>200 && err_diff[2]>200 && err_diff[3]>200 && err_diff[4]>200 && err_diff[5]>200)//////////如果连续多长都差距很大，说明高度确实是有跳变
////	{
////		weight = 1;
////	}
////	
	for(i = 9;i > 0;i--)
	{
		sonic_time[i] = sonic_time[i-1];
	}
	
	fault_range = sonic_lost_time*60;///随着超声波失效实验延长逐渐扩大容错范围
	if(fault_range>1000)
		fault_range = 1000;
	fault_range += 450;
	
//////	if(abs(err_diff[0]) > fault_range && init>=50)///init==50表示已经完成初始化，，没有完成初始化时不进行滤波
//////	{
//////		sonic_lost_time++;////超声波失效周期数
//////		if(sonic_lost_time>50)
//////			sonic_lost_time = 50;
//////		if(sonic_lost_time<=20)////////丢失时间过长就停止积分
//////		sonic_time[0] = sonic_time[0] + (s16)(speed_h_real*0.62);
//////	}
//////	else
//////	{
//////		sonic_lost_time = 0;
//////		sonic_time[0] = sonic_time_raw[0];
//////	}
	if(sonic_time_raw[0]<20000)
	{
		sonic_time[0] = sonic_time_raw[0];
	}
	
	
	if(sonic_time[0]<700)
		sonic_time[0] = 700;
	
///	sonic_time[0] = sonic_time_raw[0]*weight + sonic_time[0]*(1-weight);

////////////////////////////////////////
//////第二步 超声波时间转换为高度//////////
////////////////////////////////////////	
	
	distance_mem[2] = distance_mem[1];
	distance_mem[1] = distance_mem[0];
	distance_mem[0] = SOUND_SPEED_M_S*sonic_time[0]/1000/2; ////////转换为高度后的原始数据
	
	
////////////////////////////////////////
//////第三步 高度中值滤波//////////
////////////////////////////////////////	
	
	distance_mem_real[4] = distance_mem_real[3];
	distance_mem_real[3] = distance_mem_real[2];
	distance_mem_real[2] = distance_mem_real[1];
	distance_mem_real[1] = distance_mem_real[0];////////中值滤波后的高度
	
	//heigh_mm
	/////中值滤波
	if((distance_mem[0]>=distance_mem[1]&&distance_mem[0]<=distance_mem[2])||(distance_mem[0]>=distance_mem[2]&&distance_mem[0]<=distance_mem[1]))
	{
		 distance_mem_real[0] = distance_mem[0];
	}
	else if((distance_mem[1]>=distance_mem[0]&&distance_mem[1]<=distance_mem[2])||(distance_mem[1]>=distance_mem[2]&&distance_mem[1]<=distance_mem[0]))
	{
		 distance_mem_real[0] = distance_mem[1];
	}
	else
	{
		 distance_mem_real[0] = distance_mem[2];
	}
	////////////////////////////////////////
//////第四步 低通滤波//////////
////////////////////////////////////////	
////低通滤波作为输出
	if(init>=50)
		heigh_mm = distance_mem_real[0]*0.5 + distance_mem_real[1]*0.3 + distance_mem_real[2]*0.2;
	else//////////完成初始化后再低通滤波
		heigh_mm = distance_mem_real[0];
	distance_mem_real[0] = heigh_mm;
	
/////////求垂直速度
////超声波差分作为垂直速度
	if(init>=50)////完成初始化后再求速度
	{
		speed_h_diff = 2.55*(distance_mem_real[0] - distance_mem_real[4]);
	
		if(speed_h_diff >= 800)
		speed_h_diff = 800;
	else if(speed_h_diff <= -800)
		speed_h_diff = -800;
	
	/////坐标变换，，求垂直加速度
AngleTransforming((float)gest_6050.pitch_acc,(float)-gest_6050.roll_acc,(float)-gest_6050.yaw_acc,gest_6050.pitch,gest_6050.yaw,gest_6050.roll);
	
///加速度积分//再///融合差分的垂直速度///
	speed_h_real -=((st_coordinate_earth.Z - st_coordinate_earth_zero.Z) * 0.01);
	if(sonic_time_raw[0]<20000)///
		speed_h_real += ((speed_h_diff - speed_h_real)*0.04);
	else
		speed_h_real += ((speed_h_diff - speed_h_real)*0.01);
	
	

	
	//speed_h += (gest_6050.yaw_acc/cos_v-acc_yaw_zero)*0.0038 + (speed_h_diff - speed_h)*0.2;
	speed_h = (s16)speed_h_diff;
	//speed_h_real = 
	}
}
/////////////

