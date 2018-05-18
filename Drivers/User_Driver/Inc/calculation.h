#ifndef __CALCULATION_H
#define __CALCULATION_H

#include "stm32f1xx_hal.h"	  
#include "prj_typedef.h"
#include "relay.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

extern MonitorCH_TypeDef hMonitorCH_Voltage;  //handle of monitor voltage
extern MonitorCH_TypeDef hMonitorCH_Current;  //handle of monitor current
extern MonitorCH_TypeDef hMeasureCH_Voltage;  //handle of measure voltage

extern Float_Union_Data Adj_MeasIOffset;                     //У׼ADC��ص���ƫ����
extern Float_Union_Data Adj_MeasILinear;                     //У׼ADC��ص������Զ�
extern Float_Union_Data Adj_MeasVOffset;                     //У׼ADC��ص�ѹƫ����
extern Float_Union_Data Adj_MeasVLinear;                     //У׼ADC��ص�ѹ���Զ�
extern Float_Union_Data Adj_RelayOffset[10];                 //У׼�ɼ�����λ����ƫ����
extern Float_Union_Data Adj_PosRelayLinear[10];              //У׼�������ɼ����Զ�  
extern Float_Union_Data Adj_NegRelayLinear[10];              //У׼�������ɼ����Զ�

float MyPow(float base,uint8_t time);
uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, uint16_t origin_adcvalue_I, uint16_t origin_adcvalue_V);

#endif
	 


