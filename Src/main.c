/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */
#include "para_init.h"
#include "delay.h"
#include "AD5689R.h"
#include "AD7988_1.h"
#include "prj_typedef.h"
#include "InternalFlash.h"
#include "hardware.h"
#include "timer_ctrl.h"
#include "calculation.h"
#include "PC485_TX_protocol.h"

#define SERIALNUM_ADDR	
#define FIRMWARE_ADDR

#define BROADCAST_ADDR 0x00											//485广播地址
#define DEV_ADDR 0x04														//485设备地址
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
SMU_TypeDef hSMU;
TestPara_TypeDef TestPara;                                //测量参数存取
TestResult_TypeDef TestResult;                            //测量结果存取

uint8_t UART1_RxBuf[14]={0};                              //预备485接口
uint8_t UART2_RxBuf[17]={0};                              //接收PC端的设置命令和行动命令
uint8_t UART2_TxBuf[11]={0};                              //向PC发送采集到的数据

enum ADC_Status ADC_status=ADC_BUSY;                      //ADC采集状态
uint16_t origin_adcvalue_I=0;															//用来监控电流的ADC读数，对应7798_1_1
uint16_t origin_adcvalue_V=0;															//用来监控电压的ADC读数, 对应7798_1_2
uint8_t TX_Enable=0;                                      //开始发送标志位

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
 

  /* USER CODE BEGIN 2 */
	Hardware_Init();
	InitTestPara(&TestPara, &TestResult);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */
		
  /* USER CODE BEGIN 3 */
		if(HAL_GPIO_ReadPin(RS485_RE2_GPIO_Port, RS485_RE2_Pin)==GPIO_PIN_RESET)//只在接收状态使能接收
		{
			HAL_UART_Receive_IT(&huart2,UART2_RxBuf,17);													//持续接收PC端485总线消息
		}
		
		if(hSMU.msg_type!=MSG_TYPE_NULL)																			//如果设备收到新的命令
		{
			HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);												//指示灯闪烁
			PC485_Respond();                                                    //PC485应答
			if(hSMU.msg_type==MSG_TYPE_SETTING)																  //如果是设置命令
			{
				if(GetTestPara(&TestPara, MSG_TYPE_SETTING, hSMU.msg_port)==0)		//检查接收并转化测试参数
				{
					SetTimerPara(&TestPara, &TestResult);														//设置定时器参数和选定大小档位
				}		
			}
			else if(hSMU.msg_type==MSG_TYPE_COMMAND)                            //如果是行动命令
			{
				if(GetTestPara(&TestPara, MSG_TYPE_COMMAND, hSMU.msg_port)==0)		//检查接收并转化测试参数
				{
					SetTimerAction(&TestPara, &TestResult, &Relay);	                //定时器动作							
				}
			}
			hSMU.msg_type=MSG_TYPE_NULL;																				//清空消息
		}
		
		if(ADC_status==ADC_READY)																							//如果读取1次ADC过程完成
		{
			ADC_status=ADC_BUSY;
			if(Relay.origin_state==0)
			  Relay.origin_range=Relay.relay_range;                             //获取一次档位信息
			if(Relay_Check_Measure(&TestPara, &TestResult,&hMeasureCH_Voltage, origin_adcvalue_I,origin_adcvalue_V)==0)//检查档位是否正确
		  {
				if(Do_Calculation(&TestPara, &TestResult, origin_adcvalue_I, origin_adcvalue_V)==0)	//得到一次平均结果
				{
					HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);                                     //ADC状态指示灯
					
					if(TestPara.test_mode==MODE_FVMI_NO_SWEEP)
					{
//						if(hMonitorCH_Voltage.monitorch_relaystate==RELAY_INPUT_SCALING_5D6)
//						  TestResult.test_result_V.number_float=(Relay_Check_Monitor(&hMonitorCH_Voltage,RELAY_TEST_MODE_FVMI)*2.5/65535-1.25)*12/10; //获取监控电压
//						else
							TestResult.test_result_V.number_float=(Relay_Check_Monitor(&hMonitorCH_Voltage,RELAY_TEST_MODE_FVMI)*2.5/65535-1.25)*12;              //获取监控电压
							TestResult.test_result_V.number_float=TestResult.test_result_V.number_float*Adj_MeasVLinear.number_float+Adj_MeasVOffset.number_float;// 加入校准参数
					}
					else if(TestPara.test_mode==MODE_FIMV_NO_SWEEP)
					{
//						if(hMonitorCH_Current.monitorch_relaystate==RELAY_INPUT_SCALING_5D6)
//						  TestResult.test_result_I.number_float=(Relay_Check_Monitor(&hMonitorCH_Current,RELAY_TEST_MODE_FIMV)*2.5/65535-1.25)*12/10/MyPow(10,(Relay.relay_range-1)); //获取监控电流
//						else
							TestResult.test_result_I.number_float=(Relay_Check_Monitor(&hMonitorCH_Current,RELAY_TEST_MODE_FIMV)*2.5/65535-1.25)*12/MyPow(10,(Relay.relay_range-1)); //获取监控电流
							TestResult.test_result_I.number_float=TestResult.test_result_I.number_float*Adj_MeasILinear.number_float+Adj_MeasIOffset.number_float;                   // 加入校准参数
					}	
					if(TX_Enable==1)																								                  
					{
						TX_Enable=0;
						PC485_Update(&TestPara, &TestResult);             //更新数据
					}
					
				}
			}
		}
		
	}
  /* USER CODE END 3 */

}
/* USER CODE BEGIN 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
		if (htim->Instance == htim2.Instance)
    {
			if(TestPara.test_mode==MODE_FIMV_NO_SWEEP)
				origin_adcvalue_V=AD7988_1_ReadData(&hAD7988_1_2);										//ADC电压采集数据	
			else if(TestPara.test_mode==MODE_FVMI_NO_SWEEP)
				origin_adcvalue_I=AD7988_1_ReadData(&hAD7988_1_1);										//ADC电流采集数据
				
			ADC_status=ADC_READY;
    }
	  else if (htim->Instance == htim3.Instance)
		{
			float vol=0;
			TX_Enable=1;																													//在任何模式下，定时到达，允许传输
			if(TestPara.test_mode==MODE_FVMI_SWEEP)
			{
				if(Relay.relay_input_scaling==RELAY_INPUT_SCALING_1X)     //如果输出电流放大一倍
		       vol=(2.5-((float)TestPara.test_Vcurrent/1000*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float))/5*65535;
			  else                                                      //如果输出电压放大六倍
		       vol=(2.5-((float)TestPara.test_Vcurrent/1000*Adj_OutputLinear.number_float+Adj_OutputOffset.number_float)/6)/5*65535;
				AD5689R_WriteIR(&hAD5689R1, CH_A, vol);	                       //固定输出电压
			}
			HAL_TIM_Base_Start_IT(&htim2);																				//输出一次才开始AD采集的中断
		}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  
{
	if(huart->Instance == huart2.Instance)																		//485接口收到指令
	{
		if(UART2_RxBuf[0]<0x10)                                                 //如果是设置命令
		{
		   hSMU.msg_type=MSG_TYPE_SETTING;
			 hSMU.msg_port=MSG_PORT_RS485;	
			
		}
		else                                                                    //如果是行动命令                                        
		{
			 hSMU.msg_type=MSG_TYPE_COMMAND;
		}
	}
	
}

/* USER CODE END 4 */
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
