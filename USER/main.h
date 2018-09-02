#ifndef __MAIN_H
#define __MAIN_H

#include "priority.h"

#ifdef __cplusplus
extern "C"
{

#endif
////////


/////////////////////////////////////////
///////////////////////////////////c”Ô—‘ø‚//////////////
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

/////////ucosiii//////////
#include "includes.h"
#include "os_app_hooks.h"

//////sys////
#include "delay.h"
#include "sys.h"
#include "usart.h"
//#include "timer.h"
//#include "spi.h"

#include <math.h>
#include "arm_math.h"
//////bsp/////
//#include "bsp_led.h"
#include "bsp_key.h"
//#include "lcd.h"
//#include "cfont.h"
#include "pwm.h"
#include "adc.h"
#include "nRF24L01_API.h"
#include "VCAN_LCD.h"
#include "VCAN_LCD_ST7735R.h"
#include "VCAN_ASCII.h"
#include "gpio_iic.h"

/////////mpu6050/////
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "dmpKey.h"
#include "dmpmap.h"

//////app//////

//#include "led.h"
#include "spi.h"
//////////////////////////////////////


#ifdef __cplusplus
}
#endif
////////////


#ifdef __cplusplus


/////////////////////////////////////////
///////////////////////////////////c++ø‚//////////////
#include "app_adc.h"
#include "app_lcd.h"
#include "app_2401.h"
#include "app_gesture.h"
#include "app_buzz.h"
#include "app_motor.h"
#include "key.h"
#include "sonic_irq.h"
#include "control_data.h"
#include "camera.h"
#include "FreeCars_uart.h"
#include "CoordinateTransformation.h"
#endif

#define MILISECON_PER_TICK (1000/OSCfg_TickRate_Hz)
#define MILISECON_TO_TICK(a) (a/(MILISECON_PER_TICK))

////////
#endif


