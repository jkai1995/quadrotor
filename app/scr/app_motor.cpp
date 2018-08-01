#include "app_motor.h"

#define MOTOR_ZERO 1000

OS_TCB MOTOR_TaskTCB;  //������ƿ�
CPU_STK MOTOR_TASK_STK[MOTOR_STK_SIZE];  //�����ջ	



OS_ERR MOTOR_task_create (void)
{
		OS_ERR err;

		OSTaskCreate((OS_TCB 	* )&MOTOR_TaskTCB,		
				 (CPU_CHAR	* )"Task3 MOTOR", 		
                 (OS_TASK_PTR )MOTOR_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MOTOR_TASK_PRIO,     
                 (CPU_STK   * )&MOTOR_TASK_STK[0],	
                 (CPU_STK_SIZE)MOTOR_STK_SIZE/10,	
                 (CPU_STK_SIZE)MOTOR_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
									
		return err;
}


	
void MOTOR_task (void *p_arg)
{
	OS_ERR err;
  TIM9_PWM_Init(2500-1,168-1);
	TIM12_PWM_Init(2500-1,84-1);
	
	TIM_SetCompare1(TIM9, MOTOR_ZERO);
	TIM_SetCompare1(TIM12,MOTOR_ZERO);
	TIM_SetCompare2(TIM12,MOTOR_ZERO);
	TIM_SetCompare2(TIM9, MOTOR_ZERO);

//	TIM1_PWM_Init(1000-1,84-1);
//	TIM_SetAutoreload(TIM1,1000-1);
//	TIM_SetCompare1(TIM1,50);
	

	
	while(1)
	{
		balance_control();
		motor_input(st_motor.motor_1,st_motor.motor_2,st_motor.motor_3,st_motor.motor_4);
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err); //��ʱ5ms
	}
}


void MOTOR_init (void)
{
	TIM9_PWM_Init(2500-1,168-1);
	TIM12_PWM_Init(2500-1,84-1);
	
	TIM_SetCompare1(TIM9, MOTOR_ZERO);
	TIM_SetCompare1(TIM12,MOTOR_ZERO);
	TIM_SetCompare2(TIM12,MOTOR_ZERO);
	TIM_SetCompare2(TIM9, MOTOR_ZERO);
}
///�������
///���� ���ݽṹ ST_MOTOR  �ĸ�������������
void motor_input (s16 motor1,s16 motor2,s16 motor3,s16 motor4)
{
	
	(motor1 < 0   )?(motor1 = 0    ):(motor1);  //�����޷�
	(motor1 > 1000)?(motor1 = 1000):(motor1);
	
	(motor2 < 0   )?(motor2 = 0    ):(motor2);  //�����޷�
	(motor2 > 1000)?(motor2 = 1000):(motor2);
	
	(motor3 < 0   )?(motor3 = 0    ):(motor3);  //�����޷�
	(motor3 > 1000)?(motor3 = 1000):(motor3);
	
	(motor4 < 0   )?(motor4 = 0    ):(motor4);  //�����޷�
	(motor4 > 1000)?(motor4 = 1000):(motor4);
	
//	motor1 = 0;
//	motor2 = 0;
//	motor3 = 0;
//	motor4 = 0;
	
	
	
	TIM_SetCompare1(TIM9, motor1 + MOTOR_ZERO);
	TIM_SetCompare1(TIM12,motor3 + MOTOR_ZERO);
	TIM_SetCompare2(TIM12,motor4 + MOTOR_ZERO);
	TIM_SetCompare2(TIM9, motor2 + MOTOR_ZERO);
}



