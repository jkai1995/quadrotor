#include "control_data.h"

MOTOR_LOCK motor_lock = LOCKED;
extern u16 out_control;
Control_Input_TypeDef control_data_struct = {0,0,0,0,0};
ST_MOTOR st_motor = {0,0,0,0}; //四个螺旋桨输入

ST_Control_Output_TypeDef st_control_out; //控制量
ST_Balance_Argue_TypeDef Argue_Gyro_ring;  //角速度环参数
ST_Balance_Argue_TypeDef Argue_ACC_ring;   //角度环参数
ST_DoubleClosedLoopPID_TypeDef st_HeighControler;///高度控制

u16 Lift_FeedForward; ////油门前馈

Err_Memory_TypeDef Gyro_err_memory;        //角速度err记录
Err_Memory_TypeDef Acc_err_memory;        //角度err记录
//根据控制量解算电机输入

#define ISXMODE 1
#define ISCROSSMODE 0

#if ISCROSSMODE
void motor_solve (void)
{
	///十字结果
//	st_control_out.Upit = st_motor.motor_3 - st_motor.motor_1;    
//	st_control_out.Uroll = st_motor.motor_4 - st_motor.motor_2;
//	st_control_out.Uyaw = st_motor.motor_1 + st_motor.motor_3 - st_motor.motor_2 - st_motor.motor_4;
//	st_control_out.Ulift = st_motor.motor_1 + st_motor.motor_2 + st_motor.motor_3 + st_motor.motor_4;
//  根据此关系  可由左侧四个控制量解算四个电机的输出	  如下
	st_control_out.Uyaw = -st_control_out.Uyaw;
	if(motor_lock == UN_LOCKED&&out_control < 100) ///解锁电机后才能控制输出  遥控器信号没有超时才能输出
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
	///X字结果
//	st_control_out.Upit = m3 + m4 - m2 - m1;    
//	st_control_out.Uroll = m1 + m4 -m2 -m3;
//	st_control_out.Uyaw = m1 +m3 - m2 - m4;
//	st_control_out.Ulift = m1 + m2 + m3 + m4;
//  根据此关系  可由左侧四个控制量解算四个电机的输出	  如下
	
	s16 pit = st_control_out.Upit;
	s16 roll = st_control_out.Uroll;
	s16 yaw = -st_control_out.Uyaw;
	s16 lift = st_control_out.Ulift;
	//lift = (lift > 300)?(300):(lift);
	s16 m1 = 0,m2 = 0,m3 = 0,m4 = 0;
	
	if(motor_lock == UN_LOCKED&&out_control < 100) ///解锁电机后才能控制输出  遥控器信号没有超时才能输出
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


//向缓冲区装载需要发送的数据
void lode_send_data (u8 buf[])
{
	
	buf[1]=control_data_struct.cmd;		//命令
	
	buf[2]=control_data_struct.lift>>8;		//油门
	buf[3]=control_data_struct.lift;		//油门
	
	buf[4]=control_data_struct.pitch;	//俯仰
	buf[5]=control_data_struct.roll;		//滚动
	buf[6]=control_data_struct.yaw;		//偏航
	
	buf[0]=7;						//一共要发送5个字节，buf[0]必须是6！！！！！！

}

//读取已接收的数据
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



////飞机平衡控制

///#define CAMERA_CONTROL  ///摄像头开关



////////////////遥控器比率计算
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
	
//////////////////////四个周期一次 角度环/////////////////////////////////
	CameraPercent();
	
			if(control_data_struct.lift>471)
		{
			control_data_struct.lift = 471;
		}
	
	if(control_tick == 3)  
	{
		Acc_Ring_pitch (10*control_data_struct.pitch);
		
	//	#ifdef CAMERA_CONTROL
		//	Acc_Ring_roll ((long)0.9*st_camera_data.roll*camerapercent/1000 + (long)10*control_data_struct.roll*(1000 - camerapercent)/1000);/////摄像头控 制
		//#else
			Acc_Ring_roll (10*control_data_struct.roll);////遥控器控制
		//#endif
		Acc_Ring_yaw(50*control_data_struct.yaw);
		

		
		heigh_control(control_data_struct.lift,get_sonic_heightmm(),0);
		//st_control_out.Ulift = 4*control_data_struct.lift;
	}
/////////////////////////////////////////////////////////////////////////////

	
//////////////////////每个周期一次角速度环/////////////////////////////////////////
#if 1
	Gyro_Ring_pitch(Argue_ACC_ring.pitch.Sum_out);
	Gyro_Ring_roll(Argue_ACC_ring.roll.Sum_out);
	#ifdef CAMERA_CONTROL
		Gyro_Ring_yaw((long)2.3*st_camera_data.yaw*camerapercent/1000);////摄像头控制
	#else
	Argue_ACC_ring.yaw.Sum_out = 5*control_data_struct.yaw;
		Gyro_Ring_yaw(Argue_ACC_ring.yaw.Sum_out);// 遥控器控制
	#endif
	
#endif
	///////////////////////////////////////////////////////////////////////
	
	
	control_tick++;
	if(control_tick >= 4 )
		control_tick = 0;
}
/*************************/

