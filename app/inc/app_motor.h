
#ifndef __APP_MOTOR_H
#define __APP_MOTOR_H

#include "main.h"


/////////////////Һ����������Ϣ//////////////////

#define MOTOR_STK_SIZE 		128  //�����ջ��С	
extern OS_TCB MOTOR_TaskTCB;  //������ƿ�
extern CPU_STK MOTOR_TASK_STK[MOTOR_STK_SIZE];  //�����ջ	

void motor_input (s16 motor1,s16 motor2,s16 motor3,s16 motor4);


OS_ERR MOTOR_task_create (void);


void MOTOR_task (void *p_arg);
void MOTOR_init (void);
void motor_input (s16 motor1,s16 motor2,s16 motor3,s16 motor4);
#endif
