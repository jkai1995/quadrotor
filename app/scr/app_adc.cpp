#include "app_adc.h"


////#define LEFT_X_AXIS  11  //左侧x方向对应AD通道11
////#define LEFT_Y_AXIS  10
////#define RIGHT_X_AXIS  9
////#define RIGHT_Y_AXIS  8
#define CH_BATTERY  0

void adc_task (void *p_arg);

OS_TCB ADC_TaskTCB;  //任务控制块
CPU_STK ADC_TASK_STK[ADC_STK_SIZE];  //任务堆栈	


//创建此任务			 
//返回参数 : 错误代码
OS_ERR adc_task_create (void)
{
	  OS_ERR err;
		OSTaskCreate((OS_TCB 	* )&ADC_TaskTCB,		
				 (CPU_CHAR	* )"Task2 adc", 		
                 (OS_TASK_PTR )adc_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )ADC_TASK_PRIO,     
                 (CPU_STK   * )&ADC_TASK_STK[0],	
                 (CPU_STK_SIZE)ADC_STK_SIZE/10,	
                 (CPU_STK_SIZE)ADC_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
									
		return err;
}	

ADC_ROCKER rocker;
float battery;
//任务代码		 
//输入参数 p_arg 首次执行参数地址
void adc_task (void *p_arg)
{
	OS_ERR err;
	u16 batt[10];
	u8 i;
	u32 he;
	Buzz* buzz = Buzz::getInstance();
	Adc_Init();
	
	for(he = 0;he <20;he++)//初始化batt数组 暂时借用变量he
	{
		for(i = 9;i>0;i--)
		{
			batt[i] = batt[i-1];
		}
		batt[0] = Get_Adc(CH_BATTERY);
	}
	
	while(1)
	{
		for(i = 9;i>0;i--)
		{
			batt[i] = batt[i-1];
		}
		batt[0] = Get_Adc(CH_BATTERY);
		
		for(i = 0,he = 0;i<10;i++)
		{
			he += batt[i];
		}
		
		battery = 5.314*3.3*he/10/4096;
		
		if(battery < (float)10.6)/////////////低压报警
			buzz->set_buzz_mod(BUZZ_LOW_POWER);
		
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}
}
