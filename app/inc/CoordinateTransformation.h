#ifndef __COORDINATETRANSFORMATION_H
#define __COORDINATETRANSFORMATION_H

#include "main.h"

typedef struct
{
	s16 X;
	s16	Y;
	s16	Z;
}ST_Coordinate_TypeDef;

extern	float x1,y1,z1;
extern ST_Coordinate_TypeDef st_coordinate_earth;
extern ST_Coordinate_TypeDef st_coordinate_earth_zero;
#ifdef __cplusplus
extern "C"
{
#endif
/* c”Ô—‘ƒ⁄»›*/
/* c”Ô—‘ƒ⁄»›*/
#ifdef __cplusplus
}
#endif

void AngleTransforming(float x,float y,float z,float AnglePitch,float AngleYaw,float AngleRoll);
s16 get_earth_accz();


#endif 
