#ifndef __AD7988_1_H
#define __AD7988_1_H	 

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "common_type.h"
#include "main.h"
#include "delay.h"
	 
enum ADC_Status
{
	ADC_READY=0,
	ADC_BUSY=1
};
	
typedef struct 
{
	General_GPIOTypeDef PinSdi;
	General_GPIOTypeDef PinSclk;
	General_GPIOTypeDef PinSdo;
	General_GPIOTypeDef PinCnv;
}AD7988_1_HandleTypeDef;
	 	 
//private functions
static General_GPIOTypeDef AD7988_1_Pin_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

//exported functions
void AD7988_1_Init(AD7988_1_HandleTypeDef *hAD7988_1x, uint8_t ADC_num);
uint16_t AD7988_1_ReadData(AD7988_1_HandleTypeDef *hAD7988_1x);
		    
#endif
