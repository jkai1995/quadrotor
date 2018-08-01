#ifndef __PRIORITY_H
#define __PRIORITY_H

//����uc/OS�����ȼ�

//ALIENTEK ̽����STM32F407������ UCOSIIIʵ��
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�3����ʼ���� start_task()

// �����������
#define MOTOR_TASK_PRIO   	4  //�������ȼ�

// ��̬��ȡ����
#define GESTURE_TASK_PRIO   5  //�������ȼ�

// �������������
#define SONIC_TASK_PRIO			6  //�������ȼ�
// ���ߴ���
#define NRF2401_TASK_PRIO 	7  //�������ȼ�

// ģ��ת��
#define ADC_TASK_PRIO				8  //�������ȼ�

// ����������
#define BUZZ_TASK_PRIO			9  //�������ȼ�

// ����
#define KEY_TASK_PRIO				10  //�������ȼ�

// Һ����ʾ
#define LCD_TASK_PRIO				11  //�������ȼ�

//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()

#endif
