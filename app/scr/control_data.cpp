#include "control_data.h"

MOTOR_LOCK motor_lock = LOCKED;
extern u16 out_control;
Control_Input_TypeDef control_data_struct = {0,0,0,0,0};
ST_MOTOR st_motor = {0,0,0,0}; //�ĸ�����������

ST_Control_Output_TypeDef st_control_out; //������
ST_Balance_Argue_TypeDef Argue_Gyro_ring;  //���ٶȻ�����
ST_Balance_Argue_TypeDef Argue_ACC_ring;   //�ǶȻ�����
ST_DoubleClosedLoopPID_TypeDef st_HeighControler;///�߶ȿ���

u16 Lift_FeedForward; ////����ǰ��

Err_Memory_TypeDef Gyro_err_memory;        //���ٶ�err��¼
Err_Memory_TypeDef Acc_err_memory;        //�Ƕ�err��¼
//���ݿ���������������

#define ISXMODE 1
#define ISCROSSMODE 0

#if ISCROSSMODE
void motor_solve (void)
{
	///ʮ�ֽ��
//	st_control_out.Upit = st_motor.motor_3 - st_motor.motor_1;    
//	st_control_out.Uroll = st_motor.motor_4 - st_motor.motor_2;
//	st_control_out.Uyaw = st_motor.motor_1 + st_motor.motor_3 - st_motor.motor_2 - st_motor.motor_4;
//	st_control_out.Ulift = st_motor.motor_1 + st_motor.motor_2 + st_motor.motor_3 + st_motor.motor_4;
//  ���ݴ˹�ϵ  ��������ĸ������������ĸ���������	  ����
	st_control_out.Uyaw = -st_control_out.Uyaw;
	if(motor_lock == UN_LOCKED&&out_control < 100) ///�����������ܿ������  ң�����ź�û�г�ʱ�������
	{
		st_control_out.Uyaw = 0;
		//st_control_out.Upit = 0;
		st_control_out.Uroll = 0;
		st_motor.motor_1 = 0.25*st_control_out.Uyaw - 0.5*st_control_out.Upit + 0.25*st_control_out.Ulift;
		st_motor.motor_2 = 0.25*st_control_out.Ulift - 0.25*st_control_out.Uyaw - 0.5*st_control_out.Uroll;
		st_motor.motor_3 = 0.5*st_control_out.Upit + 0.25*st_control_out.Uyaw + 0.25*st_control_out.Ulift;
		st_motor.motor_4 = 0.5*st_control_out.Uroll - 0.25*st_control_out.Uyaw + 0.25*st_control_out.Ulift;
	}
	else
	{
		st_motor.motor_1 = 0;
		st_motor.motor_2 = 0;
		st_motor.motor_3 = 0;
		st_motor.motor_4 = 0;
		
	}
	
}
#endif
#if ISXMODE
void motor_solve (void)
{
	///X�ֽ��
//	st_control_out.Upit = m3 + m4 - m2 - m1;    
//	st_control_out.Uroll = m1 + m4 -m2 -m3;
//	st_control_out.Uyaw = m1 +m3 - m2 - m4;
//	st_control_out.Ulift = m1 + m2 + m3 + m4;
//  ���ݴ˹�ϵ  ��������ĸ������������ĸ���������	  ����
	
	s16 pit = st_control_out.Upit;
	s16 roll = st_control_out.Uroll;
	s16 yaw = -st_control_out.Uyaw;
	s16 lift = st_control_out.Ulift;
	//lift = (lift > 300)?(300):(lift);
	s16 m1 = 0,m2 = 0,m3 = 0,m4 = 0;
	
	if(motor_lock == UN_LOCKED&&out_control < 100) ///�����������ܿ������  ң�����ź�û�г�ʱ�������
	{
		m1 = 0.25*(lift - pit + roll + yaw);
		m2 = 0.25*(lift - pit - roll - yaw);
		m3 = 0.25*(lift + pit - roll + yaw);
		m4 = 0.25*(lift + pit + roll - yaw);
	}

	st_motor.motor_1 = m1;
	st_motor.motor_2 = m2;
	st_motor.motor_3 = m3;
	st_motor.motor_4 = m4;

	
}
#endif

