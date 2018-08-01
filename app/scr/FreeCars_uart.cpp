/**
  ******************************************************************************
  * @file    FreeCars_uart.c
  * @author  FreeCars�۸�
  * @version NULL
  * @date    2014/11/11
  * @brief 	 FreeCars_uart C file.(For XS128)
  *   site:   Http://FreeCars.taobao.com
  *   QQȺ��  384273254��149168724
  *   ��Ȩ��  �˴���ΪFreeCars��λ��Э����룬��������ʹ�ã�Ҳ����������ҵ��;�����뱣���˶����֣�
  *   tips:   ǿ�ҽ���С�����ʹ��FreeCars��Ʒ������ģ����װ��������λ�������￪ʼ��
  ******************************************************************************
  */
#include "FreeCars_uart.h"

/************һ���Ǹ���FreeCars��λ��Э���޸ĵģ�����ı�**************/
uint8 uSendBuf[UartDataNum*2]={0};
SerialPortType SerialPortRx;
double UartData[9] = {0};
uint8 FreeCarsDataNum=UartDataNum*2;
//uint8 FreeCarsCount=0;
/**************************/


/*
��ѯ������һ֡����
����ʱ�������ݳ����й�
�����Է����ж����������Ե���
*/
void sendDataToScope(void)
{
	uint8 i,sum=0; 

  FREECAES_SCI_SEND_CHANNEL(251);
	FREECAES_SCI_SEND_CHANNEL(109);
	FREECAES_SCI_SEND_CHANNEL(37);
	sum+=(251);      //ȫ�����ݼ���У��
	sum+=(109);
	sum+=(37);
	for(i=0;i<FreeCarsDataNum;i++)
	{
		FREECAES_SCI_SEND_CHANNEL(uSendBuf[i]);
		sum+=uSendBuf[i];         //ȫ�����ݼ���У��
	}
 	FREECAES_SCI_SEND_CHANNEL(sum);
}

/*
��ĳ��ͨ���������
adr��ͨ��
date������-32768~32767
*/
void push(uint8 chanel,uint16 dat)
{
  uSendBuf[chanel*2]=dat/256;
  uSendBuf[chanel*2+1]=dat%256;
}


#define CASE6
extern u32 tim_count,heigh_mm;

