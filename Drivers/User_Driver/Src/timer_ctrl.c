#include "timer_ctrl.h"

void SetTimerPara(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult)
{
	if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)	//在定点模式下，不用去开DAC中断，只设置ADC中断
	{
		Relay.relay_max_range=RELAY_RANGE_100M;               //定点模式下，自动换挡最大档位为10档
	}
	Relay_Set_Test_Mode(Relay.relay_test_mode);         //根据测量模式设置相应的继电器
	MX_TIM3_Init(pTestPara->test_timestep);							//DAC输出定时器,单位ms
	HAL_TIM_Base_Stop_IT(&htim3);    										//关闭DAC输出中断
	MX_TIM2_Init(10000/pTestPara->test_samplerate);			//ADC采集定时器，单位100us
	HAL_TIM_Base_Stop_IT(&htim2);     									//关闭ADC采集
}

void SetTimerAction(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay)
{
	if(pTestPara->test_cmd==CMD_START)										//开始采集的命令				
	{		
		pTestPara->test_status=ON;													//测试开始,有测试在运行中
		SetInitialVoltage(pTestPara,pRelay);
		HAL_Delay(pTestPara->test_silenttime);							//等待静默时间
		Test_Relay();																			  //既充当延时又测试档位，目前只是测试ADC值
		ChangeTimer(pTestPara, TIM_ON);											//打开相应定时器
	}
	else if(pTestPara->test_cmd==CMD_STOP)								//停止采集的命令
	{
		pTestPara->test_status=OFF;					
		ChangeTimer(pTestPara, TIM_OFF);		
		ADCread_status=0;
		pTestResult->ADC_sample_count=0;										//重新计数，清零
		pTestResult->ADC_sample_sum_I=0;										//重新累加，清零
		pTestResult->ADC_sample_sum_V=0;										//重新累加，清零
		TX_Enable=0;																		    //停止USB传输
		ClearSignOutVol(&Relay);
	}
  
}

void RangeSetting(Relay_TypeDef* pRelay)            //判断是否是自动换挡
{
	if(pRelay->relay_change_mode!=AUTO_RANGE)					//如果不是自动换挡
	{
		 pRelay->relay_range=pRelay->relay_change_mode; //将固定档位模式的对应档位赋值给档位值变量
		 Set_Relay(pRelay->relay_range);								//设置档位，后边有延时故不再加延时
	}
}

static void ChangeTimer(TestPara_TypeDef* pTestPara, uint8_t TIM_ONOFF)
{
	if(TIM_ONOFF == TIM_ON)
	{
		if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
		{
			HAL_TIM_Base_Start_IT(&htim3);								//DAC输出数据中断启动	
			HAL_Delay(2);																	//DAC输出之后固定等待2ms再开启采集
			HAL_TIM_Base_Start_IT(&htim2);								//ADC采集数据并回传中断启�
		}
	}
	else
	{
		HAL_TIM_Base_Stop_IT(&htim3);										//DAC输出数据中断启动	
		HAL_Delay(2);																		//DAC输出之后固定等待2ms再开启采集
		HAL_TIM_Base_Stop_IT(&htim2);										//ADC采集数据并回传中断停止	
	}
}

void SetInitialVoltage(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay)
{
	float vol=0;
	if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP)
	{
	   if(pRelay->relay_input_scaling==RELAY_INPUT_SCALING_1X)     //如果输出电流放大一倍
		   vol=(2.5-((float)pTestPara->test_Icurrent*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
	   else                                                        //如果输出电压放大六倍
		   vol=(2.5-((float)pTestPara->test_Icurrent*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
		 Set_Relay(Relay.relay_range);                               //根据电流值大小赋值
	}
	else if(pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
	{
		 if(pRelay->relay_input_scaling==RELAY_INPUT_SCALING_1X)     //如果输出电流放大一倍
		   vol=(2.5-((float)pTestPara->test_Vcurrent/1000*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
	   else                                                        //如果输出电压放大六倍
		   vol=(2.5-((float)pTestPara->test_Vcurrent/1000*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
		 RangeSetting(&(*pRelay));	                                 //处理FVMI模式下档位设置是定档还是自动换挡
		 //Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_5D6);         //FVMI模式下电流按照最小缩放倍数采集，换挡用挡位电阻控制
	}
	AD5689R_WriteIR(&hAD5689R1, CH_A, vol);	                       //固定输出电压
	HAL_GPIO_WritePin(Ctrl15_GPIO_Port, Ctrl15_Pin , GPIO_PIN_SET);//FORCE_H端接通测量档位
}

void ClearSignOutVol(Relay_TypeDef* pRelay)
{
	float vol=0;
	if(pRelay->relay_input_scaling==RELAY_INPUT_SCALING_1X)    //如果输出电压放大一倍
		vol=(2.5-((float)0*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
	else                                                       //如果输出电压放大六倍
		vol=(2.5-((float)0*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
	AD5689R_WriteIR(&hAD5689R1, CH_A, vol);	                   //清空输出电压
	HAL_GPIO_WritePin(Ctrl15_GPIO_Port, Ctrl15_Pin , GPIO_PIN_RESET);//FORCE_H端接地
}

void Test_Relay(void)         //测试当前档位合适
{
	for(uint8_t i=0;i<4;i++)
	{
		AD7988_1_ReadData(&hAD7988_1_1);
		HAL_Delay(1);
		AD7988_1_ReadData(&hAD7988_1_2);	
	}
}