/////////


//�򻺳���װ����Ҫ���͵�����
void lode_send_data (u8 buf[])
{
	
	buf[1]=control_data_struct.cmd;		//����
	
	buf[2]=control_data_struct.lift>>8;		//����
	buf[3]=control_data_struct.lift;		//����
	
	buf[4]=control_data_struct.pitch;	//����
	buf[5]=control_data_struct.roll;		//����
	buf[6]=control_data_struct.yaw;		//ƫ��
	
	buf[0]=7;						//һ��Ҫ����5���ֽڣ�buf[0]������6������������

}

//��ȡ�ѽ��յ�����
void lode_rece_data (u8 buf[])
{
Buzz* buzz = Buzz::getInstance();
	control_data_struct.cmd = buf[1];
	control_data_struct.lift = ((u16)buf[2]<<8) + buf[3];
	control_data_struct.pitch = (s8)buf[4];
	control_data_struct.roll = (s8)buf[5];
	control_data_struct.yaw = (s8)buf[6];
	
	if(control_data_struct.cmd == 1)
	{
		motor_lock = UN_LOCKED;
		buzz->set_buzz_mod(BUZZ_UNLOCK);
	}
	else if(control_data_struct.cmd == 2)
	{
		motor_lock = LOCKED;
		buzz->set_buzz_mod(BUZZ_LOCK);
	}
	

	
}



////�ɻ�ƽ�����

///#define CAMERA_CONTROL  ///����ͷ����



////////////////ң�������ʼ���
	static u16 camerapercent = 0;

void CameraPercent (void)
{
	u32 heigh = get_sonic_heightmm();
	if((control_data_struct.lift>=430) && heigh > 350)
	{
		camerapercent = camerapercent + 5;
	}
	
	if(camerapercent> 1000)
	{
		camerapercent = 1000;
	}

}



void balance_control (void)
{
	static u8 control_tick=0;
	
//////////////////////�ĸ�����һ�� �ǶȻ�/////////////////////////////////
	CameraPercent();
	
			if(control_data_struct.lift>471)
		{
			control_data_struct.lift = 471;
		}
	
	if(control_tick == 3)  
	{
		Acc_Ring_pitch (10*control_data_struct.pitch);
		
	//	#ifdef CAMERA_CONTROL
		//	Acc_Ring_roll ((long)0.9*st_camera_data.roll*camerapercent/1000 + (long)10*control_data_struct.roll*(1000 - camerapercent)/1000);/////����ͷ�� ��
		//#else
			Acc_Ring_roll (10*control_data_struct.roll);////ң��������
		//#endif
		Acc_Ring_yaw(50*control_data_struct.yaw);
		

		
		heigh_control(control_data_struct.lift,get_sonic_heightmm(),0);
		//st_control_out.Ulift = 4*control_data_struct.lift;
	}
/////////////////////////////////////////////////////////////////////////////

	
//////////////////////ÿ������һ�ν��ٶȻ�/////////////////////////////////////////
#if 1
	Gyro_Ring_pitch(Argue_ACC_ring.pitch.Sum_out);
	Gyro_Ring_roll(Argue_ACC_ring.roll.Sum_out);
	#ifdef CAMERA_CONTROL
		Gyro_Ring_yaw((long)2.3*st_camera_data.yaw*camerapercent/1000);////����ͷ����
	#else
	Argue_ACC_ring.yaw.Sum_out = 5*control_data_struct.yaw;
		Gyro_Ring_yaw(Argue_ACC_ring.yaw.Sum_out);// ң��������
	#endif
	
#endif
	///////////////////////////////////////////////////////////////////////
	
	
	control_tick++;
	if(control_tick >= 4 )
		control_tick = 0;
}
/*************************/

////////////                     //////**********************/////              /////////////////
////////////                    //////*    ��̬ƽ�� ����    */////               ////////////////////
//////////////                   ////**********************//////               ////////////////////
#if 1

//////////////////**********************//////////////////////
//////////////////*    �ڻ�   ���ٶ�    */////////////////////////
//////////////////**********************//////////////////////////
#if 1 

