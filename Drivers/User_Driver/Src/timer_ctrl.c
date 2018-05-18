#include "timer_ctrl.h"

void SetTimerPara(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult)
{
	if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)	//ÔÚ¶¨µãÄ£Ê½ÏÂ£¬²»ÓÃÈ¥¿ªDACÖĞ¶Ï£¬Ö»ÉèÖÃADCÖĞ¶Ï
	{
		Relay.relay_max_range=RELAY_RANGE_100M;               //¶¨µãÄ£Ê½ÏÂ£¬×Ô¶¯»»µ²×î´óµµÎ»Îª10µµ
	}
	Relay_Set_Test_Mode(Relay.relay_test_mode);         //¸ù¾İ²âÁ¿Ä£Ê½ÉèÖÃÏàÓ¦µÄ¼ÌµçÆ÷
	MX_TIM3_Init(pTestPara->test_timestep);							//DACÊä³ö¶¨Ê±Æ÷,µ¥Î»ms
	HAL_TIM_Base_Stop_IT(&htim3);    										//¹Ø±ÕDACÊä³öÖĞ¶Ï
	MX_TIM2_Init(10000/pTestPara->test_samplerate);			//ADC²É¼¯¶¨Ê±Æ÷£¬µ¥Î»100us
	HAL_TIM_Base_Stop_IT(&htim2);     									//¹Ø±ÕADC²É¼¯
}

void SetTimerAction(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay)
{
	if(pTestPara->test_cmd==CMD_START)										//¿ªÊ¼²É¼¯µÄÃüÁî				
	{		
		pTestPara->test_status=ON;													//²âÊÔ¿ªÊ¼,ÓĞ²âÊÔÔÚÔËĞĞÖĞ
		SetInitialVoltage(pTestPara,pRelay);
		HAL_Delay(pTestPara->test_silenttime);							//µÈ´ı¾²Ä¬Ê±¼ä
		Test_Relay();																			  //¼È³äµ±ÑÓÊ±ÓÖ²âÊÔµµÎ»£¬Ä¿Ç°Ö»ÊÇ²âÊÔADCÖµ
		ChangeTimer(pTestPara, TIM_ON);											//´ò¿ªÏàÓ¦¶¨Ê±Æ÷
	}
	else if(pTestPara->test_cmd==CMD_STOP)								//Í£Ö¹²É¼¯µÄÃüÁî
	{
		pTestPara->test_status=OFF;					
		ChangeTimer(pTestPara, TIM_OFF);		
		ADCread_status=0;
		pTestResult->ADC_sample_count=0;										//ÖØĞÂ¼ÆÊı£¬ÇåÁã
		pTestResult->ADC_sample_sum_I=0;										//ÖØĞÂÀÛ¼Ó£¬ÇåÁã
		pTestResult->ADC_sample_sum_V=0;										//ÖØĞÂÀÛ¼Ó£¬ÇåÁã
		TX_Enable=0;																		    //Í£Ö¹USB´«Êä
		ClearSignOutVol(&Relay);
	}
  
}

void RangeSetting(Relay_TypeDef* pRelay)            //ÅĞ¶ÏÊÇ·ñÊÇ×Ô¶¯»»µ²
{
	if(pRelay->relay_change_mode!=AUTO_RANGE)					//Èç¹û²»ÊÇ×Ô¶¯»»µ²
	{
		 pRelay->relay_range=pRelay->relay_change_mode; //½«¹Ì¶¨µµÎ»Ä£Ê½µÄ¶ÔÓ¦µµÎ»¸³Öµ¸øµµÎ»Öµ±äÁ¿
		 Set_Relay(pRelay->relay_range);								//ÉèÖÃµµÎ»£¬ºó±ßÓĞÑÓÊ±¹Ê²»ÔÙ¼ÓÑÓÊ±
	}
}

