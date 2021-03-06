/**
  ******************************************************************************
  * @file    FreeCars_uart.c
  * @author  FreeCars雄哥
  * @version NULL
  * @date    2014/11/11
  * @brief 	 FreeCars_uart C file.(For XS128)
  *   site:   Http://FreeCars.taobao.com
  *   QQ群：  384273254，149168724
  *   版权：  此代码为FreeCars上位机协议代码，允许任意使用，也允许用于商业用途，但请保留此段文字！
  *   tips:   强烈建议小伙伴们使用FreeCars出品的蓝牙模块套装，无线上位机从这里开始！
  ******************************************************************************
  */
#include "FreeCars_uart.h"
#include "heightSolution.h"
/************一下是根据FreeCars上位机协议修改的，无需改变**************/
#ifdef FREECAES_DMA_ENABLE
uint8 uSendBuf[UartDataNum*2+4]= {0};
#define FreeCarsDataNum (UartDataNum*2+4)
#define endIndex (FreeCarsDataNum - 1)
#else
uint8 uSendBuf[UartDataNum*2]= {0};
uint8 FreeCarsDataNum=UartDataNum*2;
#endif

SerialPortType SerialPortRx;
double UartData[9] = {0};

//uint8 FreeCarsCount=0;
/**************************/



#ifdef FREECAES_DMA_ENABLE
void init_FreeCares()
{
	uSendBuf[0] = 251;
	uSendBuf[1] = 109;
	uSendBuf[2] = 37;
	FREECAES_DMA_INIT(uSendBuf,FreeCarsDataNum);
}
void sendDataToScope(void)
{
	uint8 i,sum=141;//141 = (251+109+37)&0xff 
	for(i=3; i<endIndex; i++)
	{
		sum+=uSendBuf[i];         //全部数据加入校验
	}
	uSendBuf[endIndex] = sum;
	FREECAES_DMA_SEND(FreeCarsDataNum);
}
#else
/*
轮询法发送一帧数据
消耗时间与数据长度有关
不可以放在中断里面中期性调用
*/
void sendDataToScope(void)
{
	uint8 i,sum=0;

	FREECAES_SCI_SEND_CHANNEL(251);
	FREECAES_SCI_SEND_CHANNEL(109);
	FREECAES_SCI_SEND_CHANNEL(37);
	sum+=(251);      //全部数据加入校验
	sum+=(109);
	sum+=(37);
	for(i=0; i<FreeCarsDataNum; i++)
	{
		FREECAES_SCI_SEND_CHANNEL(uSendBuf[i]);
		sum+=uSendBuf[i];         //全部数据加入校验
	}
	FREECAES_SCI_SEND_CHANNEL(sum);
}
#endif
/*
向某个通道填充数据
adr：通道
date：数据-32768~32767
*/
void push(uint8 chanel,uint16 dat)
{
	chanel *= 2;
#ifdef FREECAES_DMA_ENABLE
	chanel += 3;
#endif
	uSendBuf[chanel]=(dat>>8);
	uSendBuf[chanel+1]=(dat&0xff);
}


#define CASE7
//extern u32 tim_count,heigh_mm;

extern u32 err_diff[4];
extern u32 distance_mem_real[5];
//extern LEVEL level;
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


#ifdef CASE1  ///////姿态 高度 电池观测
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
#ifdef CASE2  ///////方向命令 实际 观测

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
#ifdef CASE3  ///////调试 俯仰平衡

	push(0,10*control_data_struct.pitch);/////输入命令
	push(1,10*control_data_struct.roll);

	push(2,(s16)(gest_6050.pitch*100));   ////////实际相应
	push(3,(s16)(gest_6050.roll*100));

	push(4,Argue_ACC_ring.pitch.P_out); //外环P输出
	push(5,gest_6050.pitch_gyro);			  ////角速度
	push(6,Gyro_err_memory.pitch[0]);   //内环偏差
	push(7,Argue_Gyro_ring.pitch.P_out);   //内环比例输出
	push(8,0);

	push(9,0);
	push(10,battery*100);
#endif
	/////////
