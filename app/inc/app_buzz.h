#ifndef __APP_BUZZ_H
#define __APP_BUZZ_H

#include "main.h"

/////////////////液晶屏任务信息//////////////////

#define BUZZ_STK_SIZE 		128  //任务堆栈大小	
extern OS_TCB BUZZ_TaskTCB;  //任务控制块
extern CPU_STK BUZZ_TASK_STK[BUZZ_STK_SIZE];  //任务堆栈	

typedef enum   
{
	BUZZ_TURN_ON ,///开机提示
	BUZZ_UNLOCK,  ///解锁提示
	BUZZ_LOCK,///////////上锁提示
	BUZZ_OUT_CONTR,//////遥控信号超时
	BUZZ_ERR,//////////错误提示
	BUZZ_NONE,/////////无操作
	BUZZ_LOW_POWER,/////////低压
	BUZZ_TR_ATTACHABLE,
	BUZZ_TR_BLOCKED,
}BUZZ_MODE;//蜂鸣器提示音

class Buzz
{
public:
	u16  rhythm_ms;      //延时节奏 毫秒
	u16  period_us;		 //周期 微秒
  u16  half_period_us;		 //半周期 微秒 
	float volume;		 //音量
//构造函数
	Buzz(void);
//析构函数
	~Buzz(void);
//访问接口
	void play (u16 const *pt_halfper,float const *pt_vo,u16 const *Rhy,u8 const num);//音符半周期表 音量表 节奏表   响几声
  void play (u16 const *pt_halfper,u8 num);//音符半周期表 响几声  默认 音量 节奏
private:


	void set_buzz(u16 const h_pe,float const vo); //设置蜂鸣器 半周期（us） 音量(%)
	void drive_buzz(u16 per,float vo);
};


void set_buzz_mod(BUZZ_MODE md);

OS_ERR BUZZ_task_create (void);
void BUZZ_task (void *p_arg);
#endif
