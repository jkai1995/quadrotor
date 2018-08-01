#include "KalmanandYijielvbo.h"
#include "sys.h"

s16 ANGBLANY,ANGBLANX;
float K1 =0.05; 
float angle_dot; 	
float Q_angle=0.001;// 1y3¨¬??¨¦¨´¦Ì?D-¡¤?2?
float Q_gyro=0.003;//0.03 1y3¨¬??¨¦¨´¦Ì?D-¡¤?2? 1y3¨¬??¨¦¨´¦Ì?D-¡¤?2??a¨°???¨°?DD¨¢?¨¢D???¨®
float R_angle=0.5;// 2a¨¢???¨¦¨´¦Ì?D-¡¤?2? ?¨¨2a¨¢???2?
float dt=0.005;//                 
char  C_0 = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] ={0,0,0,0};
float PP[2][2] = { { 1, 0 },{ 0, 1 } };


extern volatile s16 MPU6050_ACCEL_XOUT_DATA;
extern volatile s16 MPU6050_ACCEL_YOUT_DATA;
extern volatile s16 MPU6050_ACCEL_ZOUT_DATA;
extern volatile s16 MPU6050_TEMP_OUT_DATA;
extern volatile s16 MPU6050_GYRO_XOUT_DATA;
extern volatile s16 MPU6050_GYRO_YOUT_DATA;
extern volatile s16 MPU6050_GYRO_ZOUT_DATA;

extern float Pitch1_1 ,Roll1_1 ,Yaw1_1  ,Temperture1; 
extern float Pitch1_2 ,Roll1_2 ,Yaw1_2 ; 
extern float Pitch2_1 ,Roll2_1 ,Yaw2_1 ,Temperture2; 
extern float Pitch1_2 ,Roll1_2 ,Yaw1_2 ; 
/**************************************************************************
o¡¥¨ºy1|?¨¹¡êo?¨°¨°¡Á?¡§???¨¹??2¡§
¨¨??¨²2?¨ºy¡êo?¨®?¨´?¨¨?¡é???¨´?¨¨
¡¤¦Ì??  ?¦Ì¡êo?T
**************************************************************************/
void Kalman_FilterY(float Accel,float Gyro)		
{
	static float kalangY;
	kalangY+=(Gyro - Q_bias) * dt; //?¨¨?¨¦1¨¤??
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-?¨¨?¨¦1¨¤???¨®2?D-¡¤?2?¦Ì??¡é¡¤?

	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-?¨¨?¨¦1¨¤???¨®2?D-¡¤?2??¡é¡¤?¦Ì??y¡¤?
	PP[0][1] += Pdot[1] * dt;   // =?¨¨?¨¦1¨¤???¨®2?D-¡¤?2?
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - kalangY;	//zk-?¨¨?¨¦1¨¤??
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //o¨®?¨¦1¨¤???¨®2?D-¡¤?2?
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	kalangY	+= K_0 * Angle_err;	 //o¨®?¨¦1¨¤??
	Q_bias	+= K_1 * Angle_err;	 //o¨®?¨¦1¨¤??
	angle_dot   = Gyro - Q_bias;	 //¨º?3??¦Ì(o¨®?¨¦1¨¤??)¦Ì??¡é¡¤?=???¨´?¨¨
}

/**************************************************************************

**************************************************************************/
void YijielvboY(float *pit, float angle_m, float gyro_m)
{
   //angle = K1 * angle_m+ (1-K1) * (angle + gyro_m * 0.005);
   *pit = K1 * angle_m+ (1-K1) * (*pit + gyro_m * 0.020);
}       //0.02
/**************************************************************************

**************************************************************************/
void YijielvboX(float *rol, float angle_m, float gyro_m)
{
   //angle = K1 * angle_m+ (1-K1) * (angle + gyro_m * 0.005);
   *rol = K1 * angle_m+ (1-K1) * (*rol + gyro_m * 0.020);
}       //0.02
/**************************************************************************

**************************************************************************/
void Get_AnglePitch(uint8 cha)
{ 
	static float angleY[4];
  float Angle_Balance; 
  float AngAccY,Accel_X,Accel_Z,Gyro_Y;
  Accel_X=MPU6050_ACCEL_XOUT_DATA;
  Accel_Z=MPU6050_ACCEL_ZOUT_DATA;
  Gyro_Y=MPU6050_GYRO_YOUT_DATA;

	
    //Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //?¨¢¨¨?Y?¨¢¨ª¨®?Y¨°?
    //Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L);    //?¨¢¨¨?Z?¨¢¨ª¨®?Y¨°?
    //Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //?¨¢¨¨?X?¨¢?¨®?¨´?¨¨??
    if(Gyro_Y>32768)  Gyro_Y-=65536;     //¨ºy?Y¨¤¨¤D¨ª¡Áa??  ¨°2?¨¦¨ª¡§1yshort????¨¤¨¤D¨ª¡Áa??
    if(Accel_Z>32768)  Accel_Z-=65536;     //¨ºy?Y¨¤¨¤D¨ª¡Áa??
    if(Accel_X>32768) Accel_X-=65536;    //¨ºy?Y¨¤¨¤D¨ª¡Áa??
    AngAccY=atan2(Accel_X,Accel_Z)*180/PI;                 //????¨®?¦Ì???¦Ì??D??	
    Gyro_Y=Gyro_Y/16.4;                                    //¨ª¨®?Y¨°?¨¢?3¨¬¡Áa??	

    YijielvboY(&angleY[cha],AngAccY,-Gyro_Y);    //?£¤21??2¡§
    
    Angle_Balance=angleY[cha];                                   //?¨¹D???oa????
    Pitch1_2=Angle_Balance*100;
}