////////////                     //////**********************/////              /////////////////
////////////                    //////*    姿态平衡 控制    */////               ////////////////////
//////////////                   ////**********************//////               ////////////////////
#if 1

//////////////////**********************//////////////////////
//////////////////*    内环   角速度    */////////////////////////
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
	
	//////////////////////比例项
	Argue_Gyro_ring.pitch.KP = 0.5;//0.5
	Argue_Gyro_ring.pitch.P_out =  err * Argue_Gyro_ring.pitch.KP;
	
	///////////////////微分项
	Argue_Gyro_ring.pitch.KD = 0;
	Argue_Gyro_ring.pitch.D_out = diff * Argue_Gyro_ring.pitch.KD;
	///////////////////积分项
	Argue_Gyro_ring.pitch.KI = 0.001;
	Argue_Gyro_ring.pitch.I_out += Argue_Gyro_ring.pitch.KI * Gyro_err_memory.pitch[0];
	
	LIMIT(Argue_Gyro_ring.pitch.I_out,60,-60);
	
	///总输出
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
	
	//////////////////////比例项
	Argue_Gyro_ring.roll.KP = 0.5;//0.5
	Argue_Gyro_ring.roll.P_out =  err * Argue_Gyro_ring.roll.KP;
	
	///////////////////微分项
	Argue_Gyro_ring.roll.KD = 0;
	Argue_Gyro_ring.roll.D_out = diff * Argue_Gyro_ring.roll.KD;
		///////////////////积分项
	Argue_Gyro_ring.roll.KI = 0.001;
	Argue_Gyro_ring.roll.I_out += Argue_Gyro_ring.roll.KI * Gyro_err_memory.roll[0];
	
	LIMIT(Argue_Gyro_ring.roll.I_out,60,-60);
	
	///总输出
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
	
	//////////////////////比例项
	Argue_Gyro_ring.yaw.KP = 0.8;
	Argue_Gyro_ring.yaw.P_out =  err * Argue_Gyro_ring.yaw.KP;
	
	///////////////////积分项
	if(abs(Gyro_err_memory.yaw[0])>200)//////////积分分离
		Argue_Gyro_ring.yaw.KI = 0;
	else if(abs(Gyro_err_memory.yaw[0])>100)//////////积分分离
		Argue_Gyro_ring.yaw.KI = 0.006;
	else
		Argue_Gyro_ring.yaw.KI = 0.01;
	
	Argue_Gyro_ring.yaw.KI = 0.01;
	
	Argue_Gyro_ring.yaw.I_out +=Gyro_err_memory.yaw[0] * Argue_Gyro_ring.yaw.KI;
	
	///////////////////////限幅
	(Argue_Gyro_ring.yaw.I_out>150)?(Argue_Gyro_ring.yaw.I_out = 150):(Argue_Gyro_ring.yaw.I_out);
	(Argue_Gyro_ring.yaw.I_out<-150)?(Argue_Gyro_ring.yaw.I_out = -150):(Argue_Gyro_ring.yaw.I_out);
	
	
	///总输出
	Argue_Gyro_ring.yaw.Sum_out = Argue_Gyro_ring.yaw.P_out + (s16)Argue_Gyro_ring.yaw.I_out;
	
	
	
	
	
	st_control_out.Uyaw = 0;//Argue_Gyro_ring.yaw.Sum_out;
	
}
////////////


