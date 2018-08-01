/**
  ******************************************************************************
  * @file    FreeCars_uart.h
  * @author  FreeCars雄哥
  * @version NULL
  * @date    2014/11/11
  * @brief 	 FreeCars_uart H file.(For XS128)
  *   site:   Http://FreeCars.taobao.com
  *   QQ群：  384273254，149168724
  *   版权：  此代码为FreeCars上位机协议代码，允许任意使用，也允许用于商业用途，但请保留此段文字！
  *   tips:   强烈建议小伙伴们使用FreeCars出品的蓝牙模块套装，无线上位机从这里开始！
  ******************************************************************************
  */
#ifndef __FREECARS_UART_H__
#define __FREECARS_UART_H__

//1 头文件

#include "main.h"

#define FREECAES_UART USART2       
//USART_SendData(USART1, USART_RX_BUF[t]); 
#define FREECAES_SCI_SEND_CHANNEL(x) {USART_SendData (FREECAES_UART, x);while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);}//等待发送结束  //保证sci发送函数 SCI_send()和sci.c中定的匹配 


//以下不要修改
#define UartRxBufferLen  100
#define UartRxDataLen    41           //FreeCars上位机发送浮点数据MCU接收，不要改
#define UartRxCmdLen     7	      //FreeCars上位机接收命令数据长度，不要改
#define UartDataNum      11           //FreeCars上位机接收通道数，按照上位机设置改变!!!

#define UartCmdNum  SerialPortRx.Buffer[SerialPortRx.Stack-3]//命令号
#define UartCmdData SerialPortRx.Buffer[SerialPortRx.Stack-2]//命令数据









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
