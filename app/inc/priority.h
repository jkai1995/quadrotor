#ifndef __PRIORITY_H
#define __PRIORITY_H

//管理uc/OS的优先级

//ALIENTEK 探索者STM32F407开发板 UCOSIII实验
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级3：开始任务 start_task()

// 电机驱动任务
#define MOTOR_TASK_PRIO   	4  //任务优先级

// 姿态获取任务
#define GESTURE_TASK_PRIO   5  //任务优先级

// 超声波测距任务
#define SONIC_TASK_PRIO			6  //任务优先级
// 无线传输
#define NRF2401_TASK_PRIO 	7  //任务优先级

// 模数转换
#define ADC_TASK_PRIO				8  //任务优先级

// 蜂鸣器任务
#define BUZZ_TASK_PRIO			9  //任务优先级

// 按键
#define KEY_TASK_PRIO				10  //任务优先级

// 液晶显示
#define LCD_TASK_PRIO				11  //任务优先级

//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

#endif
