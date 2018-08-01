#include "app_lcd.h"


OS_TCB LCD_TaskTCB;  //任务控制块
CPU_STK LCD_TASK_STK[LCD_STK_SIZE];  //任务堆栈	



OS_ERR lcd_task_create (void)
{
		OS_ERR err;
		OSTaskCreate((OS_TCB 	* )&LCD_TaskTCB,		
				 (CPU_CHAR	* )"Task3 lcd", 		
                 (OS_TASK_PTR )lcd_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LCD_TASK_PRIO,     
                 (CPU_STK   * )&LCD_TASK_STK[0],	
                 (CPU_STK_SIZE)LCD_STK_SIZE/10,	
                 (CPU_STK_SIZE)LCD_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
									
		return err;
}

void lcd_task (void *p_arg)
{
	OS_ERR err;
	u8 str[9][20];  //存储字符串
	Site_t site;  //设置液晶屏起点
	u8 i;
	LCD_init();
	//TIM2_PWM_Init(1000-1,84-1);
	//TIM_SetAutoreload(TIM2,1000-1);
	//TIM_SetCompare3(TIM2,300);
	
	site.x = 0;
	while(1)
	{
//		i=0;
//		sprintf((char *)str[i],"pitch:%8.2f",gest_6050.pitch);
//		site.y = 16*i;
//		LCD_str(site,str[i],WHITE,BLACK);
//		i=1;
//		sprintf((char *)str[i],"roll :%8.2f",gest_6050.roll);
//		site.y = 16*i;
//		LCD_str(site,str[i],WHITE,BLACK);
//		i=2;
//		sprintf((char *)str[i],"yaw  :%8.2f",gest_6050.yaw);
//		site.y = 16*i;
//		LCD_str(site,str[i],WHITE,BLACK);
		
		i=0;
		sprintf((char *)str[i],"batter:%8.2f",battery);
		site.y = 16*i;
		LCD_str(site,str[i],WHITE,BLACK);
		i=2;
		sprintf((char *)str[i],"heigh:%8d",heigh_mm);
		site.y = 16*i;
		LCD_str(site,str[i],WHITE,BLACK);
		i=4;
		sprintf((char *)str[i],"servo:%8d",st_camera_data.yaw);
		site.y = 16*i;
		LCD_str(site,str[i],WHITE,BLACK);
		
		
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err); //延时100ms
	}
}