/**********************/
/*        pitch       */
/**********************/
//////////////
void Gyro_Ring_pitch (s16 given_pitch)
{
	s16 err,diff;
	
	err = given_pitch - gest_6050.pitch_gyro;
	Gyro_err_memory.pitch[4] = Gyro_err_memory.pitch[3];
	Gyro_err_memory.pitch[3] = Gyro_err_memory.pitch[2];
	Gyro_err_memory.pitch[2] = Gyro_err_memory.pitch[1];
	Gyro_err_memory.pitch[1] = Gyro_err_memory.pitch[0];
	Gyro_err_memory.pitch[0] = err;
	
  diff = err - Gyro_err_memory.pitch[1];
	
	//////////////////////������
	Argue_Gyro_ring.pitch.KP = 0.5;//0.5
	Argue_Gyro_ring.pitch.P_out =  err * Argue_Gyro_ring.pitch.KP;
	
	///////////////////΢����
	Argue_Gyro_ring.pitch.KD = 0;
	Argue_Gyro_ring.pitch.D_out = diff * Argue_Gyro_ring.pitch.KD;
	///////////////////������
	Argue_Gyro_ring.pitch.KI = 0.001;
	Argue_Gyro_ring.pitch.I_out += Argue_Gyro_ring.pitch.KI * Gyro_err_memory.pitch[0];
	
	LIMIT(Argue_Gyro_ring.pitch.I_out,60,-60);
	
	///�����
	Argue_Gyro_ring.pitch.Sum_out = Argue_Gyro_ring.pitch.P_out + Argue_Gyro_ring.pitch.D_out+(s16)Argue_Gyro_ring.pitch.I_out;
	
	
	st_control_out.Upit = Argue_Gyro_ring.pitch.Sum_out;

}
/////////////

/**********************/
/*        Roll       */
/**********************/
void Gyro_Ring_roll (s16 given_roll)
{
	s16 err,diff;
	
	err = given_roll - gest_6050.roll_gyro;
	Gyro_err_memory.roll[4] = Gyro_err_memory.roll[3];
	Gyro_err_memory.roll[3] = Gyro_err_memory.roll[2];
	Gyro_err_memory.roll[2] = Gyro_err_memory.roll[1];
	Gyro_err_memory.roll[1] = Gyro_err_memory.roll[0];
	Gyro_err_memory.roll[0] = err;
	
  diff = err - Gyro_err_memory.roll[1];
	
	//////////////////////������
	Argue_Gyro_ring.roll.KP = 0.5;//0.5
	Argue_Gyro_ring.roll.P_out =  err * Argue_Gyro_ring.roll.KP;
	
	///////////////////΢����
	Argue_Gyro_ring.roll.KD = 0;
	Argue_Gyro_ring.roll.D_out = diff * Argue_Gyro_ring.roll.KD;
		///////////////////������
	Argue_Gyro_ring.roll.KI = 0.001;
	Argue_Gyro_ring.roll.I_out += Argue_Gyro_ring.roll.KI * Gyro_err_memory.roll[0];
	
	LIMIT(Argue_Gyro_ring.roll.I_out,60,-60);
	
	///�����
	Argue_Gyro_ring.roll.Sum_out = Argue_Gyro_ring.roll.P_out + Argue_Gyro_ring.roll.D_out + (s16)Argue_Gyro_ring.roll.KI;
	
	
	
	
	
	st_control_out.Uroll = Argue_Gyro_ring.roll.Sum_out;
}
////////////

