/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define Ctrl1_Pin GPIO_PIN_0
#define Ctrl1_GPIO_Port GPIOC
#define Ctrl2_Pin GPIO_PIN_1
#define Ctrl2_GPIO_Port GPIOC
#define Ctrl3_Pin GPIO_PIN_2
#define Ctrl3_GPIO_Port GPIOC
#define Ctrl4_Pin GPIO_PIN_3
#define Ctrl4_GPIO_Port GPIOC
#define DAC1_RESET_Pin GPIO_PIN_0
#define DAC1_RESET_GPIO_Port GPIOA
#define DAC1_SDATA_Pin GPIO_PIN_1
#define DAC1_SDATA_GPIO_Port GPIOA
#define ADC2_SDI_Pin GPIO_PIN_4
#define ADC2_SDI_GPIO_Port GPIOA
#define ADC2_SCK_Pin GPIO_PIN_5
#define ADC2_SCK_GPIO_Port GPIOA
#define ADC2_SDO_Pin GPIO_PIN_6
#define ADC2_SDO_GPIO_Port GPIOA
#define ADC2_CNV_Pin GPIO_PIN_7
#define ADC2_CNV_GPIO_Port GPIOA
#define Ctrl5_Pin GPIO_PIN_4
#define Ctrl5_GPIO_Port GPIOC
#define Ctrl6_Pin GPIO_PIN_5
#define Ctrl6_GPIO_Port GPIOC
#define Ctrl7_Pin GPIO_PIN_0
#define Ctrl7_GPIO_Port GPIOB
#define Ctrl8_Pin GPIO_PIN_1
#define Ctrl8_GPIO_Port GPIOB
#define Ctrl9_Pin GPIO_PIN_2
#define Ctrl9_GPIO_Port GPIOB
#define Ctrl10_Pin GPIO_PIN_12
#define Ctrl10_GPIO_Port GPIOB
#define ADC1_SDI_Pin GPIO_PIN_14
#define ADC1_SDI_GPIO_Port GPIOB
#define ADC1_SCK_Pin GPIO_PIN_15
#define ADC1_SCK_GPIO_Port GPIOB
#define ADC1_SDO_Pin GPIO_PIN_6
#define ADC1_SDO_GPIO_Port GPIOC
#define ADC1_CNV_Pin GPIO_PIN_7
#define ADC1_CNV_GPIO_Port GPIOC
#define RS485_RE2_Pin GPIO_PIN_8
#define RS485_RE2_GPIO_Port GPIOC
#define DAC1_SCLK_Pin GPIO_PIN_9
#define DAC1_SCLK_GPIO_Port GPIOC
#define DAC1_FSYNC_Pin GPIO_PIN_8
#define DAC1_FSYNC_GPIO_Port GPIOA
#define Ctrl14_Pin GPIO_PIN_10
#define Ctrl14_GPIO_Port GPIOC
#define Ctrl13_Pin GPIO_PIN_11
#define Ctrl13_GPIO_Port GPIOC
#define LED0_Pin GPIO_PIN_12
#define LED0_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOD
#define Ctrl11_Pin GPIO_PIN_3
#define Ctrl11_GPIO_Port GPIOB
#define Ctrl12_Pin GPIO_PIN_4
#define Ctrl12_GPIO_Port GPIOB
#define Ctrl15_Pin GPIO_PIN_5
#define Ctrl15_GPIO_Port GPIOB
#define RS485_RE_Pin GPIO_PIN_6
#define RS485_RE_GPIO_Port GPIOB
#define S1_Ctrl_Pin GPIO_PIN_7
#define S1_Ctrl_GPIO_Port GPIOB
#define S2_Ctrl_Pin GPIO_PIN_8
#define S2_Ctrl_GPIO_Port GPIOB
#define S3_Ctrl_Pin GPIO_PIN_9
#define S3_Ctrl_GPIO_Port GPIOB
#define S4_Ctrl_Pin GPIO_PIN_13
#define S4_Ctrl_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
