#ifndef __nRF24L01_API_H
#define __nRF24L01_API_H


#ifdef __cplusplus
extern "C"
{
#endif
	

#include "nRF24L01.h"
#include "includes.h"
	
#define uchar unsigned char
#define uint unsigned int

#define NRF_CSN   PCout(2)  //Ƭѡ�źţ��������  
#define NRF_CE   	PCout(3)  // �������
#define NRF_MOSI  PCout(0)  // MOSI ����������� ���������ʳ�ʼ��PC_CR2�Ĵ�����
#define NRF_SCK   PCout(1)  //���ͽ���ģʽѡ���������
#define NRF_IRQ   PEin(3)  //IRQ������������,��������
#define NRF_MISO  PEin(4)  // MISO

typedef enum ENRF2401MODE
{
	eNRF2401_RX,
	eNRF2401_TX,
}tNrf2401Mode;

typedef enum EDATADIRECTION
{
	eDataReceive,
	eDataSend,
	eDataDirectionEnd,
}EDataDirection;

typedef struct SRSMESSAGE
{
	EDataDirection eDataDire;
	uchar *data;
	uint len;
}SRSMessage;
	
//void delay_us(uchar num);
void delay_150us(void);
uchar SPI_RW(uchar byte);
uchar NRF24L01_Write_Reg(uchar reg,uchar value);
uchar NRF24L01_Read_Reg(uchar reg);
uchar NRF24L01_Read_Buf(uchar reg,uchar *pBuf,uchar len);
uchar NRF24L01_Write_Buf(uchar reg, uchar *pBuf, uchar len);
uchar NRF24L01_RxPacket(uchar *rxbuf);
uchar NRF24L01_TxPacket(uchar *txbuf);
uchar NRF24L01_Check(void);
void NRF24L01_RT_Init(OS_TCB * tcb);
void SEND_BUF(uchar *buf);

tNrf2401Mode getNrfMode(void);
void nrf_IRQ_interrupt(void);

#ifdef __cplusplus
}
#endif

#endif
