#include "timer_ctrl.h"

void SetTimerPara(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult)
{
	if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)	//�ڶ���ģʽ�£�����ȥ��DAC�жϣ�ֻ����ADC�ж�
	{
		Relay.relay_max_range=RELAY_RANGE_100M;               //����ģʽ�£��Զ��������λΪ10��
	}
	Relay_Set_Test_Mode(Relay.relay_test_mode);         //���ݲ���ģʽ������Ӧ�ļ̵���
	MX_TIM3_Init(pTestPara->test_timestep);							//DAC�����ʱ��,��λms
	HAL_TIM_Base_Stop_IT(&htim3);    										//�ر�DAC����ж�
	MX_TIM2_Init(10000/pTestPara->test_samplerate);			//ADC�ɼ���ʱ������λ100us
	HAL_TIM_Base_Stop_IT(&htim2);     									//�ر�ADC�ɼ�
}

void SetTimerAction(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay)
{
	if(pTestPara->test_cmd==CMD_START)										//��ʼ�ɼ�������				
	{		
		pTestPara->test_status=ON;													//���Կ�ʼ,�в�����������
		SetInitialVoltage(pTestPara,pRelay);
		HAL_Delay(pTestPara->test_silenttime);							//�ȴ���Ĭʱ��
		Test_Relay();																			  //�ȳ䵱��ʱ�ֲ��Ե�λ��Ŀǰֻ�ǲ���ADCֵ
		ChangeTimer(pTestPara, TIM_ON);											//����Ӧ��ʱ��
	}
	else if(pTestPara->test_cmd==CMD_STOP)								//ֹͣ�ɼ�������
	{
		pTestPara->test_status=OFF;					
		ChangeTimer(pTestPara, TIM_OFF);		
		ADCread_status=0;
		pTestResult->ADC_sample_count=0;										//���¼���������
		pTestResult->ADC_sample_sum_I=0;										//�����ۼӣ�����
		pTestResult->ADC_sample_sum_V=0;										//�����ۼӣ�����
		TX_Enable=0;																		    //ֹͣUSB����
		ClearSignOutVol(&Relay);
	}
  
}

void RangeSetting(Relay_TypeDef* pRelay)            //�ж��Ƿ����Զ�����
{
	if(pRelay->relay_change_mode!=AUTO_RANGE)					//��������Զ�����
	{
		 pRelay->relay_range=pRelay->relay_change_mode; //���̶���λģʽ�Ķ�Ӧ��λ��ֵ����λֵ����
		 Set_Relay(pRelay->relay_range);								//���õ�λ���������ʱ�ʲ��ټ���ʱ
	}
}

static void ChangeTimer(TestPara_TypeDef* pTestPara, uint8_t TIM_ONOFF)
{
	if(TIM_ONOFF == TIM_ON)
	{
		if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
		{
			HAL_TIM_Base_Start_IT(&htim3);								//DAC��������ж�����	
			HAL_Delay(2);																	//DAC���֮��̶��ȴ�2ms�ٿ����ɼ�
			HAL_TIM_Base_Start_IT(&htim2);								//ADC�ɼ����ݲ��ش��ж����
		}
	}
	else
	{
		HAL_TIM_Base_Stop_IT(&htim3);										//DAC��������ж�����	
		HAL_Delay(2);																		//DAC���֮��̶��ȴ�2ms�ٿ����ɼ�
		HAL_TIM_Base_Stop_IT(&htim2);										//ADC�ɼ����ݲ��ش��ж�ֹͣ	
	}
}

void SetInitialVoltage(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay)
{
	float vol=0;
	if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP)
	{
	   if(pRelay->relay_input_scaling==RELAY_INPUT_SCALING_1X)     //�����������Ŵ�һ��
		   vol=(2.5-((float)pTestPara->test_Icurrent*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
	   else                                                        //��������ѹ�Ŵ�����
		   vol=(2.5-((float)pTestPara->test_Icurrent*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
		 Set_Relay(Relay.relay_range);                               //���ݵ���ֵ��С��ֵ
	}
	else if(pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
	{
		 if(pRelay->relay_input_scaling==RELAY_INPUT_SCALING_1X)     //�����������Ŵ�һ��
		   vol=(2.5-((float)pTestPara->test_Vcurrent/1000*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
	   else                                                        //��������ѹ�Ŵ�����
		   vol=(2.5-((float)pTestPara->test_Vcurrent/1000*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
		 RangeSetting(&(*pRelay));	                                 //����FVMIģʽ�µ�λ�����Ƕ��������Զ�����
		 //Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_5D6);         //FVMIģʽ�µ���������С���ű����ɼ��������õ�λ�������
	}
	AD5689R_WriteIR(&hAD5689R1, CH_A, vol);	                       //�̶������ѹ
	HAL_GPIO_WritePin(Ctrl15_GPIO_Port, Ctrl15_Pin , GPIO_PIN_SET);//FORCE_H�˽�ͨ������λ
}

void ClearSignOutVol(Relay_TypeDef* pRelay)
{
	float vol=0;
	if(pRelay->relay_input_scaling==RELAY_INPUT_SCALING_1X)    //��������ѹ�Ŵ�һ��
		vol=(2.5-((float)0*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
	else                                                       //��������ѹ�Ŵ�����
		vol=(2.5-((float)0*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
	AD5689R_WriteIR(&hAD5689R1, CH_A, vol);	                   //��������ѹ
	HAL_GPIO_WritePin(Ctrl15_GPIO_Port, Ctrl15_Pin , GPIO_PIN_RESET);//FORCE_H�˽ӵ�
}

void Test_Relay(void)         //���Ե�ǰ��λ����
{
	for(uint8_t i=0;i<4;i++)
	{
		AD7988_1_ReadData(&hAD7988_1_1);
		HAL_Delay(1);
		AD7988_1_ReadData(&hAD7988_1_2);	
	}
}


