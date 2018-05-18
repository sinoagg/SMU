#ifndef __PARA_INIT_H
#define __PARA_INIT_H

#include "stm32f1xx_hal.h"
#include "prj_typedef.h"
#include "relay.h"


#define TIM_ON 1
#define TIM_OFF 0

#define DEFAULT_SAMPLERATE 2000
#define DEFAULT_SAMPLENUMBER 400
#define DEFAULT_SILENTTIME 1000
#define DEFAULT_MAXRANGESELECT 10
#define DEFAULT_V_STEP 10						//mV
#define DEFAULT_I_STEP 1						

extern Float_Union_Data Adj_OutputOffset;                //�����ѹУ׼ϵ����ƫ����
extern Float_Union_Data Adj_OutputLinear;                //�����ѹУ׼ϵ�������Զ�
extern Float_Union_Data Adj_MeasIOffset;                     //У׼ADC��ص���ƫ����
extern Float_Union_Data Adj_MeasILinear;                     //У׼ADC��ص������Զ�
extern Float_Union_Data Adj_MeasVOffset;                     //У׼ADC��ص�ѹƫ����
extern Float_Union_Data Adj_MeasVLinear;                     //У׼ADC��ص�ѹ���Զ�
extern Float_Union_Data Adj_RelayOffset[10];                 //У׼�ɼ�����λ����ƫ����
extern Float_Union_Data Adj_PosRelayLinear[10];              //У׼�������ɼ����Զ�  
extern Float_Union_Data Adj_NegRelayLinear[10];              //У׼�������ɼ����Զ�

void InitTestPara(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pResultPara);
uint8_t GetTestPara(TestPara_TypeDef* pTestPara, enum MsgType msgtype, enum MsgPort msgport);
void ClearArray(Float_Union_Data* pArray, uint8_t size);
void SetArray(Float_Union_Data* pArray, uint8_t size);

#endif 

