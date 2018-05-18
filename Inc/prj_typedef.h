#ifndef __PRJ_TYPEDEF_H
#define __PRJ_TYPEDEF_H

#include "stm32f1xx_hal.h"
#include "usbd_custom_hid_if.h"


enum MsgPort
{
	MSG_PORT_RS485=0,
	MSG_PORT_USB=1
};

enum MsgType
{
	MSG_TYPE_NULL=0,
	MSG_TYPE_SETTING=1,					
	MSG_TYPE_COMMAND=2,
	MSG_TYPE_CALIBRATION=3,
	MSG_TYPE_QUERY=4
};

enum TestStatus
{
	ON=0,
	OFF=1,
	PAUSED=2
};

enum TestCMD
{
	CMD_START=0x11,
	CMD_PAUSE=0x12,
	CMD_STOP=0x13,
	CMD_CONTINUE=0x14
};

enum TestMode
{
	MODE_CALIBRATION=0,
	MODE_FVMI_NO_SWEEP=1,
	MODE_FIMV_NO_SWEEP=2,
	MODE_FVMI_SWEEP=3,
	MODE_FIMV_SWEEP=4
	
};

typedef union										//电流值数据组织形式
{
	uint8_t number_uchar[4]; 
	float number_float;
}Float_Union_Data;

enum CaliMode										//校准模式
{
	SHORT=0,
	LOAD100=1,
	LOAD10k=2,
	LOAD1M=3,
	LOAD100M=4,
	OUTPUTVOLTAGE_ZERO=0x80,
	OUTPUTVOLTAGE_FULL=0x81,
	OPEN=0xFF
};

typedef struct
{
	enum MsgPort msg_port;
	enum MsgType msg_type;
}SMU_TypeDef;

typedef struct
{
	enum TestStatus test_status;					//测试状态
	enum TestMode test_mode;
	enum TestCMD	test_cmd;
	enum CaliMode test_calimode;
	uint16_t test_timestep;								//timestep decides the time delay between each output point and tansmission, default is 1000ms
	uint16_t test_samplerate;							//samplerate decides main current monitor ADC sampling speed, default is 1000  		
	int16_t test_Vstart;									//voltage gate start
	int16_t test_Vend;										//voltage gate end
	int16_t test_Vcurrent;								//the current voltage of gate output
	int16_t test_Istart;									//voltage drain start
	int16_t test_Iend;										//voltage drain end
	int16_t test_Icurrent;								//the current voltage of drain output
	int16_t test_step;										//step for sweeping use
	uint16_t test_averagenumber;					//默认16个点取平均值
	uint16_t test_silenttime;							//默认静默时间是2s
	int16_t test_startdelay;

}TestPara_TypeDef;

typedef struct
{
	Float_Union_Data test_result_I;				//单次测量值
	Float_Union_Data test_result_I_sum;		//单次测量值累加			
	Float_Union_Data test_result_I_avg;		//单次测量值平均
	Float_Union_Data test_result_V;					
	Float_Union_Data test_result_V_sum;		
	Float_Union_Data test_result_V_avg;	
	uint32_t ADC_sample_count;
	uint32_t ADC_sample_sum_I;						//电流ADC采样累加
	uint32_t ADC_sample_sum_V;						//电压ADC采样累加

}TestResult_TypeDef;

enum SamplingStatus
{
	SAMPLING_READY=0,
	SAMPLING_BUSY=1
};

#endif

