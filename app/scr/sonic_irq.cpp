#include "sonic_irq.h"

#define PI 3.14159265358979f
#define TRIGER_OUT PAout(3)
#define TIME_COUNT_MAX 25000
void sonic_task (void *p_arg);
void triger(void);
void set_time(u32 count);



OS_TCB SONIC_TaskTCB;  //������ƿ�
CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //�����ջ	


LEVEL level = LOW;
u32 heigh_mm;
u32 tim_count;//unit us����ʱ�����صĸߵ�ƽʱ��
u8 sonic_time_out;//���㳬ʱ����


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
TIM3_time_start();//////////////ʱ�������ʼ	
		if(sonic_time_out ==0)  ////�ϴβ���û�з�������ſ�ʼ����
		{
			triger();//��ʼһ�β��
			
			
			p_msg = OSTaskQPend(6,///�ȴ�6������
													OS_OPT_PEND_BLOCKING,///�ȴ��ڼ���������
													&msg_size,///��Ϣ��С//�ֽ�
													NULL,////��ʱ���
													&err);

			if(tim_count > 0)//�Ѿ���⵽������
			{
				set_time(tim_count);
			}
			else
			{
				sonic_time_out = 1; /////������
				tim_count = TIME_COUNT_MAX; ///���ʱ�����
				set_time(tim_count);

			}
		}
		else
		{
			tim_count = TIME_COUNT_MAX;
			set_time(tim_count);
			sonic_time_out++;
			if(sonic_time_out>5)///�������5�����ڳ�����ʾ�������Ѿ�������½���
			{
				sonic_time_out = 0;///ǿ�����²���
			}
		}
		
	//	OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
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

