#ifndef __APP_BUZZ_H
#define __APP_BUZZ_H

#include "main.h"

/////////////////Һ����������Ϣ//////////////////

#define BUZZ_STK_SIZE 		128  //�����ջ��С	
extern OS_TCB BUZZ_TaskTCB;  //������ƿ�
extern CPU_STK BUZZ_TASK_STK[BUZZ_STK_SIZE];  //�����ջ	

typedef enum   
{
	BUZZ_TURN_ON ,///������ʾ
	BUZZ_UNLOCK,  ///������ʾ
	BUZZ_LOCK,///////////������ʾ
	BUZZ_OUT_CONTR,//////ң���źų�ʱ
	BUZZ_ERR,//////////������ʾ
	BUZZ_NONE,/////////�޲���
	BUZZ_LOW_POWER,/////////��ѹ
	BUZZ_TR_ATTACHABLE,
	BUZZ_TR_BLOCKED,
}BUZZ_MODE;//��������ʾ��

class Buzz
{
public:
	u16  rhythm_ms;      //��ʱ���� ����
	u16  period_us;		 //���� ΢��
  u16  half_period_us;		 //������ ΢�� 
	float volume;		 //����
//���캯��
	Buzz(void);
//��������
	~Buzz(void);
//���ʽӿ�
	void play (u16 const *pt_halfper,float const *pt_vo,u16 const *Rhy,u8 const num);//���������ڱ� ������ �����   �켸��
  void play (u16 const *pt_halfper,u8 num);//���������ڱ� �켸��  Ĭ�� ���� ����
private:


	void set_buzz(u16 const h_pe,float const vo); //���÷����� �����ڣ�us�� ����(%)
	void drive_buzz(u16 per,float vo);
};


void set_buzz_mod(BUZZ_MODE md);

OS_ERR BUZZ_task_create (void);
void BUZZ_task (void *p_arg);
#endif
