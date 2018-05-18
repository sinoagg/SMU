#include "calculation.h"

uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, uint16_t origin_adcvalue_I, uint16_t origin_adcvalue_V)
{
		uint16_t temp_adcvalue;
		float temp_voltage;
		pTestResult->ADC_sample_count++;																								//����������
		if(pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
			pTestResult->ADC_sample_sum_I+=origin_adcvalue_I;															//�ۼӵ�������ֵ
		else if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP)
			pTestResult->ADC_sample_sum_V+=origin_adcvalue_V;															//�ۼӵ�ѹ����ֵ
		if(pTestResult->ADC_sample_count>=pTestPara->test_averagenumber)								 //�������������ƽ��ֵ����
		{
			//��������£�TIM3���ж�ʱ��Ҫ�����ⲿ�ֵĲɼ��ͼ���ʱ��	
			HAL_TIM_Base_Stop_IT(&htim2);																									 //ֹͣAD�ɼ����ж�
			if(pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
			{
				temp_adcvalue=pTestResult->ADC_sample_sum_I/pTestResult->ADC_sample_count;		//�������ƽ��ֵ	
				temp_voltage=(((float)temp_adcvalue)*2.5/65535-1.25)*12/MyPow(10,(Relay.relay_range-1));
				if(temp_voltage>0)                                                            //У׼�����ɼ�����
					pTestResult->test_result_I.number_float=temp_voltage*Adj_PosRelayLinear[Relay.relay_range-1].number_float+Adj_RelayOffset[Relay.relay_range-1].number_float;
				else
					pTestResult->test_result_I.number_float=temp_voltage*Adj_NegRelayLinear[Relay.relay_range-1].number_float+Adj_RelayOffset[Relay.relay_range-1].number_float;
				pTestResult->ADC_sample_sum_I=0;																							//�����ۼӣ�����
			}
			else if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP)
			{
				temp_adcvalue=pTestResult->ADC_sample_sum_V/pTestResult->ADC_sample_count;		//�����ѹƽ��ֵ	
//				if(hMeasureCH_Voltage.monitorch_relaystate==RELAY_INPUT_SCALING_5D6)
//					temp_voltage=(((float)temp_adcvalue)*2.5/65535-1.25)*12/10;
//				else
					temp_voltage=(((float)temp_adcvalue)*2.5/65535-1.25)*12;	
				temp_voltage=temp_voltage*Adj_MeasVLinear.number_float+Adj_MeasVOffset.number_float;// ����У׼����
				pTestResult->test_result_V.number_float=temp_voltage;
				pTestResult->ADC_sample_sum_V=0;																							//�����ۼӣ�����
			}
			pTestResult->ADC_sample_count=0;																							  //���¼���������
	
			if(Relay.relay_change==RELAY_CHANGE_OVER)                                       //��յ�λ������־
			{
				Relay.relay_change=RELAY_CHANGE_CONTINUE;
				Relay.relay_change_mode=AUTO_RANGE;
			}
			return 0;				
		}
		return 1;																						//û����Ч�Ľ��
}


float MyPow(float base,uint8_t time)
{
	uint8_t i;
	float rtn=1;
	for(i=0;i<time;i++)
		rtn*=base;
	return rtn;
}	



