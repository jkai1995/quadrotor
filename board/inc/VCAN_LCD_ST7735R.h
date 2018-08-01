/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,山外科技
 *     All rights reserved.
 *     技术讨论：山外论坛 http://www.vcan123.com
 *
 *     除注明出处外，以下所有内容版权均属山外科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留山外科技的版权声明。
 *
 * @file       VCAN_LCD_ST7735R.h
 * @brief      LCD 函数库
 * @author     山外科技
 * @version    v5.0
 * @date       2013-09-07
 */

#ifndef _VCAN_LCD_ST7735R_H_
#define _VCAN_LCD_ST7735R_H_

#include "stm32f4xx_conf.h"
#include "sys.h"
#include "delay.h"
#define ASSERT assert_param
typedef u8   uint8;
typedef u16  uint16;
typedef u32  uint32;
typedef s8   int8 ;
typedef s16  int16;
typedef s32  int32;
/*
 * 定义矩形大小结构体
 */
typedef struct
{
    uint16 W;       //宽
    uint16 H;       //高
} Size_t;


/*
 * 定义坐标结构体
 */
typedef struct
{
    uint16 x;
    uint16 y;
} Site_t;

/*
 * 定义带位域的联合体类型
 */
typedef union
{
    uint16  W;
    uint8   B[2];
    struct
    {
        uint16 b0: 1;
        uint16 b1: 1;
        uint16 b2: 1;
        uint16 b3: 1;
        uint16 b4: 1;
        uint16 b5: 1;
        uint16 b6: 1;
        uint16 b7: 1;
        uint16 b8: 1;
        uint16 b9: 1;
        uint16 b10: 1;
        uint16 b11: 1;
        uint16 b12: 1;
        uint16 b13: 1;
        uint16 b14: 1;
        uint16 b15: 1;

    }BIT;
} Dtype;    //sizeof(Dtype) 为 4

#define PE_height_OUT(data)  {GPIOE->BSRRL=(uint16)((data)<<8);GPIOE->BSRRH=(uint16)(~((data<<8)|0xff));} //PE BSRRL 可将对应位置1 BSRRH可将对应位置0


//#include "VCAN_LCD.h"

//用户配置
#define ST7735R_DIR_DEFAULT     0       //定义默认方向 (0~3)

#if (USE_LCD == LCD_ST7735R)

/*********************** API接口 ******************************/
//提供API接口给LCD调用

#define LCD_H                   ST7735R_get_h()                 //高
#define LCD_W                   ST7735R_get_w()                 //宽

#define LCD_INIT()              LCD_ST7735R_init()              //初始化
#define LCD_PTLON(site,size)    LCD_ST7735R_ptlon(site,size)    //开窗
#define LCD_RAMWR()             LCD_WR_CMD(0x2C)                //写模式
#define LCD_WR_DATA(data)       do{LCD_ST7735R_WR_8DATA((uint8)((data)>>8) );LCD_ST7735R_WR_8DATA( (uint8)(data) );}while(0)       //写数据
#define LCD_WR_CMD(cmd)         LCD_ST7735R_WR_8CMD(cmd)        //命令
#define LCD_SET_DIR(opt)        LCD_ST7735R_dir(opt)            //方向

#define LCD_DIR                 ST7735R_get_dir()               //获取方向


//通过函数调用，速度太慢了，改成宏定义调用
//#define LCD_WR_8DATA(data)      LCD_ST7735R_wr_data(data)       //写数据
//#define LCD_WR_8CMD(cmd)        LCD_ST7735R_wr_cmd(cmd)         //命令

#endif  //(USE_LCD == LCD_ST7735R)

/********************* ST7735R内部实现 ***********************/
//LCD 的管脚定义
////#define     LCD_ST7735R_WR      PTB2
////#define     LCD_ST7735R_RD      PTB11
////#define     LCD_ST7735R_CS      PTB0
////#define     LCD_ST7735R_RS      PTB7
////#define     LCD_ST7735R_RST     PTB1

//此部分需要修改初始化里的代码
#define     LCD_ST7735R_P0(data)        PE_height_OUT(data)
#define     LCD_ST7735R_PIN             PTB_B2_IN
#define     LCD_ST7735R_PDDR_OUT()      (PTB_B2_DDR = 0xff)
#define     LCD_ST7735R_PDDR_IN()       (PTB_B2_DDR = 0)


#define ST7735R_DELAY()                         //do{asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");}while(0)
#define ST7735R_DELAY_MS(ms)    delay_ms(ms)


extern void     LCD_ST7735R_init(void);
extern void     LCD_ST7735R_dir(uint8 option);
extern void     LCD_ST7735R_ptlon(Site_t site, Size_t size);
extern uint16   ST7735R_get_h(void);
extern uint16   ST7735R_get_w(void);
extern uint8    ST7735R_get_dir(void);


extern void LCD_ST7735R_wr_data(uint8 data);
extern void LCD_ST7735R_wr_cmd(uint8 cmd);

#define     LCD_ST7735R_WR_OUT      PCout(4) 
#define     LCD_ST7735R_RD_OUT      PEout(7)
#define     LCD_ST7735R_CS_OUT      PAout(5)
#define     LCD_ST7735R_RS_OUT      PBout(0)
#define     LCD_ST7735R_RST_OUT     PAout(7)

#define LCD_ST7735R_WR_8CMD(cmd)    do\
                            {\
                                LCD_ST7735R_RD_OUT=1;\
                                LCD_ST7735R_RS_OUT=0;\
                                LCD_ST7735R_CS_OUT=0;\
                                LCD_ST7735R_P0(cmd);\
                                LCD_ST7735R_WR_OUT=0;\
                                LCD_ST7735R_WR_OUT=1;\
                                LCD_ST7735R_CS_OUT=1;\
                            }while(0)   //LCD_WR=0;LCD_WR=1;产生一个上升沿

#define LCD_ST7735R_WR_8DATA(data)  do\
                            {\
                                LCD_ST7735R_RD_OUT=1;\
                                LCD_ST7735R_RS_OUT=1;\
                                LCD_ST7735R_CS_OUT=0;\
                                LCD_ST7735R_P0(data);\
                                LCD_ST7735R_WR_OUT=0;\
                                LCD_ST7735R_WR_OUT=1;\
                                LCD_ST7735R_CS_OUT=1;\
                            }while(0)   //LCD_WR=0;在这里写入数据到RAM


#define LCD_ST7735R_WR_DATA(data)       do{LCD_ST7735R_WR_8DATA((uint8)((data)>>8) );LCD_ST7735R_WR_8DATA( (uint8)(data) );}while(0)       //写数据



#endif  //_VCAN_LCD_ST7735R_H_
