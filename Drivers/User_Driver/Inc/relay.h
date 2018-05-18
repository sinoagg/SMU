#ifndef __RELAY_H
#define __RELAY_H	 

#include "stm32f1xx_hal.h"	   
#include "main.h"
#include "prj_typedef.h"
#include "AD7988_1.h"

#define RANGE_CHANGE_DELAY 80								//delay time between each relay change in ms

#define AUTO_RANGE 0                        //自动换挡模式
#define FIXED_RANGE 1								        //手动换挡模式

#define RELAY_CHANGE_OVER 1                 //换挡结束
#define RELAY_CHANGE_CONTINUE 0             //换挡继续

#define RELAY_INPUT_SCALING_1X 0            //放大1倍
#define RELAY_INPUT_SCALING_6X 1            //放大6倍

//#define RELAY_INPUT_SCALING_5D6  0           //缩小为原值的5/6
//#define RELAY_INPUT_SCALING_1D12 1           //缩小为原值的1/12

#define RELAY_TEST_MODE_FVMI 1
#define RELAY_TEST_MODE_FIMV 2

#define RELAY_RANGE_1R 1
#define RELAY_RANGE_10R 2
#define RELAY_RANGE_100R 3
#define RELAY_RANGE_1K 4
#define RELAY_RANGE_10K 5
#define RELAY_RANGE_100K 6
#define RELAY_RANGE_1M 7
#define RELAY_RANGE_10M 8
#define RELAY_RANGE_100M 9
#define RELAY_RANGE_1G 10

#define DEFAULT_RELAY_INPUT_SCALING RELAY_INPUT_SCALING_6X
#define DEFAULT_RELAY_TEST_MODE RELAY_TEST_MODE_FIMV
#define DEFAULT_RELAY_CHANGE_MODE AUTO_RANGE
#define DEFAULT_RELAY_MAX_RANGE RELAY_RANGE_1G
#define DEFAULT_RELAY_MIN_RANGE RELAY_RANGE_1R
#define DEFAULT_RELAY_RANGE RELAY_RANGE_100K


typedef struct
{
	uint8_t relay_input_scaling;							//输入缩放倍数
	uint8_t relay_test_mode;									//恒压或恒流测试
	uint8_t relay_change_mode;								//自动或手动换挡：0为自动换挡，1-10为手动换挡的档位值
	uint8_t	relay_max_range;									//最大挡位
	uint8_t relay_min_range;									//最小挡位
	uint8_t relay_range;											//目前所用挡位
	uint8_t origin_state;                     //原始档位计数
	uint8_t origin_range;                     //原始档位
	uint8_t relay_change;                     //换挡结束标志，0为继续，1为结束
}Relay_TypeDef;

typedef struct
{
	uint8_t monitorch_relaystate;
	AD7988_1_HandleTypeDef *phAD7988_1_x;
}MonitorCH_TypeDef;

extern Relay_TypeDef Relay;

void Relay_Init(Relay_TypeDef *pRelay);
void Relay_Clear(void);
void Set_Relay(uint8_t range_select) ;      
uint8_t Relay_Check_Measure(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pResultPara, MonitorCH_TypeDef *phMonitorCH, uint32_t origin_adcvalue_I, uint32_t origin_adcvalue_V);
float Relay_Check_Monitor(MonitorCH_TypeDef *phMonitorCH,	uint8_t monitor_mode);

static void Relay_Set_Input_Scaling(uint8_t scale);
void Relay_Set_Test_Mode(uint8_t testmode);
//void Relay_ADC_Voltage_Scaling(uint8_t scale);  //设置ADC2采集电压缩小倍数
//void Relay_ADC_Current_Scaling(uint8_t scale);  //设置ADC1采集电流缩小倍数

#endif

