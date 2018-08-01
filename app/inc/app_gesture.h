#ifndef __APP_GESTURE_H
#define __APP_GESTURE_H

#include "main.h"

/////////////////Һ����������Ϣ//////////////////

typedef struct
{
	s16 	pit_zero;
	s16 	roll_zero;
	s16		yaw_zero;
	s16		acc_yaw_zero;
	float pitch_acc_zero;
	float roll_acc_zero;
	float yaw_acc_zero;
}GYRO_ZERO_TypeDef;

#define GESTURE_STK_SIZE 		256  //�����ջ��С	
extern OS_TCB GESTURE_TaskTCB;  //������ƿ�
extern CPU_STK GESTURE_TASK_STK[GESTURE_STK_SIZE];  //�����ջ	
extern u8 mpu_err;
extern GYRO_ZERO_TypeDef gesture_zero;
extern s16 gyro_roll_m[4];
extern s16 gyro_pit_m[4];
extern s16 gyro_yaw_m[4];

OS_ERR gesture_task_create (void);
void gesture_task (void *p_arg);
void Gyro_zero_get(void);
#endif