static void ChangeTimer(TestPara_TypeDef* pTestPara, uint8_t TIM_ONOFF)
{
	if(TIM_ONOFF == TIM_ON)
	{
		if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
		{
			HAL_TIM_Base_Start_IT(&htim3);								//DACÊä³öÊı¾İÖĞ¶ÏÆô¶¯	
			HAL_Delay(2);																	//DACÊä³öÖ®ºó¹Ì¶¨µÈ´ı2msÔÙ¿ªÆô²É¼¯
			HAL_TIM_Base_Start_IT(&htim2);								//ADC²É¼¯Êı¾İ²¢»Ø´«ÖĞ¶ÏÆô¶
		}
	}
	else
	{
		HAL_TIM_Base_Stop_IT(&htim3);										//DACÊä³öÊı¾İÖĞ¶ÏÆô¶¯	
		HAL_Delay(2);																		//DACÊä³öÖ®ºó¹Ì¶¨µÈ´ı2msÔÙ¿ªÆô²É¼¯
		HAL_TIM_Base_Stop_IT(&htim2);										//ADC²É¼¯Êı¾İ²¢»Ø´«ÖĞ¶ÏÍ£Ö¹	
	}
}

void SetInitialVoltage(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay)
{
	float vol=0;
	if(pTestPara->test_mode==MODE_FIMV_NO_SWEEP)
	{
	   if(pRelay->relay_input_scaling==RELAY_INPUT_SCALING_1X)     //Èç¹ûÊä³öµçÁ÷·Å´óÒ»±¶
		   vol=(2.5-((float)pTestPara->test_Icurrent*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
	   else                                                        //Èç¹ûÊä³öµçÑ¹·Å´óÁù±¶
		   vol=(2.5-((float)pTestPara->test_Icurrent*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
		 Set_Relay(Relay.relay_range);                               //¸ù¾İµçÁ÷Öµ´óĞ¡¸³Öµ
	}
	else if(pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
	{
		 if(pRelay->relay_input_scaling==RELAY_INPUT_SCALING_1X)     //Èç¹ûÊä³öµçÁ÷·Å´óÒ»±¶
		   vol=(2.5-((float)pTestPara->test_Vcurrent/1000*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
	   else                                                        //Èç¹ûÊä³öµçÑ¹·Å´óÁù±¶
		   vol=(2.5-((float)pTestPara->test_Vcurrent/1000*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
		 RangeSetting(&(*pRelay));	                                 //´¦ÀíFVMIÄ£Ê½ÏÂµµÎ»ÉèÖÃÊÇ¶¨µµ»¹ÊÇ×Ô¶¯»»µ²
		 //Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_5D6);         //FVMIÄ£Ê½ÏÂµçÁ÷°´ÕÕ×îĞ¡Ëõ·Å±¶Êı²É¼¯£¬»»µ²ÓÃµ²Î»µç×è¿ØÖÆ
	}
	AD5689R_WriteIR(&hAD5689R1, CH_A, vol);	                       //¹Ì¶¨Êä³öµçÑ¹
	HAL_GPIO_WritePin(Ctrl15_GPIO_Port, Ctrl15_Pin , GPIO_PIN_SET);//FORCE_H¶Ë½ÓÍ¨²âÁ¿µµÎ»
}

void ClearSignOutVol(Relay_TypeDef* pRelay)
{
	float vol=0;
	if(pRelay->relay_input_scaling==RELAY_INPUT_SCALING_1X)    //Èç¹ûÊä³öµçÑ¹·Å´óÒ»±¶
		vol=(2.5-((float)0*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
	else                                                       //Èç¹ûÊä³öµçÑ¹·Å´óÁù±¶
		vol=(2.5-((float)0*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
	AD5689R_WriteIR(&hAD5689R1, CH_A, vol);	                   //Çå¿ÕÊä³öµçÑ¹
	HAL_GPIO_WritePin(Ctrl15_GPIO_Port, Ctrl15_Pin , GPIO_PIN_RESET);//FORCE_H¶Ë½ÓµØ
}

void Test_Relay(void)         //²âÊÔµ±Ç°µµÎ»ºÏÊÊ
{
	for(uint8_t i=0;i<4;i++)
	{
		AD7988_1_ReadData(&hAD7988_1_1);
		HAL_Delay(1);
		AD7988_1_ReadData(&hAD7988_1_2);	
	}
}


