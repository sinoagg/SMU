#ifndef __AD5689R_H
#define __AD5689R_H	 

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "common_type.h"
#include "main.h"
#include "delay.h"

#define CH_A 1
#define CH_B 0
	 
	
	
typedef struct 
{
	General_GPIOTypeDef PinReset;
	General_GPIOTypeDef PinSclk;
	General_GPIOTypeDef PinSdata;
	General_GPIOTypeDef PinFsync;

}AD5689R_HandleTypeDef;
	 

	
	 
//private functions
static General_GPIOTypeDef AD5689R_Pin_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
static void AD5689R_WriteCmdCode(AD5689R_HandleTypeDef *hAD5689Rx, uint8_t Cmd);
static void AD5689R_ChangeMode(AD5689R_HandleTypeDef *hAD5689Rx);
static void AD5689R_WriteChCode(AD5689R_HandleTypeDef *hAD5689Rx, uint8_t ch);
//exported functions
void AD5689R_Init(AD5689R_HandleTypeDef *hAD5689Rx, int8_t DAC_num);
void AD5689R_WriteIR(AD5689R_HandleTypeDef *hAD5689Rx, uint8_t channel, uint16_t val);
		    
#endif

