#include "heightSolution.h"

//acc  ���Ų��� 1g��Լ�� 3900
static s16 acc_z_zero = 3900;

float solution_height_mm = 0;
float solution_speed_mmPs = 0;

s32 get_height()
{
	u32 height = get_sonic_heightmm();
	return (s32)height;
}

float K2 = 0.015625;
float K1 = 0.015625;
// Kacc * acc_z_zero() * 200(��) = 10,000(mm/s)
//acc_z_zero��һ��g�ļ��ٶȣ�200Hz�ļ���Ƶ�ʣ�
//һ����Ӧ������ 10,000 (����ÿ��)
//Kacc�ǽ������ٶȼƵ�λ��ϵ��
float Kacc = 0.0128;
float Kspeed = 0.005;
s16 accz;
s32 sen_height_mm;
float h_err;
s32 speed_err;
void heightSolution()
{
	accz = get_earth_accz() - acc_z_zero;
	sen_height_mm = get_height();
	
	h_err = solution_height_mm - sen_height_mm;
	
	solution_speed_mmPs += (Kacc*accz - h_err*K2);
	solution_height_mm += (Kspeed*solution_speed_mmPs - h_err *K1);
}