/**********************/
/*         Yaw        */
/**********************/
void Gyro_Ring_yaw (s16 given_yaw)
{
	
		s16 err;
	
	err = given_yaw - gest_6050.yaw_gyro;
	Gyro_err_memory.yaw[4] = Gyro_err_memory.yaw[3];
	Gyro_err_memory.yaw[3] = Gyro_err_memory.yaw[2];
	Gyro_err_memory.yaw[2] = Gyro_err_memory.yaw[1];
	Gyro_err_memory.yaw[1] = Gyro_err_memory.yaw[0];
	Gyro_err_memory.yaw[0] = err;
	
//  diff = err - Gyro_err_memory.yaw[1];
	
	//////////////////////������
	Argue_Gyro_ring.yaw.KP = 0.8;
	Argue_Gyro_ring.yaw.P_out =  err * Argue_Gyro_ring.yaw.KP;
	
	///////////////////������
	if(abs(Gyro_err_memory.yaw[0])>200)//////////���ַ���
		Argue_Gyro_ring.yaw.KI = 0;
	else if(abs(Gyro_err_memory.yaw[0])>100)//////////���ַ���
		Argue_Gyro_ring.yaw.KI = 0.006;
	else
		Argue_Gyro_ring.yaw.KI = 0.01;
	
	Argue_Gyro_ring.yaw.KI = 0.01;
	
	Argue_Gyro_ring.yaw.I_out +=Gyro_err_memory.yaw[0] * Argue_Gyro_ring.yaw.KI;
	
	///////////////////////�޷�
	(Argue_Gyro_ring.yaw.I_out>150)?(Argue_Gyro_ring.yaw.I_out = 150):(Argue_Gyro_ring.yaw.I_out);
	(Argue_Gyro_ring.yaw.I_out<-150)?(Argue_Gyro_ring.yaw.I_out = -150):(Argue_Gyro_ring.yaw.I_out);
	
	
	///�����
	Argue_Gyro_ring.yaw.Sum_out = Argue_Gyro_ring.yaw.P_out + (s16)Argue_Gyro_ring.yaw.I_out;
	
	
	
	
	
	st_control_out.Uyaw = 0;//Argue_Gyro_ring.yaw.Sum_out;
	
}
////////////


#endif  /////////�ڻ�  ���ٶȻ�
///////////////�ڻ�  ���ٶȻ�

/********************/




//////////////////**********************//////////////////////
//////////////////*    �⻷   �Ƕ�     */////////////////////////
//////////////////**********************//////////////////////////
#if 1

/*********************/
/*     �⻷  pitch   */
/*********************/
void Acc_Ring_pitch (s16 given_pitch)
{
	s16 err;
	
	err = given_pitch - (s16)(gest_6050.pitch*100);
	Acc_err_memory.pitch[4] = Acc_err_memory.pitch[3];
	Acc_err_memory.pitch[3] = Acc_err_memory.pitch[2];
	Acc_err_memory.pitch[2] = Acc_err_memory.pitch[1];
	Acc_err_memory.pitch[1] = Acc_err_memory.pitch[0];
	Acc_err_memory.pitch[0] = err;
	
	
	//////////////////////////��������
	Argue_ACC_ring.pitch.KP = 0.5;
	Argue_ACC_ring.pitch.P_out = err * Argue_ACC_ring.pitch.KP;
	
	
	///�����
	Argue_ACC_ring.pitch.Sum_out = Argue_ACC_ring.pitch.P_out;
}

//////////�ǶȻ�  ///   ����ƽ��
void Acc_Ring_roll (s16 given_roll)
{
	s16 err,diff;
	
	err = given_roll - (s16)(gest_6050.roll*100);
	Acc_err_memory.roll[4] = Acc_err_memory.roll[3];
	Acc_err_memory.roll[3] = Acc_err_memory.roll[2];
	Acc_err_memory.roll[2] = Acc_err_memory.roll[1];
	Acc_err_memory.roll[1] = Acc_err_memory.roll[0];
	Acc_err_memory.roll[0] = err;
	
	diff = Acc_err_memory.roll[0] - Acc_err_memory.roll[1];
	//////////////////////////��������
	Argue_ACC_ring.roll.KP = 0.5; //1  �ڻ�0.8 �ٽ��ȶ�
	Argue_ACC_ring.roll.P_out = err * Argue_ACC_ring.roll.KP;
	diff = diff;
	
	///�����
	Argue_ACC_ring.roll.Sum_out = Argue_ACC_ring.roll.P_out;
}

