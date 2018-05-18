#ifndef __PC485_TX_PROTOCOL_H
#define __PC485_TX_PROTOCOL_H

#include "stm32f1xx_hal.h"
#include "prj_typedef.h"
#include "relay.h"
#include "hardware.h"

#define NOTENDOFTEST	0xF2
#define ENDOFTEST 0xF3
	
extern uint8_t UART2_TxBuf[11];                              //向PC发送采集到的数据;

void PC485_TX_Message(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, uint8_t EndOfTest);
void PC485_Respond(void);
void PC485_Update(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult);

#endif


