#include "relay.h"

static void Relay_Set_Input_Scaling(uint8_t scale) //����DAC�����ѹ�Ŵ���
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

void Relay_Set_Test_Mode(uint8_t testmode) //���ò���ģʽ��ѹ���ߺ���
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

//void Relay_ADC_Voltage_Scaling(uint8_t scale)  //����ADC2�ɼ���ѹ��С����
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

//void Relay_ADC_Current_Scaling(uint8_t scale)  //����ADC1�ɼ�������С����
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
	Relay_Set_Input_Scaling(pRelay->relay_input_scaling);			//��������Ŵ�
	Relay_Set_Test_Mode(pRelay->relay_test_mode);							//����FVMI��FIMVģʽ
	Set_Relay(pRelay->relay_range);														//���õ�ǰ��λ
//	Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_5D6);       //���òɼ���ѹΪԭ�ȵ�����֮��
//	Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_5D6);       //���òɼ�����Ϊԭ�ȵ�����֮��
}

void Relay_Clear(void)   //��ʼ��10���̵������ƶ�
{
	/*�ɼ�Դ���������Ƽ̵���*/
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

void Set_Relay(uint8_t range_select)                                   //FVMI���òɼ���ѹ�Ŵ�Χ��FIMV���õ�����λ
{
	Relay_Clear();
	switch(range_select)
	{
		case 1:																															//1R��λ���Ŵ�1��  
			HAL_GPIO_WritePin(Ctrl1_GPIO_Port, Ctrl1_Pin , GPIO_PIN_SET);
			break;
		case 2:                                                             //10R��λ���Ŵ�10��
			HAL_GPIO_WritePin(Ctrl2_GPIO_Port, Ctrl2_Pin , GPIO_PIN_SET);
			break;
		case 3:                                                             //100R��λ���Ŵ�10^2��
			HAL_GPIO_WritePin(Ctrl3_GPIO_Port, Ctrl3_Pin , GPIO_PIN_SET);
			break;
		case 4:                                                             //1K��λ���Ŵ�10^3��
			HAL_GPIO_WritePin(Ctrl4_GPIO_Port, Ctrl4_Pin , GPIO_PIN_SET);
			break;
		case 5:                                                             //10K��λ���Ŵ�10^4��
			HAL_GPIO_WritePin(Ctrl5_GPIO_Port, Ctrl5_Pin , GPIO_PIN_SET);
			break;
		case 6:                                                             //100K��λ���Ŵ�10^5��
			HAL_GPIO_WritePin(Ctrl6_GPIO_Port, Ctrl6_Pin , GPIO_PIN_SET);
			break;
		case 7:                                                             //1M��λ���Ŵ�10^6��
		  HAL_GPIO_WritePin(Ctrl7_GPIO_Port, Ctrl7_Pin , GPIO_PIN_SET);
			break;
		case 8:                                                             //10M��λ���Ŵ�10^7��
			HAL_GPIO_WritePin(Ctrl8_GPIO_Port, Ctrl8_Pin , GPIO_PIN_SET);
			break;
		case 9:                                                             //100M��λ���Ŵ�10^8��
			HAL_GPIO_WritePin(Ctrl9_GPIO_Port, Ctrl9_Pin , GPIO_PIN_SET);
			break;
		case 10:                                                            //1G��λ���Ŵ�10^9��
			HAL_GPIO_WritePin(Ctrl10_GPIO_Port, Ctrl10_Pin , GPIO_PIN_SET);
			break;
		default:
			break;
	}
	
}

/******************************************************************/
/*��������λ�Ƿ���ȷ												                    */
/*����ֵ�� 0 Ϊ��ȷ��1Ϊ����																			*/
/******************************************************************/
uint8_t Relay_Check_Measure(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pResultPara, MonitorCH_TypeDef *phMonitorCH, uint32_t origin_adcvalue_I, uint32_t origin_adcvalue_V)
{
	if((pTestPara->test_mode==MODE_FVMI_NO_SWEEP)&&(Relay.relay_change_mode==AUTO_RANGE)) //���ΪFVMI�Զ�����ģʽ
	{
		float R_voltage;                                                     //��λ�������˵�ѹ
		R_voltage=(origin_adcvalue_I*2.5/65535-1.25)*12;
		if(Relay.origin_state==0) Relay.origin_state=1;
		else Relay.origin_state=0;
		if((R_voltage>(12-pTestPara->test_Vcurrent/1000))||(R_voltage<(-12-pTestPara->test_Vcurrent/1000)))  			//����ɼ����ĵ�������
		{
			if(Relay.relay_range>1)																		       //�����λ��1�����ϣ���Ȼ���Խ���
			{		
				Relay.relay_range--;
				if(Relay.relay_range==Relay.origin_range)                      //�����ͣ���������ϴ�λ
				{
					Relay.relay_change=RELAY_CHANGE_OVER;                 			 //��������
					Relay.relay_test_mode=FIXED_RANGE;                           //�̶���λ
				}
				else
				{
					Relay.relay_change=RELAY_CHANGE_CONTINUE;                     //��������
				}
				Set_Relay(Relay.relay_range);
				pResultPara->ADC_sample_count=0;																//��������Ҫ���¼���
				pResultPara->ADC_sample_sum_I=0;																//��������Ҫ���¼���
				HAL_Delay(RANGE_CHANGE_DELAY);
				return 1;
			}
			else																															//�����λ��1�����޷���������������
				return 0;
		}
		else if((origin_adcvalue_I<0x8800)&&(origin_adcvalue_I>0x78FF))			//����ɼ����ĵ�����С
		{
			if(Relay.relay_range<Relay.relay_max_range)	                      //�����λ��7�����£���Ȼ��������
			{
				Relay.relay_range++;
				if(Relay.relay_range==Relay.origin_range)
				{
					Relay.relay_range--;                                          //�����ϴ�λ
					Relay.relay_change=RELAY_CHANGE_OVER;                 				//��������
					Relay.relay_test_mode=FIXED_RANGE;                            //�̶���λ
				}
				else
				{
					Relay.relay_change=RELAY_CHANGE_CONTINUE;                     //��������
				}
				Set_Relay(Relay.relay_range);
				pResultPara->ADC_sample_count=0;																//��������Ҫ���¼���
				pResultPara->ADC_sample_sum_I=0;																//��������Ҫ���¼���
				HAL_Delay(RANGE_CHANGE_DELAY);						
				return 1;
			}
			else																															//�����λ��7�����޷���������������
				return 0;
		}
		else 
			return 0;
	}
	else if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP)                      //���ΪFIMVģʽ
	{
//		if((origin_adcvalue_V>0xFD00)||(origin_adcvalue_V<0x300))  			   //����ɼ����ĵ�������
//		{
//			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_1D12;
//			Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_1D12);
//			pResultPara->ADC_sample_count=0;																//��������Ҫ���¼���
//			pResultPara->ADC_sample_sum_V=0;																//��������Ҫ���¼���
//			HAL_Delay(RANGE_CHANGE_DELAY);			
//			return 1;
//		}
//		else if((origin_adcvalue_V<0x8800)&&(origin_adcvalue_V>0x78FF))			//����ɼ����ĵ�����С
//		{
//			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_5D6;
//			Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_5D6);
//			pResultPara->ADC_sample_count=0;																//��������Ҫ���¼���
//			pResultPara->ADC_sample_sum_V=0;																//��������Ҫ���¼���
//			HAL_Delay(RANGE_CHANGE_DELAY);			
//			return 1;
//		}
//		else
			return 0;
	}
	else                                            											 //���ΪFVMI�Ĺ̶���λģʽ
	{
		return 0;
	}
}

/******************************************************************/
/*����ص�λ�Ƿ���ȷ					                               		*/
/*����ֵ��origin_adcvalue																	        */
/******************************************************************/
float Relay_Check_Monitor(MonitorCH_TypeDef *phMonitorCH,	uint8_t monitor_mode)
{
	float origin_adcvalue;
	origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);	 //ADC��ѹ�ɼ�����	
//	if((origin_adcvalue>0xFD00)||(origin_adcvalue<0x300))  			   //����ɼ����ĵ�������
//	{
//			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_1D12;
//			if(monitor_mode==RELAY_TEST_MODE_FVMI)
//			  Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_1D12);
//			else if(monitor_mode==RELAY_TEST_MODE_FIMV)
//				Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_1D12);
//			HAL_Delay(20);			
//			origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);
//	}
//	else if((origin_adcvalue<0x8800)&&(origin_adcvalue>0x78FF))			//����ɼ����ĵ�����С
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


