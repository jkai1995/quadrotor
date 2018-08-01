#include "sonic_irq.h"

#define PI 3.14159265358979f
#define TRIGER_OUT PAout(3)
#define SOUND_SPEED_M_S  340///�������� 340m/s
void sonic_task (void *p_arg);
void triger(void);
void set_time(u32 count);



OS_TCB SONIC_TaskTCB;  //������ƿ�
CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //�����ջ	

LEVEL level = LOW;
u32 heigh_mm;
u32 tim_count;
u8 sonic_time_out;
//����������			 
//���ز��� : �������
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

//�������		 
//������� p_arg �״�ִ�в�����ַ

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
		run_time_us.time3 = TIM3_time_over();////////////ʱ���������
		TIM3_time_start();//////////////ʱ�������ʼ	
//		run_time_us.time0 = 0;
//		run_time_us.time1 = 0;
//		run_time_us.time2 = 0;
//		run_time_us.time4 = 0;
		if(sonic_time_out ==0)  ////�ϴβ���û�з�������ſ�ʼ����
		{
			triger();//��ʼһ�β��
			run_time_us.time4 = TIM3_time_over();////////////ʱ���������
			
			p_msg = OSTaskQPend(3,///�ȴ�3������
													OS_OPT_PEND_BLOCKING,///�ȴ��ڼ���������
													&msg_size,///��Ϣ��С//�ֽ�
													NULL,////��ʱ���
													&err);
run_time_us.time2 = TIM3_time_over();////////////ʱ���������
			if(tim_count > 0)//�Ѿ���⵽������
			{
				set_time(tim_count);
			}
			else
			{
				sonic_time_out = 1; /////������
				tim_count = 25000; ///���ʱ�����
				set_time(tim_count);

			}
		}
		else
		{
			tim_count = 30000;
			set_time(tim_count);
			sonic_time_out++;
			if(sonic_time_out>5)///�������5�����ڳ�����ʾ�������Ѿ�������½���
			{
				sonic_time_out = 0;///ǿ�����²���
			}
		}

		//	run_time_us.time0 = 0;
			//run_time_us.time1 = 0;
		///	run_time_us.time2 = 0;
		
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}
}
//////



void triger(void)
{
	rising_catch();///��ʼ�����ز�׽
	level = LOW;
	TRIGER_OUT = 1; ////15us���崥��
	delay_us(15);
	TRIGER_OUT = 0;
}
////////////////

////���������ز�׽
void rising_catch (void)
{

	TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); //ͨ��2 ����Ϊ�����ز���
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC2|TIM_IT_Update); //����жϱ�־λ
	TIM_ITConfig(TIM5,TIM_IT_CC2,ENABLE);//���������ж�
	TIM_Cmd(TIM5,DISABLE );///�رռ�����  �ȵ���⵽�������ڴ�  
	TIM_SetCounter(TIM5,0);		//��ռ�����
	tim_count = 0; ///���ʱ�����
	
}
//////////////

///////�����½��ز�׽
void falling_catch (void)
{
	
	TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Falling); //ͨ��2 ����Ϊ�½��ز���
	
}
/////////////

		/////////////
		/////////��׽�ж�
extern "C"
{

		void TIM5_IRQHandler(void)
		{
			OSIntEnter();
			OS_ERR 					err;
			if(TIM_GetITStatus(TIM5,TIM_IT_CC2) == SET)
			{
				if(level == LOW) ////���֮ǰ�ǵ͵�ƽ  �����������������
				{////��׽�������أ�
					run_time_us.time0 = TIM3_time_over();////////////ʱ���������
					TIM_Cmd(TIM5,ENABLE );//��ʼ����
					falling_catch();//��ʼ��׽�½���
					level = HIGH;///��ƽ��Ϊ�ߵ�ƽ
							
				}
				else if(level == HIGH)////���֮ǰ�Ǹߵ�ƽ  ������������½���
				{////��׽���½���
					sonic_time_out = 0;///�����ⳬʱ��־
					TIM_Cmd(TIM5,DISABLE ); ///�رն�ʱ��
					tim_count = TIM_GetCapture2(TIM5);//��ȡ��ǰ�Ĳ���ֵ.
					TIM_SetCounter(TIM5,0);		//��ռ�����
					level = LOW;////��ƽ��Ϊ�͵�ƽ
					TIM_ITConfig(TIM5,TIM_IT_CC2,DISABLE);//�رմ����ж�
					/////////////////////////
					///////////////////�����񷢲���Ϣ
					run_time_us.time1 = TIM3_time_over();////////////ʱ���������	
					OSTaskQPost(&SONIC_TaskTCB,
											(void *)&tim_count,
											4,///�ĸ��ֽ�
											OS_OPT_POST_FIFO,
											&err);
				}
				
			}
			else
			{
				TIM_ITConfig(TIM5,TIM_IT_CC2,DISABLE);//�رմ����ж�
			}
			TIM_ClearITPendingBit(TIM5, TIM_IT_CC2|TIM_IT_Update); //����жϱ�־λ
			OSIntExit();
		}
}
////////////////

