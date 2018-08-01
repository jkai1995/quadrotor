#ifndef __KalmanandYijielvbo_H
#define __KalmanandYijielvbo_H


#include "sys.h"

#define uint8 u8



void Kalman_FilterY(float Accel,float Gyro);
void YijielvboY(float *pit, float angle_m, float gyro_m);
void YijielvboX(float *rol, float angle_m, float gyro_m);
void Get_AngleY(uint8 Way_Angle);
void Get_AngleX(uint8 Way_Angle);


void Get_Pitch(struct mpu_e * mpux);
void Get_Roll(struct mpu_e * mpux);

#endif
