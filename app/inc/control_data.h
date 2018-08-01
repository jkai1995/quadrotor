#ifndef __CONTROL_DATA_H
#define __CONTROL_DATA_H

#include "main.h"

#define LIMIT(data,max,min) {(data > max) ? (data = max):(data);(data < min) ? (data = min):(data);}

typedef struct
{
	u8 cmd; 		//命令
	u16 lift;   //油门
	s16 pitch;  //俯仰
	s16 roll;   //滚动
	s16 yaw;    //偏航
} Control_Input_TypeDef; //遥控输入

typedef struct   
{
	s16 	motor_1;
	s16   motor_2;
	s16   motor_3;
	s16   motor_4;
}ST_MOTOR; //电机升力

typedef struct   
{
	s16 	Upit;
	s16   Uroll;
	s16   Uyaw;
	s16   Ulift;
}ST_Control_Output_TypeDef;//控制信号输出

typedef struct  //单轴的控制数据
{
	s16 err[10];
	s16 diff;
	/////////系数
	float KP;
	float KD;
	float KI;
	//////////输出
	s16   P_out;
	s16   D_out;
	float   I_out;
	////////////总输出
	s16		Sum_out;
}ST_PID_Controler_TypeDef;//////PID控制器

typedef struct
{
	ST_PID_Controler_TypeDef	OuterRing;
	ST_PID_Controler_TypeDef	InnerRing;
}ST_DoubleClosedLoopPID_TypeDef;//////////双闭环PID控制器

typedef struct   
{
	ST_PID_Controler_TypeDef		pitch;
	ST_PID_Controler_TypeDef		roll;
	ST_PID_Controler_TypeDef		yaw;

}ST_Balance_Argue_TypeDef;//控制信号输出



typedef enum   
{
	LOCKED ,
	UN_LOCKED
}MOTOR_LOCK;//控制信号输出

typedef struct
{
	s16 pitch[5];
	s16 roll[5];
	s16 yaw[5];
} Err_Memory_TypeDef;
extern Control_Input_TypeDef control_data_struct;
extern ST_MOTOR st_motor;
extern MOTOR_LOCK motor_lock;
extern ST_DoubleClosedLoopPID_TypeDef st_HeighControler;///高度控制
extern u16 Lift_FeedForward; ////油门前馈

extern ST_Balance_Argue_TypeDef Argue_Gyro_ring;
extern ST_Balance_Argue_TypeDef Argue_ACC_ring;   //角度环参数
extern Err_Memory_TypeDef Gyro_err_memory;
extern Err_Memory_TypeDef Acc_err_memory;        //角度err记录
extern ST_Control_Output_TypeDef st_control_out;
void lode_send_data (u8 buf[]);
void lode_rece_data (u8 buf[]);

void Gyro_Ring_pitch (s16 given_pitch);
void Gyro_Ring_roll (s16 given_roll);
void Gyro_Ring_yaw (s16 given_yaw);
void balance_control (void);
void motor_solve (void);
void Acc_Ring_pitch (s16 given_pitch);
void Acc_Ring_roll (s16 given_roll);
void Acc_Ring_yaw (s16 given_yaw);
void heigh_control (u16 goal,u16 heigh,s16 speed);
#ifdef __cplusplus
extern "C"
{
#endif
/* c语言内容*/
/* c语言内容*/
#ifdef __cplusplus
}
#endif





#endif 
