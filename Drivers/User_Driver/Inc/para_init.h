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

extern Float_Union_Data Adj_OutputOffset;                //输出电压校准系数：偏移量
extern Float_Union_Data Adj_OutputLinear;                //输出电压校准系数：线性度
extern Float_Union_Data Adj_MeasIOffset;                     //校准ADC监控电流偏移量
extern Float_Union_Data Adj_MeasILinear;                     //校准ADC监控电流线性度
extern Float_Union_Data Adj_MeasVOffset;                     //校准ADC监控电压偏移量
extern Float_Union_Data Adj_MeasVLinear;                     //校准ADC监控电压线性度
extern Float_Union_Data Adj_RelayOffset[10];                 //校准采集各档位电流偏移量
extern Float_Union_Data Adj_PosRelayLinear[10];              //校准正电流采集线性度  
extern Float_Union_Data Adj_NegRelayLinear[10];              //校准负电流采集线性度

void InitTestPara(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pResultPara);
uint8_t GetTestPara(TestPara_TypeDef* pTestPara, enum MsgType msgtype, enum MsgPort msgport);
void ClearArray(Float_Union_Data* pArray, uint8_t size);
void SetArray(Float_Union_Data* pArray, uint8_t size);

#endif 

