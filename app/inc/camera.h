#ifndef __CAMERA_H
#define __CAMERA_H

#include "main.h"

typedef struct
{
	u8 	cmd;//����
	s16 pitch;//����
	s16 roll;//����
	s16	yaw;//����
}ST_CameraData_TypeDef;

extern ST_CameraData_TypeDef st_camera_data;


#ifdef __cplusplus
extern "C"
{
#endif
/* c��������*/
/* c��������*/
#ifdef __cplusplus
}
#endif
//////




#endif 
