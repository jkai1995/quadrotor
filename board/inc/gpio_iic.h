#ifndef __GPIO_IIC_H
#define __GPIO_IIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������V3
//MPU6050 IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/17
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 	   		   
//IO��������
#define MPU_SDA_IN()  {GPIOE->MODER&=~(3<<(1*2));GPIOE->MODER|=0<<1*2;}	//PE1����ģʽ
#define MPU_SDA_OUT() {GPIOE->MODER&=~(3<<(1*2));GPIOE->MODER|=1<<1*2;} //PE1���ģʽ

//IO��������	 
#define MPU_IIC_SCL    PEout(0) 		//SCL
#define MPU_IIC_SDA    PEout(1) 		//SDA	 
#define MPU_READ_SDA   PEin(1) 		//����SDA 

//IIC���в�������
void MPU_IIC_Delay(void);				//MPU IIC��ʱ����
void MPU_IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void MPU_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 MPU_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 MPU_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void MPU_IIC_Ack(void);					//IIC����ACK�ź�
void MPU_IIC_NAck(void);				//IIC������ACK�ź�

void IMPU_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr,u8 addr);	  

u8 IIC_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);//IIC����д
u8 IIC_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf); //IIC������ 
u8 IIC_Write_Byte(u8 addr,u8 reg,u8 data);				//IICдһ���ֽ�
u8 IIC_Read_Byte(u8 addr,u8 reg);						//IIC��һ���ֽ�
u8 I2C_NoAddr_WriteByte(unsigned char DeviceAddr,unsigned char info) ;//дһ���ֽڣ�û�е�ַ�ź�

#endif
















