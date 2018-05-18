#include "hardware.h"


TIM_HandleTypeDef htim2;				     //handle of timer 2
TIM_HandleTypeDef htim3;				     //handle of timer 3

UART_HandleTypeDef huart1;			     //handle of uart 1
UART_HandleTypeDef huart2;			     //handle of uart 2
UART_HandleTypeDef huart3;

AD5689R_HandleTypeDef hAD5689R1;	   //handle of DAC 1

AD7988_1_HandleTypeDef hAD7988_1_1;		//handle of main current monitor ADC 1
AD7988_1_HandleTypeDef hAD7988_1_2;		//handle of main voltage monitor ADC 2
MonitorCH_TypeDef hMonitorCH_Voltage; //handle of monitor voltage
MonitorCH_TypeDef hMonitorCH_Current; //handle of monitor current
MonitorCH_TypeDef hMeasureCH_Voltage; //handle of measure voltage

Relay_TypeDef Relay={DEFAULT_RELAY_INPUT_SCALING,DEFAULT_RELAY_TEST_MODE,DEFAULT_RELAY_CHANGE_MODE,DEFAULT_RELAY_MAX_RANGE,DEFAULT_RELAY_MIN_RANGE,DEFAULT_RELAY_RANGE};

void Hardware_Init(void)
{
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();               //预备接口485
	MX_USART2_UART_Init();               //和上位机软件通信传递采集的值
  MX_USART3_UART_Init();               //屏幕通信

  /* USER CODE BEGIN 2 */
	Delay_Init(72);										    //微秒延时初始化，主频72M
	AD5689R_Init(&hAD5689R1,0);				    //DAC1初始化		
	ClearSignOutVol(&Relay);	            //初始化输出电压为0	
	AD7988_1_Init(&hAD7988_1_1,0);		    //ADC1 for current monitoring 初始化
	AD7988_1_Init(&hAD7988_1_2,1);        //ADC2 for voltage monitoring 初始化
	Relay_Init(&Relay);								    //默认输出放大6倍，模式为恒流，清空测量10个档位，设置默认档位
	HAL_Delay(2000);									    //设置档位后延时2s
	
	hMonitorCH_Voltage.phAD7988_1_x=&hAD7988_1_2;                         //ADC2监控被测器件电压
	hMonitorCH_Current.phAD7988_1_x=&hAD7988_1_1;                         //ADC1监控被测器件电流
	hMeasureCH_Voltage.phAD7988_1_x=&hAD7988_1_2;                         //ADC2测量被测器件电压
	hMonitorCH_Voltage.monitorch_relaystate=0;                            //设置监控端档位
	hMonitorCH_Current.monitorch_relaystate=0;
	hMeasureCH_Voltage.monitorch_relaystate=0;

	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);			       //指令状态指示灯关闭
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);			       //采集状态指示灯关闭
	
	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);	 //使RS485处在接收状态
	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);
	
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Ctrl1_Pin|Ctrl2_Pin|Ctrl3_Pin|Ctrl4_Pin 
                          |Ctrl5_Pin|Ctrl6_Pin|ADC1_CNV_Pin|RS485_RE2_Pin 
                          |Ctrl14_Pin|Ctrl13_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DAC1_RESET_Pin|DAC1_FSYNC_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DAC1_SDATA_Pin|ADC2_SDI_Pin|ADC2_SCK_Pin|ADC2_CNV_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Ctrl7_Pin|Ctrl8_Pin|Ctrl9_Pin|Ctrl10_Pin 
                          |ADC1_SDI_Pin|ADC1_SCK_Pin|Ctrl11_Pin|Ctrl12_Pin 
                          |RS485_RE_Pin|Ctrl15_Pin|S1_Ctrl_Pin|S2_Ctrl_Pin
													|S3_Ctrl_Pin|S4_Ctrl_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DAC1_SCLK_Pin|LED0_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : Ctrl1_Pin Ctrl2_Pin Ctrl3_Pin Ctrl4_Pin 
                           Ctrl5_Pin Ctrl6_Pin ADC1_CNV_Pin RS485_RE2_Pin 
                           DAC1_SCLK_Pin Ctrl14_Pin Ctrl13_Pin */
  GPIO_InitStruct.Pin = Ctrl1_Pin|Ctrl2_Pin|Ctrl3_Pin|Ctrl4_Pin 
                          |Ctrl5_Pin|Ctrl6_Pin|ADC1_CNV_Pin|RS485_RE2_Pin 
                          |DAC1_SCLK_Pin|Ctrl14_Pin|Ctrl13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : DAC1_RESET_Pin DAC1_SDATA_Pin ADC2_SDI_Pin ADC2_SCK_Pin 
                           ADC2_CNV_Pin DAC1_FSYNC_Pin */
  GPIO_InitStruct.Pin = DAC1_RESET_Pin|DAC1_SDATA_Pin|ADC2_SDI_Pin|ADC2_SCK_Pin 
                          |ADC2_CNV_Pin|DAC1_FSYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ADC2_SDO_Pin */
  GPIO_InitStruct.Pin = ADC2_SDO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ADC2_SDO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Ctrl7_Pin Ctrl8_Pin Ctrl9_Pin Ctrl10_Pin 
                           ADC1_SDI_Pin ADC1_SCK_Pin Ctrl11_Pin Ctrl12_Pin 
                           RS485_RE_Pin */
  GPIO_InitStruct.Pin = Ctrl7_Pin|Ctrl8_Pin|Ctrl9_Pin|Ctrl10_Pin 
                          |ADC1_SDI_Pin|ADC1_SCK_Pin|Ctrl11_Pin|Ctrl12_Pin 
                          |RS485_RE_Pin|Ctrl15_Pin|S1_Ctrl_Pin|S2_Ctrl_Pin
													|S3_Ctrl_Pin|S4_Ctrl_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : ADC1_SDO_Pin */
  GPIO_InitStruct.Pin = ADC1_SDO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ADC1_SDO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED0_Pin */
  GPIO_InitStruct.Pin = LED0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

}

/* TIM2 init function */
void MX_TIM2_Init(uint16_t time_step)    //unit 100us 分辨率100us的定时器
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7200-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = time_step-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* TIM3 init function */									//DAC输出中断设置 
void MX_TIM3_Init(uint16_t time_step)			// Uint: ms 分辨率0.1ms
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 28800-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = time_step*2.5-1;					//本来是除以0.4
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USART1 init function */
void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 19200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USART2 init function */
void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USART3 init function */
void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}


