#include "CReceiveSend.h"
#include "control_data.h"

#define MILISECON_PER_TICK (1000/OSCfg_TickRate_Hz)
#define MILISECON_TO_TICK(a) (a/(MILISECON_PER_TICK))
static CReceiveSend _CRS;
CReceiveSend* CReceiveSend::_instance = NULL;

CReceiveSend* CReceiveSend::getInstance(void)
{
//	if(_instance == NULL)
//	{
//		_instance = new CReceiveSend();
//	}
	return _instance;
}

CReceiveSend::CReceiveSend(void)
{
	m_sendMsg.eDataDire = eDataSend;
	m_pTcb = NULL;
	if(getInstance() == NULL)
		_instance = this;
}

bool CReceiveSend::initCRS(OS_TCB *tcb)
{
	m_pTcb = tcb;
	
	if(NRF24L01_Check() == 0)
	{
		m_Nrf2401IsExist = true;
		NRF24L01_RT_Init(tcb);
	}
	else
	{
		m_Nrf2401IsExist = false;
	}
	
	return m_Nrf2401IsExist;
}

void CReceiveSend::sendData(u8 *data,u16 len )
{
	
	if(m_pTcb == NULL 
		||m_Nrf2401IsExist == false)
	{
		return;
	}
		
	OS_ERR err;
	m_sendMsg.data = data;
	m_sendMsg.len = len;
	OSTaskQPost (m_pTcb,
							 &m_sendMsg,
							 sizeof(m_sendMsg),
							 OS_OPT_POST_FIFO,
							 &err);
							
}

void CReceiveSend::run(void)
{
	OS_ERR err;
	u8 rece_buf[32];
	CPU_TS ts;
	OS_MSG_SIZE msg_size;
	SRSMessage *srMsg;
	while(1)
	{
		srMsg = (SRSMessage*)OSTaskQPend (MILISECON_TO_TICK(20),
																			OS_OPT_PEND_BLOCKING,
																			&msg_size,
																			&ts,
																			&err);
		
		if(err == OS_ERR_NONE || NRF_IRQ == 0 )
		{
			if(srMsg->eDataDire == eDataSend)
			{
				SEND_BUF(srMsg->data);
			}
			else// if(srMsg->eDataDire == eDataReceive)
			{
				if(NRF24L01_RxPacket(rece_buf)==0)
				{
					lode_rece_data(rece_buf);
					m_outCtrl = 0;
				}
			}
		}
		else if(err == OS_ERR_TIMEOUT)
		{
			m_outCtrl++;
			if(m_outCtrl == 50)
			{
				
				motor_lock = LOCKED; //遥控超时，上锁飞机
				set_buzz_mod(BUZZ_OUT_CONTR);//遥控超时提示
			}
			if(m_outCtrl >= 60)
			{
				m_outCtrl = 60;
			}
		}
	}
}

