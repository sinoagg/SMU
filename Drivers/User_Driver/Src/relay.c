#include "relay.h"

static void Relay_Set_Input_Scaling(uint8_t scale) //设置DAC输出电压放大倍数
{
	if(scale==RELAY_INPUT_SCALING_1X)
	{
		HAL_GPIO_WritePin(Ctrl11_GPIO_Port, Ctrl11_Pin , GPIO_PIN_RESET); 
	}
	else
	{
		HAL_GPIO_WritePin(Ctrl11_GPIO_Port, Ctrl11_Pin , GPIO_PIN_SET); 
	}
}

void Relay_Set_Test_Mode(uint8_t testmode) //设置测量模式恒压或者恒流
{
	if(testmode==RELAY_TEST_MODE_FVMI)
	{
		HAL_GPIO_WritePin(Ctrl12_GPIO_Port, Ctrl12_Pin , GPIO_PIN_RESET); 
	}
	else
	{
		HAL_GPIO_WritePin(Ctrl12_GPIO_Port, Ctrl12_Pin , GPIO_PIN_SET); 
	}
}

//void Relay_ADC_Voltage_Scaling(uint8_t scale)  //设置ADC2采集电压缩小倍数
//{
//	if(scale==RELAY_INPUT_SCALING_5D6)
//	{
//		HAL_GPIO_WritePin(Ctrl14_GPIO_Port, Ctrl14_Pin , GPIO_PIN_SET); 
//	}
//	else
//	{
//		HAL_GPIO_WritePin(Ctrl14_GPIO_Port, Ctrl14_Pin , GPIO_PIN_RESET);
//	}
//}

//void Relay_ADC_Current_Scaling(uint8_t scale)  //设置ADC1采集电流缩小倍数
//{
//	if(scale==RELAY_INPUT_SCALING_5D6)
//	{
//		HAL_GPIO_WritePin(Ctrl13_GPIO_Port, Ctrl13_Pin , GPIO_PIN_SET); 
//	}
//	else
//	{
//		HAL_GPIO_WritePin(Ctrl13_GPIO_Port, Ctrl13_Pin , GPIO_PIN_RESET);
//	}
//}
	
void Relay_Init(Relay_TypeDef *pRelay)
{
	Relay_Clear();
	Relay_Set_Input_Scaling(pRelay->relay_input_scaling);			//设置输入放大
	Relay_Set_Test_Mode(pRelay->relay_test_mode);							//设置FVMI或FIMV模式
	Set_Relay(pRelay->relay_range);														//设置当前挡位
//	Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_5D6);       //设置采集电压为原先的六分之五
//	Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_5D6);       //设置采集电流为原先的六分之五
}

void Relay_Clear(void)   //初始化10个继电器控制端
{
	/*采集源极电流控制继电器*/
	HAL_GPIO_WritePin(Ctrl1_GPIO_Port, Ctrl1_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl2_GPIO_Port, Ctrl2_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl3_GPIO_Port, Ctrl3_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl4_GPIO_Port, Ctrl4_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl5_GPIO_Port, Ctrl5_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl6_GPIO_Port, Ctrl6_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl7_GPIO_Port, Ctrl7_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl8_GPIO_Port, Ctrl8_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl9_GPIO_Port, Ctrl9_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl10_GPIO_Port, Ctrl10_Pin , GPIO_PIN_RESET); 
}

void Set_Relay(uint8_t range_select)                                   //FVMI设置采集电压放大范围，FIMV设置电流档位
{
	Relay_Clear();
	switch(range_select)
	{
		case 1:																															//1R档位，放大1倍  
			HAL_GPIO_WritePin(Ctrl1_GPIO_Port, Ctrl1_Pin , GPIO_PIN_SET);
			break;
		case 2:                                                             //10R档位，放大10倍
			HAL_GPIO_WritePin(Ctrl2_GPIO_Port, Ctrl2_Pin , GPIO_PIN_SET);
			break;
		case 3:                                                             //100R档位，放大10^2倍
			HAL_GPIO_WritePin(Ctrl3_GPIO_Port, Ctrl3_Pin , GPIO_PIN_SET);
			break;
		case 4:                                                             //1K档位，放大10^3倍
			HAL_GPIO_WritePin(Ctrl4_GPIO_Port, Ctrl4_Pin , GPIO_PIN_SET);
			break;
		case 5:                                                             //10K档位，放大10^4倍
			HAL_GPIO_WritePin(Ctrl5_GPIO_Port, Ctrl5_Pin , GPIO_PIN_SET);
			break;
		case 6:                                                             //100K档位，放大10^5倍
			HAL_GPIO_WritePin(Ctrl6_GPIO_Port, Ctrl6_Pin , GPIO_PIN_SET);
			break;
		case 7:                                                             //1M档位，放大10^6倍
		  HAL_GPIO_WritePin(Ctrl7_GPIO_Port, Ctrl7_Pin , GPIO_PIN_SET);
			break;
		case 8:                                                             //10M档位，放大10^7倍
			HAL_GPIO_WritePin(Ctrl8_GPIO_Port, Ctrl8_Pin , GPIO_PIN_SET);
			break;
		case 9:                                                             //100M档位，放大10^8倍
			HAL_GPIO_WritePin(Ctrl9_GPIO_Port, Ctrl9_Pin , GPIO_PIN_SET);
			break;
		case 10:                                                            //1G档位，放大10^9倍
			HAL_GPIO_WritePin(Ctrl10_GPIO_Port, Ctrl10_Pin , GPIO_PIN_SET);
			break;
		default:
			break;
	}
	
}

