#ifndef __COMMON_TYPE_H
#define __COMMON_TYPE_H	 

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


	 
	 typedef struct
{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
}General_GPIOTypeDef;
	


	
	
		    
#endif