#endif  /////////内环  角速度环
///////////////内环  角速度环

/********************/




//////////////////**********************//////////////////////
//////////////////*    外环   角度     */////////////////////////
//////////////////**********************//////////////////////////
#if 1

/*********************/
/*     外环  pitch   */
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
	
	
	//////////////////////////比例控制
	Argue_ACC_ring.pitch.KP = 0.5;
	Argue_ACC_ring.pitch.P_out = err * Argue_ACC_ring.pitch.KP;
	
	
	///总输出
	Argue_ACC_ring.pitch.Sum_out = Argue_ACC_ring.pitch.P_out;
}

//////////角度环  ///   滚动平衡
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
	//////////////////////////比例控制
	Argue_ACC_ring.roll.KP = 0.5; //1  内环0.8 临界稳定
	Argue_ACC_ring.roll.P_out = err * Argue_ACC_ring.roll.KP;
	diff = diff;
	
	///总输出
	Argue_ACC_ring.roll.Sum_out = Argue_ACC_ring.roll.P_out;
}

//////////角度环  ///   航向
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
	//////////////////////////比例控制
	Argue_ACC_ring.yaw.KP = 0.5; //1  内环0.8 临界稳定
	Argue_ACC_ring.yaw.P_out = err * Argue_ACC_ring.yaw.KP;
	diff = diff;
	
	///总输出
	Argue_ACC_ring.yaw.Sum_out = Argue_ACC_ring.yaw.P_out;
}

#endif /////////外环 角度环
/////////////外环 角度环

#endif //////姿态平衡控制
////////////////姿态平衡控制

///////////////////////            /******************************/        ////////////////////////
///////////////////////            /*         高度控制           */        ////////////////////////
///////////////////////            /*****************************/        ////////////////////////
#if 1

///////////////////////高度控制  外环//////////
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
	PID.P_out = PID.KP * PID.err[0];///////比例
	
	PID.KD = 0.08;
	PID.D_out = PID.KD * PID.diff;//////////微分
	
	sum = PID.P_out + PID.D_out;
	
	LIMIT(sum,60,-60);
	return sum;
}
/////////

///////////////////////高度控制  内环  速度环//////////
s16 Heigh_Control_InnerRing (ST_PID_Controler_TypeDef &PID,u16 goal,u16 speed)
{
	s16 sum;
	PID.err[0] = goal - speed;
	
	PID.KP = 0.6;
	PID.P_out = PID.KP * PID.err[0]; ////////比例
	LIMIT(PID.KP,200,-200);
	/////////////////////////////积分分离
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
		
	PID.I_out += PID.KI * PID.err[0]; /////////积分
		
		
	LIMIT(PID.I_out,100,0);
		
	sum = PID.P_out + (s16)PID.I_out;

		return sum;
}
//////////////


void heigh_control (u16 goal,u16 heigh,s16 speed)
{
	s16 sum;
	if(goal<115)  //////////油门前馈
	{
		Lift_FeedForward = 4*goal;
	}
	else
	{
		Lift_FeedForward = 460;
	}
	
	if(goal>470)   ///////////////油门输入限幅
		goal = 470;
	
	
	st_HeighControler.OuterRing.Sum_out = Heigh_Control_OuterRing(st_HeighControler.OuterRing,goal,heigh); ////////高度控制外环
	st_HeighControler.InnerRing.Sum_out = Heigh_Control_InnerRing(st_HeighControler.InnerRing,st_HeighControler.OuterRing.Sum_out,speed);////////高度控制内环 
	
	sum = Lift_FeedForward + st_HeighControler.InnerRing.Sum_out;
	
	if(sum < 0)
		sum = 0;
	st_control_out.Ulift = 4 * (sum);
}

#endif
///////////高度控制

/********************************/
