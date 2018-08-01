#include "app_sonic.h"

#define TRIGER_OUT PAout(3)
#define SOUND_SPEED_M_S  340///�������� 340m/s
void sonic_task (void *p_arg);




OS_TCB SONIC_TaskTCB;  //������ƿ�
CPU_STK SONIC_TASK_STK[SONIC_STK_SIZE];  //�����ջ	


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
	
	Sonic sonic;//ʵ������
	
	while(1)
	{
		sonic.triger();//��ʼһ�β��
		
		p_msg = OSTaskQPend(0,///�޳�ʱʱ��
												OS_OPT_PEND_BLOCKING,///�ȴ��ڼ���������
												&msg_size,///��Ϣ��С//�ֽ�
												NULL,////��ʱ���
												&err);
		
		sonic.set_time(*(u32*)p_msg);
		
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}
}






////////
//////���캯��������ʼ��TIM5 ���벶׽����
//////1us����һ��
Sonic::Sonic(void)
{
	TIM5_PWM_Init(0xFFFFFFFF,84-1);
	Sonic_triger_init();
}
//////////
//////��������
Sonic::~Sonic(void)
{
}

void Sonic::triger(void)
{
	level = LOW;///��ʱΪ�͵�ƽ
	rising_catch();///��ʼ�����ز�׽
	TRIGER_OUT = 1; ////15us���崥��
	delay_us(15);
	TRIGER_OUT = 0;
}
///

void Sonic::set_time(u32 count)
{
	time_us = count;
	distance_mm = SOUND_SPEED_M_S*time_us/1000/2;
}
////���������ز�׽
void rising_catch (void)
{

	TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); //ͨ��2 ����Ϊ�����ز���
	TIM_Cmd(TIM5,ENABLE );
}

///////�����½��ز�׽
void falling_catch (void)
{
	TIM_Cmd(TIM5,DISABLE ); 	//�رն�ʱ��5
	TIM_SetCounter(TIM5,0);		//��ռ�����
	TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); //ͨ��2 ����Ϊ�����ز���
	TIM_Cmd(TIM5,ENABLE );
}

/////////////
/////////��׽�ж�
void TIM5_IRQHandler(void)
{
	OSIntEnter();
	OS_ERR 					err;

		if(level == LOW) ////���֮ǰ�ǵ͵�ƽ  �����������������
		{////��׽�������أ�
			falling_catch();//��ʼ��׽�½���
			level = HIGH;///��ƽ��Ϊ�ߵ�ƽ
		}
		else if(level == HIGH)////���֮ǰ�Ǹߵ�ƽ  ������������½���
		{////��׽���½���
			tim_count = TIM_GetCapture1(TIM5);//��ȡ��ǰ�Ĳ���ֵ.
			TIM_Cmd(TIM5,DISABLE ); ///�رն�ʱ��
			
			/////////////////////////
			///////////////////�����񷢲���Ϣ
			OSTaskQPost(&SONIC_TaskTCB,
									(void *)&tim_count,
									4,///�ĸ��ֽ�
									OS_OPT_POST_FIFO,
									&err);
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC2); //����жϱ�־λ
	OSIntExit();
}

