#ifndef __TIMER_CTRL_H
#define __TIMER_CTRL_H

#include "stm32f1xx_hal.h"
#include "usbd_custom_hid_if.h"
#include "prj_typedef.h"
#include "relay.h"
#include "InternalFlash.h"
#include "hardware.h"

#define ADJ_GATE 0
#define ADJ_SOURCE 1
#define ADJ_DRAIN 2
#define ADJ_SUB 3

#define TIM_ON 1
#define TIM_OFF 0

#define SENSOR_PRESSURE 0
#define SENSOR_TEMPERATURE 1
#define SENSOR_HUMIDITY 2

extern uint8_t silenttime_past;							//静默时间过去标志位，0表示未过
extern uint8_t ADCread_status;							//ADC是否读取的标志位
extern uint8_t TX_Enable;										//是否可以传输标志位
extern TestPara_TypeDef TestPara;           //测量参数存取
extern TestResult_TypeDef TestResult;       //测量结果存取
extern uint16_t origin_adcvalue_I;			    //用来监控电流的ADC读数，对应7798_1_1
extern uint16_t origin_adcvalue_V;					//用来监控电压的ADC读数, 对应7798_1_2
extern MonitorCH_TypeDef hMeasureCH_Voltage;

void SetTimerPara(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult);
void SetTimerAction(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay);
void RangeSetting(Relay_TypeDef* pRelay);
void SetInitialVoltage(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay);
void ClearSignOutVol(Relay_TypeDef* pRelay);
void Test_Relay(void);

static void ChangeTimer(TestPara_TypeDef* pTestPara, uint8_t TIM_ONOFF);

#endif 