extern u32 err_diff[4];
extern u32 distance_mem_real[5];
extern LEVEL level;
void WirelessDateSend (void)
{
	#ifdef CASE0
			push(0,Acc_err_memory.pitch[0]);  
			push(1,Acc_err_memory.roll[0]);
			push(2,Gyro_err_memory.pitch[0]);
			
			push(3,Gyro_err_memory.roll[0]);
			
			push(4,Argue_ACC_ring.pitch.Sum_out);
			push(5,Argue_ACC_ring.roll.Sum_out);                         
			
			push(6,Argue_Gyro_ring.pitch.Sum_out);
			push(7,Argue_Gyro_ring.roll.Sum_out);
			push(8,st_motor.motor_3);
			
			push(9,st_motor.motor_4);     
			
			push(10,0);
	#endif
	/////////////////
	
	
	#ifdef CASE1  ///////��̬ �߶� ��ع۲�
			push(0,(s16)(gest_6050.pitch*100));  
			push(1,(s16)(gest_6050.roll*100));
			push(2,(s16)(gest_6050.yaw*10));
			
			push(3,gest_6050.pitch_acc);
			push(4,gest_6050.roll_acc);
			push(5,gest_6050.yaw_acc);                         
			
			push(6,gest_6050.pitch_gyro);
			push(7,gest_6050.roll_gyro);			
			push(8,gest_6050.yaw_gyro);     
			
			push(9,heigh_mm);
			push(10,battery*100);
	#endif
	////////////////
		#ifdef CASE2  ///////�������� ʵ�� �۲�
		
			push(0,10*control_data_struct.pitch);
			push(1,10*control_data_struct.roll);
			push(2,10*control_data_struct.yaw); 
			
			push(3,(s16)(gest_6050.pitch*100));  
			push(4,(s16)(gest_6050.roll*100));
			push(5,(s16)((gest_6050.yaw-gesture_zero.yaw_acc_zero)*100));                    
			
			push(6,0);
			push(7,0);			
			push(8,control_data_struct.lift);     
			
			push(9,heigh_mm);
			push(10,battery*100);
	#endif
	/////////	
			#ifdef CASE3  ///////���� ����ƽ��
		
			push(0,10*control_data_struct.pitch);/////��������
			push(1,10*control_data_struct.roll);
	
			push(2,(s16)(gest_6050.pitch*100));   ////////ʵ����Ӧ
			push(3,(s16)(gest_6050.roll*100));                   
			
			push(4,Argue_ACC_ring.pitch.P_out); //�⻷P���
			push(5,gest_6050.pitch_gyro);			  ////���ٶ�
			push(6,Gyro_err_memory.pitch[0]);   //�ڻ�ƫ�� 
			push(7,Argue_Gyro_ring.pitch.P_out);   //�ڻ��������
			push(8,0);   
			
			push(9,0);
			push(10,battery*100);
	#endif
	/////////
		#ifdef CASE4  ///////���� ����
		
			push(0,10*control_data_struct.yaw); 
			push(1,(s16)((gest_6050.yaw-gesture_zero.yaw_acc_zero)*100));                    
			push(2,Argue_ACC_ring.yaw.Sum_out); 
			push(3,gest_6050.yaw_gyro);
			push(4,Gyro_err_memory.yaw[0]);
			push(5,Argue_Gyro_ring.yaw.P_out);
			push(6,Argue_Gyro_ring.yaw.D_out);
			push(7,(s16)Argue_Gyro_ring.yaw.I_out);
			
			push(8,Argue_Gyro_ring.yaw.Sum_out);     		
			push(9,heigh_mm);
			
			push(10,battery*100);
	#endif
/////////
		#ifdef CASE5  ///////////////�߶Ȳ����۲�	 �߶ȿ���
	
			push(0,control_data_struct.lift); //ת���ɸ߶Ⱥ��
			push(1,heigh_mm);  //��ֵ�˲����
			push(2,st_HeighControler.OuterRing.P_out);//�⻷����
			push(3,st_HeighControler.OuterRing.diff);///΢��
			push(4,st_HeighControler.OuterRing.Sum_out); //Ŀ���ٶ�
			push(5,st_HeighControler.InnerRing.err[0]);//�ڻ�ƫ��	
			push(6,st_HeighControler.InnerRing.P_out);//�������
			push(7,(s16)st_HeighControler.InnerRing.I_out);//�������
			push(8,Lift_FeedForward);   //ǰ��
			push(9,st_control_out.Ulift/4);//�����
			push(10,battery*100);	
	#endif
/////////


		#ifdef CASE6  ///////////////�߶Ȳ����۲�	
	
			push(0,sonic_time_raw[0]); //���ԭʼ����
			push(1,sonic_time[0]);  //��ֵ�˲����
			push(2,heigh_mm);//ת���ɸ߶Ⱥ��
			push(3,((s16)PAin(1))*1000);//��ֵ
			push(4,run_time_us.time4/10); //
			push(5,(s16)speed_h_real);//
			push(6,((s16)level)*1000);//ʵ���ٶ�
			push(7,run_time_us.time0/10);//��ʱ��⵽������
			push(8,run_time_us.time1/10);//�½���
			push(9,run_time_us.time2/10);   //��ʼ����
			
			push(10,run_time_us.time3/10); ///��ʱ��
			//push(10,sonic_lost_time*1000);	
	#endif
////////////
		#ifdef CASE7  ///////////////��ֱ�ٶ��ں�	
	
			push(0,sonic_time_raw[0]); //���ԭʼ����
			push(1,sonic_time[0]);  //��ֵ�˲����
			push(2,heigh_mm);//ת���ɸ߶Ⱥ��
			push(3,(s16)speed_h_diff);//
			push(4,(s16)speed_h_real); //
			push(5,(s16)(st_coordinate_earth.Z-st_coordinate_earth_zero.Z));//
			push(6,(s16)st_HeighControler.InnerRing.P_out);//ʵ���ٶ�
			push(7,(s16)st_HeighControler.InnerRing.I_out);//��ʱ��⵽������
			push(8,(s16)st_HeighControler.InnerRing.I_out);//�½���
			push(9,Lift_FeedForward);   //��ʼ����
			push(10,battery*100);	
	#endif
////////////

		#ifdef CASE8  ///////��������
		
			push(0,10*control_data_struct.pitch);///�Ƕ�����
			push(1,(s16)(gest_6050.pitch*100));////ʵ�ʽǶ�
			
			push(2,Argue_ACC_ring.pitch.Sum_out); //// ���ٶ�����
			push(3,(s16)(gest_6050.pitch_gyro)); //////ʵ�ʽ��ٶ�
			push(4,(s16)(Gyro_err_memory.pitch[0]));////�Ƕȶ����
			push(5,Argue_Gyro_ring.pitch.P_out);/////////���ٶȱ�����			
			
			push(6,(s16)Argue_Gyro_ring.pitch.I_out);////���ٶȻ�����
			push(7,Argue_Gyro_ring.pitch.Sum_out);   ////���ٶ����			
			push(8,0);
			push(9,(s16)(Argue_Gyro_ring.pitch.KI*10000));		
			push(10,battery*100);
	#endif
	/////////	
		#ifdef CASE9  ///////���� �������������
		
			push(0,10*control_data_struct.pitch);///�Ƕ�����
			push(1,(s16)(gest_6050.pitch*100));////ʵ�ʽǶ�
			
			push(2,10*control_data_struct.roll); //// ���ٶ�����
			push(3,(s16)(gest_6050.roll*100)); //////ʵ�ʽ��ٶ�
			push(4,(s16)(Gyro_err_memory.pitch[0]));////�Ƕȶ����
			push(5,Argue_Gyro_ring.pitch.P_out);/////////���ٶȱ�����			
			
			push(6,(s16)Argue_Gyro_ring.pitch.I_out);////���ٶȻ�����
			push(7,Argue_Gyro_ring.pitch.Sum_out);   ////���ٶ����			
			push(8,(s16)Argue_Gyro_ring.roll.I_out);
			push(9,(s16)(Argue_Gyro_ring.pitch.KI*10000));		
			push(10,battery*100);
	#endif
	/////////	
     sendDataToScope();
	

///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////	

////////////////////////////////////////////////////////
///////////////////////////////////////////////////


/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//	push(0,(s16)(gest_6050.pitch*100));  
//  push(1,(s16)(gest_6050.roll*100));
//  push(2,(s16)(gest_6050.yaw*10));
//  
//  push(3,gest_6050.pitch_gyro);
//  
//  push(4,Gyro_err_memory.pitch[0]);
//  push(5,Acc_err_memory.pitch[0]);                         
//  
//  push(6,Argue_Gyro_ring.pitch.Sum_out);
//  push(7,Argue_ACC_ring.pitch.Sum_out);
//  push(8,Acc_err_memory.pitch[0]-Acc_err_memory.pitch[1]);
//  
//  push(9,Acc_err_memory.pitch[0]-Acc_err_memory.pitch[2]);     
//  
//  push(10,Acc_err_memory.pitch[0]-Acc_err_memory.pitch[3]);

//  push(0,st_control_out.Upit);  
//  push(1,st_control_out.Uroll);
//  push(2,st_control_out.Uyaw);
//  
//  push(3,st_control_out.Ulift);
//  
//  push(4,Argue_ACC_ring.roll.Sum_out);
//  push(5,st_motor.motor_2);                         
//  
//  push(6,st_motor.motor_3);
//  push(7,st_motor.motor_4);
//  push(8,control_data_struct.roll);
//  
//  push(9,control_data_struct.lift);     
//  
//  push(10,battery*100);

//	push(0,sonic_time[0]);  
//  push(1,heigh_mm);
//  push(2,(s16)speed_h);
//  push(3,speed_h_diff);
//  push(4,(s16)height_out_I);
//  push(5,height_out_P);                         
//  push(6,height_out_D);
//  push(7,height_out);
//  push(8,speed_h_diff);
//  
//  push(9,control_data_struct.lift);     
//  
//  push(10,battery*100);	
	

  
//	push(0,400);  
//  push(1,heigh_mm);
//  push(2,(s16)speed_h);
//  push(3,height_out_diff);
//  push(4,height_out_P);
//  push(5,(s16)height_out_I);	
//  push(6,height_out_D);
//  push(7,err_mem[0]-err_mem[3]);
//  push(8,height_out_P + height_out_diff);
//  
//  push(9,control_data_struct.lift);     
//  
//  push(10,battery*100);
//	

  
}