///
float speed_h_diff; ///�ɳ�������ֵõ����ٶ�
s16 speed_h = 0;
float speed_h_real;
float acc_yaw_zero;
u32 sonic_time_raw[10]; /////////����������ԭʼ����
u32 sonic_time[10]; 
s32 err_diff[6];
s32 distance_mem_real[5];
u8 sonic_lost_time;////������ʧЧ������
s16 fault_range;///���ڳ�����ʧЧʱ���ӳ�����ֵ�˲��ݴ�Χ��Ҫ����
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
//////��һ�� ��ֵ�˲�//////////
////////////////	
	err_diff[5] = err_diff[4];
	err_diff[4] = err_diff[3];
	err_diff[3] = err_diff[2];
	err_diff[2] = err_diff[1];
	err_diff[1] = err_diff[0];
	err_diff[0] = (s16)(sonic_time_raw[0] - (sonic_time[0] + (s16)(speed_h_real*0.62)));///�����ݺ;����ݲ��
	

	
	
////	///��ֵ�˲�  //////��ֵԽ��  �����ݵĿɿ��Ծ�ԽС���������Ŀ�����Խ�����Ա��ؾ�ԽС
////	if(err_diff[0]>300) //��ֵ����һ����ֵ�Ժ���������ȫû�пɿ���
////		weight = 0;
////	else if(err_diff[0]<200)
////		weight = 1; ////��ֵ��С���ɿ���Ϊ1
////	else
////	{
////		weight = 1 - (float)(err_diff[0] - 200)/100;
////	}
////	
////	if(err_diff[0]>200 && err_diff[1]>200 && err_diff[2]>200 && err_diff[3]>200 && err_diff[4]>200 && err_diff[5]>200)//////////��������೤�����ܴ�˵���߶�ȷʵ��������
////	{
////		weight = 1;
////	}
////	
	for(i = 9;i > 0;i--)
	{
		sonic_time[i] = sonic_time[i-1];
	}
	
	fault_range = sonic_lost_time*60;///���ų�����ʧЧʵ���ӳ��������ݴ�Χ
	if(fault_range>1000)
		fault_range = 1000;
	fault_range += 450;
	
//////	if(abs(err_diff[0]) > fault_range && init>=50)///init==50��ʾ�Ѿ���ɳ�ʼ������û����ɳ�ʼ��ʱ�������˲�
//////	{
//////		sonic_lost_time++;////������ʧЧ������
//////		if(sonic_lost_time>50)
//////			sonic_lost_time = 50;
//////		if(sonic_lost_time<=20)////////��ʧʱ�������ֹͣ����
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
//////�ڶ��� ������ʱ��ת��Ϊ�߶�//////////
////////////////////////////////////////	
	
	distance_mem[2] = distance_mem[1];
	distance_mem[1] = distance_mem[0];
	distance_mem[0] = SOUND_SPEED_M_S*sonic_time[0]/1000/2; ////////ת��Ϊ�߶Ⱥ��ԭʼ����
	
	
////////////////////////////////////////
//////������ �߶���ֵ�˲�//////////
////////////////////////////////////////	
	
	distance_mem_real[4] = distance_mem_real[3];
	distance_mem_real[3] = distance_mem_real[2];
	distance_mem_real[2] = distance_mem_real[1];
	distance_mem_real[1] = distance_mem_real[0];////////��ֵ�˲���ĸ߶�
	
	//heigh_mm
	/////��ֵ�˲�
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
//////���Ĳ� ��ͨ�˲�//////////
////////////////////////////////////////	
////��ͨ�˲���Ϊ���
	if(init>=50)
		heigh_mm = distance_mem_real[0]*0.5 + distance_mem_real[1]*0.3 + distance_mem_real[2]*0.2;
	else//////////��ɳ�ʼ�����ٵ�ͨ�˲�
		heigh_mm = distance_mem_real[0];
	distance_mem_real[0] = heigh_mm;
	
/////////��ֱ�ٶ�
////�����������Ϊ��ֱ�ٶ�
	if(init>=50)////��ɳ�ʼ���������ٶ�
	{
		speed_h_diff = 2.55*(distance_mem_real[0] - distance_mem_real[4]);
	
		if(speed_h_diff >= 800)
		speed_h_diff = 800;
	else if(speed_h_diff <= -800)
		speed_h_diff = -800;
	
	/////����任������ֱ���ٶ�
AngleTransforming((float)gest_6050.pitch_acc,(float)-gest_6050.roll_acc,(float)-gest_6050.yaw_acc,gest_6050.pitch,gest_6050.yaw,gest_6050.roll);
	
///���ٶȻ���//��///�ںϲ�ֵĴ�ֱ�ٶ�///
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

