#ifndef CRECEIVESEND_H
#define CRECEIVESEND_H




#ifdef __cplusplus
extern "C"
{
#endif
	
#include "nRF24L01_API.h"	
#include "stm32f4xx.h"
#include "includes.h"
//#include "stdio.h"
	
#ifdef __cplusplus
}
#endif


//data receive and send
class CReceiveSend
{
public:
	CReceiveSend(void);
	static CReceiveSend* getInstance(void);
	void sendData(u8 *data,u16 len);
	bool RSModuleIsExist(){return m_Nrf2401IsExist;}
	//void setTCB(OS_TCB *tcb);
	bool initCRS(OS_TCB *tcb);
	void run(void);
private:
		s16 m_outCtrl;
		OS_TCB *m_pTcb;
		SRSMessage m_sendMsg; 
		bool m_Nrf2401IsExist;
		static CReceiveSend *_instance;
};



#endif