/******************************************************************/
/*检查测量档位是否正确												                    */
/*返回值： 0 为正确，1为换档																			*/
/******************************************************************/
uint8_t Relay_Check_Measure(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pResultPara, MonitorCH_TypeDef *phMonitorCH, uint32_t origin_adcvalue_I, uint32_t origin_adcvalue_V)
{
	if((pTestPara->test_mode==MODE_FVMI_NO_SWEEP)&&(Relay.relay_change_mode==AUTO_RANGE)) //如果为FVMI自动换挡模式
	{
		float R_voltage;                                                     //挡位电阻两端电压
		R_voltage=(origin_adcvalue_I*2.5/65535-1.25)*12;
		if(Relay.origin_state==0) Relay.origin_state=1;
		else Relay.origin_state=0;
		if((R_voltage>(12-pTestPara->test_Vcurrent/1000))||(R_voltage<(-12-pTestPara->test_Vcurrent/1000)))  			//如果采集到的电流过大
		{
			if(Relay.relay_range>1)																		       //如果档位在1档以上，仍然可以降档
			{		
				Relay.relay_range--;
				if(Relay.relay_range==Relay.origin_range)                      //如果不停换挡则保留较大档位
				{
					Relay.relay_change=RELAY_CHANGE_OVER;                 			 //换挡结束
					Relay.relay_test_mode=FIXED_RANGE;                           //固定档位
				}
				else
				{
					Relay.relay_change=RELAY_CHANGE_CONTINUE;                     //换挡继续
				}
				Set_Relay(Relay.relay_range);
				pResultPara->ADC_sample_count=0;																//换挡后需要重新计数
				pResultPara->ADC_sample_sum_I=0;																//换挡后需要重新计算
				HAL_Delay(RANGE_CHANGE_DELAY);
				return 1;
			}
			else																															//如果档位在1档，无法降档，换挡结束
				return 0;
		}
		else if((origin_adcvalue_I<0x8800)&&(origin_adcvalue_I>0x78FF))			//如果采集到的电流过小
		{
			if(Relay.relay_range<Relay.relay_max_range)	                      //如果档位在7档以下，仍然可以升档
			{
				Relay.relay_range++;
				if(Relay.relay_range==Relay.origin_range)
				{
					Relay.relay_range--;                                          //保留较大档位
					Relay.relay_change=RELAY_CHANGE_OVER;                 				//换挡结束
					Relay.relay_test_mode=FIXED_RANGE;                            //固定档位
				}
				else
				{
					Relay.relay_change=RELAY_CHANGE_CONTINUE;                     //换挡继续
				}
				Set_Relay(Relay.relay_range);
				pResultPara->ADC_sample_count=0;																//换挡后需要重新计数
				pResultPara->ADC_sample_sum_I=0;																//换挡后需要重新计算
				HAL_Delay(RANGE_CHANGE_DELAY);						
				return 1;
			}
			else																															//如果档位在7档，无法升档，换挡结束
				return 0;
		}
		else 
			return 0;
	}
	else if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP)                      //如果为FIMV模式
	{
//		if((origin_adcvalue_V>0xFD00)||(origin_adcvalue_V<0x300))  			   //如果采集到的电流过大
//		{
//			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_1D12;
//			Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_1D12);
//			pResultPara->ADC_sample_count=0;																//换挡后需要重新计数
//			pResultPara->ADC_sample_sum_V=0;																//换挡后需要重新计算
//			HAL_Delay(RANGE_CHANGE_DELAY);			
//			return 1;
//		}
//		else if((origin_adcvalue_V<0x8800)&&(origin_adcvalue_V>0x78FF))			//如果采集到的电流过小
//		{
//			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_5D6;
//			Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_5D6);
//			pResultPara->ADC_sample_count=0;																//换挡后需要重新计数
//			pResultPara->ADC_sample_sum_V=0;																//换挡后需要重新计算
//			HAL_Delay(RANGE_CHANGE_DELAY);			
//			return 1;
//		}
//		else
			return 0;
	}
	else                                            											 //如果为FVMI的固定档位模式
	{
		return 0;
	}
}

/******************************************************************/
/*检查监控档位是否正确					                               		*/
/*返回值：origin_adcvalue																	        */
/******************************************************************/
float Relay_Check_Monitor(MonitorCH_TypeDef *phMonitorCH,	uint8_t monitor_mode)
{
	float origin_adcvalue;
	origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);	 //ADC电压采集数据	
//	if((origin_adcvalue>0xFD00)||(origin_adcvalue<0x300))  			   //如果采集到的电流过大
//	{
//			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_1D12;
//			if(monitor_mode==RELAY_TEST_MODE_FVMI)
//			  Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_1D12);
//			else if(monitor_mode==RELAY_TEST_MODE_FIMV)
//				Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_1D12);
//			HAL_Delay(20);			
//			origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);
//	}
//	else if((origin_adcvalue<0x8800)&&(origin_adcvalue>0x78FF))			//如果采集到的电流过小
//	{
//			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_5D6;
//			if(monitor_mode==RELAY_TEST_MODE_FVMI)
//				Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_5D6);
//			else if(monitor_mode==RELAY_TEST_MODE_FIMV)
//				Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_5D6);
//			HAL_Delay(20);	
//			origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);
//		}
	return origin_adcvalue;
}