void Get_Pitch(struct  mpu_e * mpux)
{
	float AngAccY,Accel_X,Accel_Z,Gyro_Y;
	Accel_X = mpux->ACCEL_X;
	Accel_Z = mpux->ACCEL_Z;
	Gyro_Y = mpux->GYRO_Y;
	
	if(Accel_X>32768) Accel_X-=65536; 
	if(Accel_Z>32768)  Accel_Z-=65536; 
	if(Gyro_Y>32768)  Gyro_Y-=65536;   
	 
	AngAccY=atan2(Accel_X,Accel_Z)*180/PI;    
	Gyro_Y=Gyro_Y/16.4;                       
	
	YijielvboY(&mpux->Pitch2f ,AngAccY,-Gyro_Y);  
	mpux->Pitch2 = mpux->Pitch2f*100;
} 

/**************************************************************************

**************************************************************************/
void Get_AngleRoll(uint8 Way_Angle)
{ 
	static float angleX;
  float Angle_Balance; 
  float AngAccX,Accel_Y,Accel_Z,Gyro_X;
  Accel_Y=MPU6050_ACCEL_YOUT_DATA;
  Accel_Z=MPU6050_ACCEL_ZOUT_DATA;
  Gyro_X=MPU6050_GYRO_XOUT_DATA;
  if(Way_Angle==1)                                      //DMP??¨®D¨¦??¡ã¦Ì?????¦Ì?¨º¡ÀD¨°?¨º¨¬a¡ê??¨²?¡Âo¡¥¨ºy?¨¢¨¨?
  {	
  }			
  else
  {
    //Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //?¨¢¨¨?Y?¨¢¨ª¨®?Y¨°?
    //Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L);    //?¨¢¨¨?Z?¨¢¨ª¨®?Y¨°?
    //Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //?¨¢¨¨?X?¨¢?¨®?¨´?¨¨??
    if(Gyro_X>32768)  Gyro_X-=65536;     //¨ºy?Y¨¤¨¤D¨ª¡Áa??  ¨°2?¨¦¨ª¡§1yshort????¨¤¨¤D¨ª¡Áa??
    if(Accel_Z>32768)  Accel_Z-=65536;     //¨ºy?Y¨¤¨¤D¨ª¡Áa??
    if(Accel_Y>32768) Accel_Y-=65536;    //¨ºy?Y¨¤¨¤D¨ª¡Áa??
    AngAccX=atan2(Accel_Y,Accel_Z)*180/PI;                 //????¨®?¦Ì???¦Ì??D??	
    Gyro_X=Gyro_X/16.4;                                    //¨ª¨®?Y¨°?¨¢?3¨¬¡Áa??	
      
    YijielvboX(&angleX,AngAccX,Gyro_X);    //?£¤21??2¡§
    
    Angle_Balance=angleX;                                   //?¨¹D???oa????
    ANGBLANX=Angle_Balance*100;
  }
}
void Get_Roll(struct mpu_e * mpux)
{
	float AngAccX,Accel_Y,Accel_Z,Gyro_X;
	Accel_Y = mpux->ACCEL_Y;
	Accel_Z = mpux->ACCEL_Z;
	Gyro_X = mpux->GYRO_X;
	 
	if(Accel_Y>32768) Accel_Y-=65536;   
	if(Accel_Z>32768)  Accel_Z-=65536; 
	if(Gyro_X>32768)  Gyro_X-=65536; 
	
	AngAccX=atan2(Accel_Y,Accel_Z)*180/PI;
	Gyro_X=Gyro_X/16.4;                   
  
	YijielvboX(&mpux->Roll2f ,AngAccX,Gyro_X);    
	
	mpux->Roll2 = mpux->Roll2f*100;
} 

