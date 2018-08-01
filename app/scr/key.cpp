#include "key.h"

 u8  key_number = 0; //�������±�־

void KEY_Scan(void)
{	 
	static u8 key_state=UNPRESSED;

 if(key_state == UNPRESSED && key_number == 0) ///////ɨ���Ƿ��а�������
	{
				(KEY_GET(KEY0)==PRESSED)?(key_number += 1) : (key_number += 0);
				(KEY_GET(KEY1)==PRESSED)?(key_number += 2) : (key_number += 0);
				(KEY_GET(KEY2)==PRESSED)?(key_number += 3) : (key_number += 0);
				(KEY_GET(KEY3)==PRESSED)?(key_number += 4) : (key_number += 0);
				(KEY_GET(KEY4)==PRESSED)?(key_number += 5) : (key_number += 0);			
		if(key_number > 0)
		  key_state = PRESSED;
	}
	else if(1               ////ɨ�谴���Ժ��Ƿ�̧��
				  &&KEY_GET(KEY0)==UNPRESSED
				  &&KEY_GET(KEY1)==UNPRESSED
				  &&KEY_GET(KEY2)==UNPRESSED
				  &&KEY_GET(KEY3)==UNPRESSED 
	        &&KEY_GET(KEY4)==UNPRESSED
				  )
	{
				key_state=UNPRESSED; 	
	}

}

u8 Get_key_value(void)
{
	u8 i = key_number;
	key_number = 0;
	return i;
}

OS_TCB KEY_TaskTCB;  //������ƿ�
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];  //�����ջ	
//����������			 
//���ز��� : �������
OS_ERR key_task_create (void)
{
	  OS_ERR err;
		//����TASK1����
	OSTaskCreate((OS_TCB 	* )&KEY_TaskTCB,
				 (CPU_CHAR	* )"Task1 key", 		
                 (OS_TASK_PTR )key_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )KEY_TASK_PRIO,     
                 (CPU_STK   * )&KEY_TASK_STK[0],	
                 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
									
		return err;
}	
//����1��������
void key_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	Key_Init();			//������ʼ��

	while(1)
	{
		KEY_Scan();
		key = Get_key_value();
		key = key;
		num++;
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}
}


