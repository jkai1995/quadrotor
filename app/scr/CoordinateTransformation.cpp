#include "CoordinateTransformation.h"

//	#define PI 3.1415926
	ST_Coordinate_TypeDef st_coordinate_earth;
	ST_Coordinate_TypeDef st_coordinate_earth_zero;
	float DCM[3][3];	//DirectionCosineMatrix
void EylerAnglesToRotationMatrix (float pitch,float roll,float yaw)
{
	///ZYX模式
	DCM[0][0] = (cos(roll)*cos(yaw));
	DCM[0][1]	= ((cos(yaw)*sin(roll)*sin(pitch))	- (cos(pitch)*sin(yaw)));////?
	DCM[0][2] = ((cos(pitch)*sin(roll)*cos(yaw)) + (sin(pitch)*sin(yaw)));
	DCM[1][0] = (cos(roll)*sin(yaw));/////////???
	DCM[1][1] = (cos(pitch)*cos(yaw) + sin(pitch)*sin(roll)*sin(yaw));
	DCM[1][2] = ((cos(pitch)*sin(roll)*sin(yaw)) - (sin(pitch)*cos(yaw)));
	DCM[2][0] = (-sin(roll));
	DCM[2][1] = (sin(pitch)*cos(roll));
	DCM[2][2] = (cos(pitch)*(cos(roll)));
	
}
	
	
	

//

//输入为加速度或者角速度值
//example：
//	/////坐标变换，，求垂直加速度
//  AngleTransforming((float)gest_6050.pitch_acc,(float)-gest_6050.roll_acc,(float)-gest_6050.yaw_acc,gest_6050.pitch,gest_6050.yaw,gest_6050.roll);
void AngleTransforming(float x,float y,float z,float AnglePitch,float AngleYaw,float AngleRoll)
{
	
	//输入的角度是以初始面为基准的，该函数要把值转换到初始面，因此角度加负号
	float pitch = -AnglePitch * PI / 180;
	float yaw   = -AngleYaw * PI / 180;
 	float roll  = AngleRoll * PI / 180;
	
	EylerAnglesToRotationMatrix(pitch,roll,yaw);


	st_coordinate_earth.X = 	(DCM[0][0]) * x
													+ (DCM[0][1]) * y
													+ (DCM[0][2]) * z; 	

	st_coordinate_earth.Y = 	(DCM[1][0]) * x 
													+ (DCM[1][1]) * y
													+ (DCM[1][2]) * z; 
 
  st_coordinate_earth.Z = 	(DCM[2][0]) * x
													+ (DCM[2][1]) * y
													+ (DCM[2][2]) * z; 

	//赋值给输出值
}
///////////


