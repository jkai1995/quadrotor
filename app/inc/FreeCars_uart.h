/**
  ******************************************************************************
  * @file    FreeCars_uart.h
  * @author  FreeCars�۸�
  * @version NULL
  * @date    2014/11/11
  * @brief 	 FreeCars_uart H file.(For XS128)
  *   site:   Http://FreeCars.taobao.com
  *   QQȺ��  384273254��149168724
  *   ��Ȩ��  �˴���ΪFreeCars��λ��Э����룬��������ʹ�ã�Ҳ����������ҵ��;�����뱣���˶����֣�
  *   tips:   ǿ�ҽ���С�����ʹ��FreeCars��Ʒ������ģ����װ��������λ�������￪ʼ��
  ******************************************************************************
  */
#ifndef __FREECARS_UART_H__
#define __FREECARS_UART_H__

//1 ͷ�ļ�

#include "main.h"

#define FREECAES_UART USART2       
//USART_SendData(USART1, USART_RX_BUF[t]); 
#define FREECAES_SCI_SEND_CHANNEL(x) {USART_SendData (FREECAES_UART, x);while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);}//�ȴ����ͽ���  //��֤sci���ͺ��� SCI_send()��sci.c�ж���ƥ�� 


//���²�Ҫ�޸�
#define UartRxBufferLen  100
#define UartRxDataLen    41           //FreeCars��λ�����͸�������MCU���գ���Ҫ��
#define UartRxCmdLen     7	      //FreeCars��λ�������������ݳ��ȣ���Ҫ��
#define UartDataNum      11           //FreeCars��λ������ͨ������������λ�����øı�!!!

#define UartCmdNum  SerialPortRx.Buffer[SerialPortRx.Stack-3]//�����
#define UartCmdData SerialPortRx.Buffer[SerialPortRx.Stack-2]//��������









typedef struct
{
  long int Stack;
  uint8 Data;
  uint8 PreData;
  uint8 Buffer[UartRxBufferLen];
  uint8 Enable;
  uint8 Check;
}SerialPortType;

//Export Varibales
extern uint8 uSendBuf[UartDataNum*2];
extern SerialPortType SerialPortRx;
extern double UartData[9];

//Export Functions
void sendDataToScope(void);
void push(uint8 adr,uint16 date);
void WirelessDateSend (void);

#endif
