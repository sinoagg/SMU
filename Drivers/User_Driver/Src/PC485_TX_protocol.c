#include "PC485_TX_protocol.h"


void PC485_TX_Message(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, uint8_t EndOfTest)
{
	pTestResult->test_result_V.number_float*=1000;                   //��ѹתΪmV���д���
	if(pTestPara->test_mode==MODE_FVMI_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
	{
		UART2_TxBuf[2]=pTestResult->test_result_I.number_uchar[3];     //�ɼ�����
		UART2_TxBuf[3]=pTestResult->test_result_I.number_uchar[2];
		UART2_TxBuf[4]=pTestResult->test_result_I.number_uchar[1];
		UART2_TxBuf[5]=pTestResult->test_result_I.number_uchar[0];
		UART2_TxBuf[7]=pTestResult->test_result_V.number_uchar[3];     //��ص�ѹ
		UART2_TxBuf[8]=pTestResult->test_result_V.number_uchar[2];	
		UART2_TxBuf[9]=pTestResult->test_result_V.number_uchar[1];																		
		UART2_TxBuf[10]=pTestResult->test_result_V.number_uchar[0];																		
	}
	else if(pTestPara->test_mode==MODE_FIMV_SWEEP||pTestPara->test_mode==MODE_FIMV_NO_SWEEP)
	{
		UART2_TxBuf[2]=pTestResult->test_result_V.number_uchar[3];     //�ɼ���ѹ
		UART2_TxBuf[3]=pTestResult->test_result_V.number_uchar[2];
		UART2_TxBuf[4]=pTestResult->test_result_V.number_uchar[1];
		UART2_TxBuf[5]=pTestResult->test_result_V.number_uchar[0];
		UART2_TxBuf[7]=pTestResult->test_result_I.number_uchar[3];     //��ص���
		UART2_TxBuf[8]=pTestResult->test_result_I.number_uchar[2];	
		UART2_TxBuf[9]=pTestResult->test_result_I.number_uchar[1];																		
		UART2_TxBuf[10]=pTestResult->test_result_I.number_uchar[0];		
	}
	UART2_TxBuf[0]=EndOfTest;																							//�����Ƿ�Ϊ���һ������	
	UART2_TxBuf[1]=EndOfTest;																							//�����Ƿ�Ϊ���һ������																
	UART2_TxBuf[6]=Relay.relay_range;
	
	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET); //ʹ�ܴ���2����
	HAL_UART_AbortReceive_IT(&huart1);
	if(HAL_UART_Transmit(&huart2, UART2_TxBuf, 11, 1000)!=HAL_OK)
	{
		Error_Handler();
	}		
	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);//ʹ�ܽ���
}

void PC485_Respond(void)
{
	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET); //ʹ�ܴ���2����
	HAL_UART_AbortReceive_IT(&huart1);
	UART2_TxBuf[0]=0xF1;
	UART2_TxBuf[1]=0xF1;
	if(HAL_UART_Transmit(&huart2, UART2_TxBuf, 11, 1000)!=HAL_OK)         //ACKӦ��
	{
		Error_Handler();
	}
	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);//ʹ�ܽ���
}

void PC485_Update(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult)
{
	if(pTestPara->test_mode==MODE_FVMI_NO_SWEEP||pTestPara->test_mode==MODE_FIMV_NO_SWEEP) // I-t R-t V-t ģʽ
	{ 
		PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);		          //PC485�ϴ�����
	}
	else if(pTestPara->test_mode==MODE_FVMI_SWEEP)                        //I-V
	{
		if(pTestPara->test_Vstart<=pTestPara->test_Vend)                    //��ʼ��ѹֵС�ڽ�����ѹ����ɨ
		{
			if(pTestPara->test_Vcurrent<=pTestPara->test_Vend)                //�����ǰ��ѹС�ڽ���ֵ       
			{
				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //����δ�����ı�־�͵�ѹ����ֵ
				pTestPara->test_Vcurrent+=pTestPara->test_step;						      //������һ�������ѹ
				if(pTestPara->test_Vcurrent>pTestPara->test_Vend)								//ɨ�����																												
				{
					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC��������жϹر�		
					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC�ɼ��жϹر�
					HAL_Delay(100);
					pTestPara->test_status=OFF;
					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //��������,�������һ��F3��ͷ������
					ClearSignOutVol(&Relay);																			//�����ѹ����ͶϿ����
				}
			}
		}
		else                                                                //��ʼ��ѹֵ���ڽ�����ѹ�Ƿ�ɨ
		{
			if(pTestPara->test_Vcurrent>=pTestPara->test_Vend)                //�����ǰ��ѹֵ���ڽ�����ѹ
			{
				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //����δ�����ı�־�͵�ѹ����ֵ
				pTestPara->test_Vcurrent-=pTestPara->test_step;						      //������һ�������ѹ
				if(pTestPara->test_Vcurrent<pTestPara->test_Vend)		            //ɨ�����																										//ɨ�����																												
				{
					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC��������жϹر�		
					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC�ɼ��жϹر�
					HAL_Delay(100);
					pTestPara->test_status=OFF;
					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //��������,�������һ��F3��ͷ������
					ClearSignOutVol(&Relay);																			//�����ѹ����ͶϿ����
				}
			}
		}
	}	
	else if(pTestPara->test_mode==MODE_FIMV_SWEEP)                        //V-I
	{
		if(pTestPara->test_Istart<=pTestPara->test_Iend)                    //��ʼ����ֵС�ڽ�����������ɨ
		{
			if(pTestPara->test_Icurrent<=pTestPara->test_Iend)                //�����ǰ����С�ڽ���ֵ       
			{
				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //����δ�����ı�־�͵�ѹ����ֵ
				pTestPara->test_Icurrent+=pTestPara->test_step;						      //������һ���������
				if(pTestPara->test_Icurrent>pTestPara->test_Iend)								//ɨ�����																												
				{
					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC��������жϹر�		
					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC�ɼ��жϹر�
					HAL_Delay(100);
					pTestPara->test_status=OFF;
					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //��������,�������һ��F3��ͷ������
					ClearSignOutVol(&Relay);																			//�����ѹ����ͶϿ����
				}
			}
		}
		else                                                                //��ʼ����ֵ���ڽ�������ֵ�Ƿ�ɨ
		{
			if(pTestPara->test_Icurrent>=pTestPara->test_Iend)                //�����ǰ����ֵ���ڽ�������
			{
				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //����δ�����ı�־�͵�ѹ����ֵ
				pTestPara->test_Icurrent-=pTestPara->test_step;						      //������һ���������
				if(pTestPara->test_Icurrent<pTestPara->test_Iend)		            //ɨ�����																										//ɨ�����																												
				{
					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC��������жϹر�		
					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC�ɼ��жϹر�
					HAL_Delay(100);
					pTestPara->test_status=OFF;
					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //��������,�������һ��F3��ͷ������
					ClearSignOutVol(&Relay);																			//�����ѹ����ͶϿ����
				}
			}
		}
	}
}