#ifdef CASE4  ///////调试 航向

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
#ifdef CASE5  ///////////////高度测量观测	 高度控制

	push(0,control_data_struct.lift); //转换成高度后的
	push(1,heigh_mm);  //极值滤波后的
	push(2,st_HeighControler.OuterRing.P_out);//外环比例
	push(3,st_HeighControler.OuterRing.diff);///微分
	push(4,st_HeighControler.OuterRing.Sum_out); //目标速度
	push(5,st_HeighControler.InnerRing.err[0]);//内环偏差
	push(6,st_HeighControler.InnerRing.P_out);//比例输出
	push(7,(s16)st_HeighControler.InnerRing.I_out);//积分输出
	push(8,Lift_FeedForward);   //前馈
	push(9,st_control_out.Ulift/4);//总输出
	push(10,battery*100);
#endif
/////////


#ifdef CASE6  ///////////////高度测量观测	

	push(0,0); //测距原始数据
	push(1,tim_count);  //极值滤波后的
	push(2,heigh_mm);//转换成高度后的
	push(3,((s16)PAin(1))*1000);//极值
	push(4,run_time_us.time4/10); //
	push(5,0);//
	push(6,((s16)level)*1000);//实际速度
	push(7,run_time_us.time0);//此时检测到上升沿
	push(8,run_time_us.time1);//下降沿
	push(9,run_time_us.time2/10);   //开始处理

	push(10,run_time_us.time3/10); ///总时间
	//push(10,sonic_lost_time*1000);
#endif
////////////
#ifdef CASE7  ///////////////垂直速度融合	

	static s16 h,s;
	s16 hh = get_sonic_heightmm();
	s = hh - h;
	h	= hh;
	push(0,h); //测距原始数据
	push(1,st_coordinate_earth.Z);  //极值滤波后的
	push(2,s);//转换成高度后的
	push(3,st_coordinate_earth.Y);//极值
	push(4,(s16)h_err); //
	push(5,(s16)solution_speed_mmPs);//
	push(6,(s16)solution_height_mm);//实际速度
	push(7,(s16)(gest_6050.pitch*100));//此时检测到上升沿
	push(8,(s16)(gest_6050.yaw*100));//下降沿
	push(9,(s16)(gest_6050.roll*100));   //开始处理
	push(10,0); ///总时间
#endif
////////////

#ifdef CASE8  ///////俯仰控制

	push(0,10*control_data_struct.pitch);///角度命令
	push(1,(s16)(gest_6050.pitch*100));////实际角度

	push(2,Argue_ACC_ring.pitch.Sum_out); //// 角速度命令
	push(3,(s16)(gest_6050.pitch_gyro)); //////实际角速度
	push(4,(s16)(Gyro_err_memory.pitch[0]));////角度都误差
	push(5,Argue_Gyro_ring.pitch.P_out);/////////角速度比例项

	push(6,(s16)Argue_Gyro_ring.pitch.I_out);////角速度积分项
	push(7,Argue_Gyro_ring.pitch.Sum_out);   ////角速度输出
	push(8,0);
	push(9,(s16)(Argue_Gyro_ring.pitch.KI*10000));
	push(10,battery*100);
#endif
	/////////
#ifdef CASE9  ///////俯仰 滚动积分项调试

	push(0,10*control_data_struct.pitch);///角度命令
	push(1,(s16)(gest_6050.pitch*100));////实际角度

	push(2,10*control_data_struct.roll); //// 角速度命令
	push(3,(s16)(gest_6050.roll*100)); //////实际角速度
	push(4,(s16)(Gyro_err_memory.pitch[0]));////角度都误差
	push(5,Argue_Gyro_ring.pitch.P_out);/////////角速度比例项

	push(6,(s16)Argue_Gyro_ring.pitch.I_out);////角速度积分项
	push(7,Argue_Gyro_ring.pitch.Sum_out);   ////角速度输出
	push(8,(s16)Argue_Gyro_ring.roll.I_out);
	push(9,(s16)(Argue_Gyro_ring.pitch.KI*10000));
	push(10,battery*100);
#endif
#ifdef CASE10  ///////俯仰 滚动积分项调试

	push(0,1);///角度命令
	push(1,2);////实际角度

	push(2,3); //// 角速度命令
	push(3,4); //////实际角速度
	push(4,5);////角度都误差
	push(5,6);/////////角速度比例项

	push(6,7);////角速度积分项
	push(7,8);   ////角速度输出
	push(8,9);
	push(9,10);
	push(10,11);
#endif
	/////////
	sendDataToScope();


///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////



}









