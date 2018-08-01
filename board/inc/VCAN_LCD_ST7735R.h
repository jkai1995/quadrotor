/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,ɽ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�ɽ����̳ http://www.vcan123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
 *
 * @file       VCAN_LCD_ST7735R.h
 * @brief      LCD ������
 * @author     ɽ��Ƽ�
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
 * ������δ�С�ṹ��
 */
typedef struct
{
    uint16 W;       //��
    uint16 H;       //��
} Size_t;


/*
 * ��������ṹ��
 */
typedef struct
{
    uint16 x;
    uint16 y;
} Site_t;

/*
 * �����λ�������������
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
} Dtype;    //sizeof(Dtype) Ϊ 4

#define PE_height_OUT(data)  {GPIOE->BSRRL=(uint16)((data)<<8);GPIOE->BSRRH=(uint16)(~((data<<8)|0xff));} //PE BSRRL �ɽ���Ӧλ��1 BSRRH�ɽ���Ӧλ��0


//#include "VCAN_LCD.h"

//�û�����
#define ST7735R_DIR_DEFAULT     0       //����Ĭ�Ϸ��� (0~3)

#if (USE_LCD == LCD_ST7735R)

/*********************** API�ӿ� ******************************/
//�ṩAPI�ӿڸ�LCD����

#define LCD_H                   ST7735R_get_h()                 //��
#define LCD_W                   ST7735R_get_w()                 //��

#define LCD_INIT()              LCD_ST7735R_init()              //��ʼ��
#define LCD_PTLON(site,size)    LCD_ST7735R_ptlon(site,size)    //����
#define LCD_RAMWR()             LCD_WR_CMD(0x2C)                //дģʽ
#define LCD_WR_DATA(data)       do{LCD_ST7735R_WR_8DATA((uint8)((data)>>8) );LCD_ST7735R_WR_8DATA( (uint8)(data) );}while(0)       //д����
#define LCD_WR_CMD(cmd)         LCD_ST7735R_WR_8CMD(cmd)        //����
#define LCD_SET_DIR(opt)        LCD_ST7735R_dir(opt)            //����

#define LCD_DIR                 ST7735R_get_dir()               //��ȡ����


//ͨ���������ã��ٶ�̫���ˣ��ĳɺ궨�����
//#define LCD_WR_8DATA(data)      LCD_ST7735R_wr_data(data)       //д����
//#define LCD_WR_8CMD(cmd)        LCD_ST7735R_wr_cmd(cmd)         //����

#endif  //(USE_LCD == LCD_ST7735R)

/********************* ST7735R�ڲ�ʵ�� ***********************/
//LCD �ĹܽŶ���
////#define     LCD_ST7735R_WR      PTB2
////#define     LCD_ST7735R_RD      PTB11
////#define     LCD_ST7735R_CS      PTB0
////#define     LCD_ST7735R_RS      PTB7
////#define     LCD_ST7735R_RST     PTB1

//�˲�����Ҫ�޸ĳ�ʼ����Ĵ���
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
                            }while(0)   //LCD_WR=0;LCD_WR=1;����һ��������

#define LCD_ST7735R_WR_8DATA(data)  do\
                            {\
                                LCD_ST7735R_RD_OUT=1;\
                                LCD_ST7735R_RS_OUT=1;\
                                LCD_ST7735R_CS_OUT=0;\
                                LCD_ST7735R_P0(data);\
                                LCD_ST7735R_WR_OUT=0;\
                                LCD_ST7735R_WR_OUT=1;\
                                LCD_ST7735R_CS_OUT=1;\
                            }while(0)   //LCD_WR=0;������д�����ݵ�RAM


#define LCD_ST7735R_WR_DATA(data)       do{LCD_ST7735R_WR_8DATA((uint8)((data)>>8) );LCD_ST7735R_WR_8DATA( (uint8)(data) );}while(0)       //д����



#endif  //_VCAN_LCD_ST7735R_H_