//////////�ǶȻ�  ///   ����
void Acc_Ring_yaw (s16 given_yaw)
{
	s16 err,diff;
	
	err = given_yaw - (s16)((gest_6050.yaw-gesture_zero.yaw_acc_zero)*100);
	Acc_err_memory.yaw[4] = Acc_err_memory.yaw[3];
	Acc_err_memory.yaw[3] = Acc_err_memory.yaw[2];
	Acc_err_memory.yaw[2] = Acc_err_memory.yaw[1];
	Acc_err_memory.yaw[1] = Acc_err_memory.yaw[0];
	Acc_err_memory.yaw[0] = err;
	
	diff = Acc_err_memory.yaw[0] - Acc_err_memory.yaw[1];
	//////////////////////////��������
	Argue_ACC_ring.yaw.KP = 0.5; //1  �ڻ�0.8 �ٽ��ȶ�
	Argue_ACC_ring.yaw.P_out = err * Argue_ACC_ring.yaw.KP;
	diff = diff;
	
	///�����
	Argue_ACC_ring.yaw.Sum_out = Argue_ACC_ring.yaw.P_out;
}

#endif /////////�⻷ �ǶȻ�
/////////////�⻷ �ǶȻ�

#endif //////��̬ƽ�����
////////////////��̬ƽ�����

///////////////////////            /******************************/        ////////////////////////
///////////////////////            /*         �߶ȿ���           */        ////////////////////////
///////////////////////            /*****************************/        ////////////////////////
#if 1

///////////////////////�߶ȿ���  �⻷//////////
s16 Heigh_Control_OuterRing (ST_PID_Controler_TypeDef &PID,u16 goal,u16 heigh)
{
	u8 i;
	s16 sum;

		for(i = 9;i > 0;i--)
	{
		PID.err[i] = PID.err[i-1];
	}
	PID.err[0] = goal - heigh;
	PID.diff = PID.err[0] - PID.err[3];//////
	
	PID.KP = 0.15;
	PID.P_out = PID.KP * PID.err[0];///////����
	
	PID.KD = 0.08;
	PID.D_out = PID.KD * PID.diff;//////////΢��
	
	sum = PID.P_out + PID.D_out;
	
	LIMIT(sum,60,-60);
	return sum;
}
/////////

///////////////////////�߶ȿ���  �ڻ�  �ٶȻ�//////////
s16 Heigh_Control_InnerRing (ST_PID_Controler_TypeDef &PID,u16 goal,u16 speed)
{
	s16 sum;
	PID.err[0] = goal - speed;
	
	PID.KP = 0.6;
	PID.P_out = PID.KP * PID.err[0]; ////////����
	LIMIT(PID.KP,200,-200);
	/////////////////////////////���ַ���
		if(abs(PID.err[0]) < 10)
			 PID.KI = 0.05;
		else if(abs(PID.err[0]) < 20)
			 PID.KI = 0.04;
		else if(abs(PID.err[0]) < 30)
			PID.KI = 0.02;
		else if(abs(PID.err[0]) < 50)
			 PID.KI = 0.007;
		else 
			 PID.KI = 0.0001;
		
	PID.I_out += PID.KI * PID.err[0]; /////////����
		
		
	LIMIT(PID.I_out,100,0);
		
	sum = PID.P_out + (s16)PID.I_out;

		return sum;
}
//////////////


void heigh_control (u16 goal,u16 heigh,s16 speed)
{
	s16 sum;
	if(goal<115)  //////////����ǰ��
	{
		Lift_FeedForward = 4*goal;
	}
	else
	{
		Lift_FeedForward = 460;
	}
	
	if(goal>470)   ///////////////���������޷�
		goal = 470;
	
	
	st_HeighControler.OuterRing.Sum_out = Heigh_Control_OuterRing(st_HeighControler.OuterRing,goal,heigh); ////////�߶ȿ����⻷
	st_HeighControler.InnerRing.Sum_out = Heigh_Control_InnerRing(st_HeighControler.InnerRing,st_HeighControler.OuterRing.Sum_out,speed);////////�߶ȿ����ڻ� 
	
	sum = Lift_FeedForward + st_HeighControler.InnerRing.Sum_out;
	
	if(sum < 0)
		sum = 0;
	st_control_out.Ulift = 4 * (sum);
}

#endif
///////////�߶ȿ���

/********************************/
