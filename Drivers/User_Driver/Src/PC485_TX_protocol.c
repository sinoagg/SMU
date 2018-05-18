#include "PC485_TX_protocol.h"


void PC485_TX_Message(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, uint8_t EndOfTest)
{
	pTestResult->test_result_V.number_float*=1000;                   //电压转为mV进行传输
	if(pTestPara->test_mode==MODE_FVMI_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
	{
		UART2_TxBuf[2]=pTestResult->test_result_I.number_uchar[3];     //采集电流
		UART2_TxBuf[3]=pTestResult->test_result_I.number_uchar[2];
		UART2_TxBuf[4]=pTestResult->test_result_I.number_uchar[1];
		UART2_TxBuf[5]=pTestResult->test_result_I.number_uchar[0];
		UART2_TxBuf[7]=pTestResult->test_result_V.number_uchar[3];     //监控电压
		UART2_TxBuf[8]=pTestResult->test_result_V.number_uchar[2];	
		UART2_TxBuf[9]=pTestResult->test_result_V.number_uchar[1];																		
		UART2_TxBuf[10]=pTestResult->test_result_V.number_uchar[0];																		
	}
	else if(pTestPara->test_mode==MODE_FIMV_SWEEP||pTestPara->test_mode==MODE_FIMV_NO_SWEEP)
	{
		UART2_TxBuf[2]=pTestResult->test_result_V.number_uchar[3];     //采集电压
		UART2_TxBuf[3]=pTestResult->test_result_V.number_uchar[2];
		UART2_TxBuf[4]=pTestResult->test_result_V.number_uchar[1];
		UART2_TxBuf[5]=pTestResult->test_result_V.number_uchar[0];
		UART2_TxBuf[7]=pTestResult->test_result_I.number_uchar[3];     //监控电流
		UART2_TxBuf[8]=pTestResult->test_result_I.number_uchar[2];	
		UART2_TxBuf[9]=pTestResult->test_result_I.number_uchar[1];																		
		UART2_TxBuf[10]=pTestResult->test_result_I.number_uchar[0];		
	}
	UART2_TxBuf[0]=EndOfTest;																							//数据是否为最后一组数据	
	UART2_TxBuf[1]=EndOfTest;																							//数据是否为最后一组数据																
	UART2_TxBuf[6]=Relay.relay_range;
	
	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET); //使能串口2发送
	HAL_UART_AbortReceive_IT(&huart1);
	if(HAL_UART_Transmit(&huart2, UART2_TxBuf, 11, 1000)!=HAL_OK)
	{
		Error_Handler();
	}		
	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);//使能接收
}

void PC485_Respond(void)
{
	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET); //使能串口2发送
	HAL_UART_AbortReceive_IT(&huart1);
	UART2_TxBuf[0]=0xF1;
	UART2_TxBuf[1]=0xF1;
	if(HAL_UART_Transmit(&huart2, UART2_TxBuf, 11, 1000)!=HAL_OK)         //ACK应答
	{
		Error_Handler();
	}
	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);//使能接收
}

void PC485_Update(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult)
{
	if(pTestPara->test_mode==MODE_FVMI_NO_SWEEP||pTestPara->test_mode==MODE_FIMV_NO_SWEEP) // I-t R-t V-t 模式
	{ 
		PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);		          //PC485上传数据
	}
	else if(pTestPara->test_mode==MODE_FVMI_SWEEP)                        //I-V
	{
		if(pTestPara->test_Vstart<=pTestPara->test_Vend)                    //起始电压值小于结束电压是正扫
		{
			if(pTestPara->test_Vcurrent<=pTestPara->test_Vend)                //如果当前电压小于结束值       
			{
				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //传递未结束的标志和电压电流值
				pTestPara->test_Vcurrent+=pTestPara->test_step;						      //设置下一次输出电压
				if(pTestPara->test_Vcurrent>pTestPara->test_Vend)								//扫描结束																												
				{
					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC输出数据中断关闭		
					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC采集中断关闭
					HAL_Delay(100);
					pTestPara->test_status=OFF;
					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //结束测试,发送最后一组F3开头的命令
					ClearSignOutVol(&Relay);																			//输出电压清零和断开输出
				}
			}
		}
		else                                                                //起始电压值大于结束电压是反扫
		{
			if(pTestPara->test_Vcurrent>=pTestPara->test_Vend)                //如果当前电压值大于结束电压
			{
				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //传递未结束的标志和电压电流值
				pTestPara->test_Vcurrent-=pTestPara->test_step;						      //设置下一次输出电压
				if(pTestPara->test_Vcurrent<pTestPara->test_Vend)		            //扫描结束																										//扫描结束																												
				{
					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC输出数据中断关闭		
					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC采集中断关闭
					HAL_Delay(100);
					pTestPara->test_status=OFF;
					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //结束测试,发送最后一组F3开头的命令
					ClearSignOutVol(&Relay);																			//输出电压清零和断开输出
				}
			}
		}
	}	
	else if(pTestPara->test_mode==MODE_FIMV_SWEEP)                        //V-I
	{
		if(pTestPara->test_Istart<=pTestPara->test_Iend)                    //起始电流值小于结束电流是正扫
		{
			if(pTestPara->test_Icurrent<=pTestPara->test_Iend)                //如果当前电流小于结束值       
			{
				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //传递未结束的标志和电压电流值
				pTestPara->test_Icurrent+=pTestPara->test_step;						      //设置下一次输出电流
				if(pTestPara->test_Icurrent>pTestPara->test_Iend)								//扫描结束																												
				{
					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC输出数据中断关闭		
					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC采集中断关闭
					HAL_Delay(100);
					pTestPara->test_status=OFF;
					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //结束测试,发送最后一组F3开头的命令
					ClearSignOutVol(&Relay);																			//输出电压清零和断开输出
				}
			}
		}
		else                                                                //起始电流值大于结束电流值是反扫
		{
			if(pTestPara->test_Icurrent>=pTestPara->test_Iend)                //如果当前电流值大于结束电流
			{
				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //传递未结束的标志和电压电流值
				pTestPara->test_Icurrent-=pTestPara->test_step;						      //设置下一次输出电流
				if(pTestPara->test_Icurrent<pTestPara->test_Iend)		            //扫描结束																										//扫描结束																												
				{
					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC输出数据中断关闭		
					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC采集中断关闭
					HAL_Delay(100);
					pTestPara->test_status=OFF;
					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //结束测试,发送最后一组F3开头的命令
					ClearSignOutVol(&Relay);																			//输出电压清零和断开输出
				}
			}
		}
	}
}



