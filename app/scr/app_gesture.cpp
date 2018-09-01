#include "app_gesture.h"
#include "includes.h"
#include "os_app_hooks.h"

OS_TCB GESTURE_TaskTCB;  //������ƿ�
CPU_STK GESTURE_TASK_STK[GESTURE_STK_SIZE];  //�����ջ	
GYRO_ZERO_TypeDef gesture_zero;


OS_ERR gesture_task_create (void)
{
		OS_ERR err;
		OSTaskCreate((OS_TCB 	* )&GESTURE_TaskTCB,		
				 (CPU_CHAR	* )"Task3 gesture", 		
                 (OS_TASK_PTR )gesture_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )GESTURE_TASK_PRIO,     
                 (CPU_STK   * )&GESTURE_TASK_STK[0],	
                 (CPU_STK_SIZE)GESTURE_STK_SIZE/10,	
                 (CPU_STK_SIZE)GESTURE_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
									
		return err;
}
u8 mpu_err;
s16 gyro_roll_m[4];
s16 gyro_pit_m[4];
s16 gyro_yaw_m[4];
void gesture_task (void *p_arg)
{
	OS_ERR err;
	MOTOR_init();//�����ʼ��
	OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_PERIODIC,&err);
	CPU_SR_ALLOC();
	MPU_IIC_Init();//��ʼ��IIC����
	MPU_Init();  //��ʼ��6050
	mpu_dmp_init(); //��ʼ��dmp
  uart2_init(115200); ///��ʼ��uart
	OS_CRITICAL_ENTER();//�����ٽ���
	Gyro_zero_get();
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	set_buzz_mod(BUZZ_TURN_ON);
	
	
	OSTaskSemPost(&SONIC_TaskTCB,///��������෢���ź�����ʾ��ʼ����
											OS_OPT_POST_NO_SCHED,//�������������
											&err);
	

	while(1)
	{	
		
		
		//TIM3_time_start();//////////////ʱ�������ʼ	

		//INTX_DISABLE();//�ر������ж� ��ֹ�������
		mpu_err = mpu_dmp_get_data(&(gest_6050.pitch),&(gest_6050.roll),&(gest_6050.yaw));  ///��ʱ1.29ms
		gest_6050.yaw = -gest_6050.yaw;
		//run_time_us.time0 = TIM3_time_over();////////////ʱ���������
		MPU_Get_Gyroscope(&(gest_6050.roll_gyro),&(gest_6050.pitch_gyro),&(gest_6050.yaw_gyro));  ///��ʱ0.27ms
		//run_time_us.time1 = TIM3_time_over();////////////ʱ���������
		MPU_Get_Accelerometer(&(gest_6050.roll_acc),&(gest_6050.pitch_acc),&(gest_6050.yaw_acc));////��ʱ0.27ms
		//run_time_us.time2 = TIM3_time_over();////////////ʱ���������
		gest_6050.yaw_gyro = -gest_6050.yaw_gyro;

    //INTX_ENABLE(); ////���������жϻָ��������

		gest_6050.roll_gyro = gest_6050.roll_gyro - gesture_zero.roll_zero;
		gest_6050.pitch_gyro = gest_6050.pitch_gyro - gesture_zero.pit_zero;
		gest_6050.yaw_gyro = gest_6050.yaw_gyro - gesture_zero.yaw_zero;
		
		AngleTransforming((float)gest_6050.pitch_acc,(float)-gest_6050.roll_acc,(float)-gest_6050.yaw_acc,gest_6050.pitch,gest_6050.yaw,gest_6050.roll);
		if(abs(gest_6050.pitch)>30 ||abs(gest_6050.roll)>30 || (abs(gest_6050.pitch)+abs(gest_6050.roll))>50)
		{
			motor_lock = LOCKED; //ң�س�ʱ�������ɻ�
			set_buzz_mod(BUZZ_OUT_CONTR);//ң�س�ʱ��ʾ
		}		
	//	gest_6050.pitch_gyro = (long)(gyro_pit_m[0] + gyro_pit_m[1] + gyro_pit_m[2] + gyro_pit_m[3])/4;
	//	gest_6050.roll_gyro = (long)(gyro_roll_m[0] + gyro_roll_m[1] + gyro_roll_m[2] + gyro_roll_m[3])/4;
	//	gest_6050.yaw_gyro = (long)(gyro_yaw_m[0] + gyro_yaw_m[1] + gyro_yaw_m[2] + gyro_yaw_m[3])/4;
		if(motor_lock == LOCKED)
		{
			gesture_zero.yaw_acc_zero = gest_6050.yaw; //���º����0λ�����������������ڸ��£�0λ�̶�
		}////
		else/// (motor_lock == UN_LOCKED)
		{
			balance_control();/////ƽ�����
		}
		motor_solve();  ////���������
		motor_input(st_motor.motor_1,st_motor.motor_2,st_motor.motor_3,st_motor.motor_4);
//run_time_us.time3 = TIM3_time_over();////////////ʱ���������
				WirelessDateSend(); ////��ʱ2.48ms
		//run_time_us.time4 = TIM3_time_over();////////////ʱ���������
	
		OSTimeDly(1,OS_OPT_TIME_PERIODIC,&err); //��ʱһ��ʱ�ӽ���
	}
}

///
///��ȡ�����Ǿ���
//
void Gyro_zero_get(void)
{
	//OS_ERR err;
	u8 i=0;
	s32 g_p = 0,g_r = 0,g_y = 0,ac_y = 0;
	s32 g_earth_z = 0;
	for(i = 0;i<50;i++)
	{
		mpu_dmp_get_data(&(gest_6050.pitch),&(gest_6050.roll),&(gest_6050.yaw));  ///��ʱ1.39ms
		gest_6050.yaw = -gest_6050.yaw;
		MPU_Get_Gyroscope(&(gest_6050.roll_gyro),&(gest_6050.pitch_gyro),&(gest_6050.yaw_gyro));
		gest_6050.yaw_gyro = -gest_6050.yaw_gyro;
		MPU_Get_Accelerometer(&(gest_6050.roll_acc),&(gest_6050.pitch_acc),&(gest_6050.yaw_acc));////��ʱ0.29ms
		AngleTransforming((float)gest_6050.pitch_acc,(float)-gest_6050.roll_acc,(float)-gest_6050.yaw_acc,gest_6050.pitch,gest_6050.yaw,gest_6050.roll);
		g_earth_z += (s32)st_coordinate_earth.Z;
		g_p += gest_6050.pitch_gyro;
		g_r += gest_6050.roll_gyro;
		g_y += gest_6050.yaw_gyro;
		ac_y += gest_6050.yaw_acc;
		delay_us(5*1000);
	}
	gesture_zero.pit_zero = g_p/50;
	gesture_zero.roll_zero = g_r/50;
	gesture_zero.yaw_zero = g_y/50;
	st_coordinate_earth_zero.Z = g_earth_z/50;
}

