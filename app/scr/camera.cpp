#include "camera.h"

ST_CameraData_TypeDef st_camera_data;
#define BUFMAX 5
static u8 buf[BUFMAX];

static u16 time_out; 


extern "C" /////�жϺ�����Ҫ��c���Ա�������
{
		void USART2_IRQHandler(void) //����2�жϷ������
		{
				#if SYSTEM_SUPPORT_OS  //ʹ��UCOS����ϵͳ
					OSIntEnter();    
				#endif
			u8 i;
					if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
					{
						
						buf[0] = USART_ReceiveData(USART2);
						if(buf[0] == 76)
						{
								for(i = 1;i <BUFMAX;i++)
								{
									time_out = 0;
									while(!USART_GetFlagStatus(USART2,USART_FLAG_RXNE) && time_out<400)//�ȴ����ջ������� ����ʱ������
										time_out++; //�ȴ����ռĴ�����
									
									if(time_out >= 400)
										break;/////��ʱ��ֱ���˳�
									
									buf[i] = USART_ReceiveData(USART2);						
								}
								
								if(time_out<400)//���ݴ�������
								{
									st_camera_data.roll = (buf[1]<<8) + (buf[2]);	///��Χ�ڡ�936	
									(st_camera_data.roll> 1200)?(st_camera_data.roll = 1200):(st_camera_data.roll);//////�޷�
									(st_camera_data.roll<-1200)?(st_camera_data.roll = -1200):(st_camera_data.roll);
									st_camera_data.yaw = (buf[3]<<8) + (buf[4]);  ///����250
								}
								
					  }
						
					} 
					
				#if SYSTEM_SUPPORT_OS  
					OSIntExit();    	//�˳��ж�
				#endif
		} 
			//////////
}

