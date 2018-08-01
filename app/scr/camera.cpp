#include "camera.h"

ST_CameraData_TypeDef st_camera_data;
#define BUFMAX 5
static u8 buf[BUFMAX];

static u16 time_out; 


extern "C" /////中断函数需要用c语言编译运行
{
		void USART2_IRQHandler(void) //串口2中断服务程序
		{
				#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
					OSIntEnter();    
				#endif
			u8 i;
					if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
					{
						
						buf[0] = USART_ReceiveData(USART2);
						if(buf[0] == 76)
						{
								for(i = 1;i <BUFMAX;i++)
								{
									time_out = 0;
									while(!USART_GetFlagStatus(USART2,USART_FLAG_RXNE) && time_out<400)//等待接收缓冲区满 但有时间限制
										time_out++; //等待接收寄存器满
									
									if(time_out >= 400)
										break;/////超时则直接退出
									
									buf[i] = USART_ReceiveData(USART2);						
								}
								
								if(time_out<400)//数据传输正常
								{
									st_camera_data.roll = (buf[1]<<8) + (buf[2]);	///范围在±936	
									(st_camera_data.roll> 1200)?(st_camera_data.roll = 1200):(st_camera_data.roll);//////限幅
									(st_camera_data.roll<-1200)?(st_camera_data.roll = -1200):(st_camera_data.roll);
									st_camera_data.yaw = (buf[3]<<8) + (buf[4]);  ///正负250
								}
								
					  }
						
					} 
					
				#if SYSTEM_SUPPORT_OS  
					OSIntExit();    	//退出中断
				#endif
		} 
			//////////
}

