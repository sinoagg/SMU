#ifndef __DELAY_H
#define __DELAY_H	 

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal.h"	
	 
void Delay_Init(uint8_t SYSCLK);	
void delay_us(uint32_t nus);
	 
#endif

