#include "AD7988_1.h"


static General_GPIOTypeDef AD7988_1_Pin_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	General_GPIOTypeDef PinName; 
	PinName.GPIOx=GPIOx;
	PinName.GPIO_Pin=GPIO_Pin;
	return PinName;
}

void AD7988_1_Init(AD7988_1_HandleTypeDef *hAD7988_1x, uint8_t ADC_num)
{
	 if(ADC_num==0)
	 {
		hAD7988_1x->PinSdi = AD7988_1_Pin_Init(ADC1_SDI_GPIO_Port, ADC1_SDI_Pin);
		hAD7988_1x->PinSdo = AD7988_1_Pin_Init(ADC1_SDO_GPIO_Port, ADC1_SDO_Pin);
		hAD7988_1x->PinSclk = AD7988_1_Pin_Init(ADC1_SCK_GPIO_Port, ADC1_SCK_Pin);
		hAD7988_1x->PinCnv = AD7988_1_Pin_Init(ADC1_CNV_GPIO_Port, ADC1_CNV_Pin);
		delay_us(100);
		HAL_GPIO_WritePin(ADC1_CNV_GPIO_Port, ADC1_CNV_Pin, GPIO_PIN_RESET);		//Í£Ö¹×ª»»
		HAL_GPIO_WritePin(ADC1_SDI_GPIO_Port, ADC1_SDI_Pin, GPIO_PIN_SET);			//Ð¾Æ¬Ê¹ÄÜ
	 }
	 else
	 {
		 hAD7988_1x->PinSdi = AD7988_1_Pin_Init(ADC2_SDI_GPIO_Port, ADC2_SDI_Pin);
		 hAD7988_1x->PinSdo = AD7988_1_Pin_Init(ADC2_SDO_GPIO_Port, ADC2_SDO_Pin);
		 hAD7988_1x->PinSclk = AD7988_1_Pin_Init(ADC2_SCK_GPIO_Port, ADC2_SCK_Pin);
		 hAD7988_1x->PinCnv = AD7988_1_Pin_Init(ADC2_CNV_GPIO_Port, ADC2_CNV_Pin);
		 delay_us(100);
		 HAL_GPIO_WritePin(ADC2_CNV_GPIO_Port, ADC2_CNV_Pin, GPIO_PIN_RESET);		//Í£Ö¹×ª»»
		 HAL_GPIO_WritePin(ADC2_SDI_GPIO_Port, ADC2_SDI_Pin, GPIO_PIN_SET);			//Ð¾Æ¬Ê¹ÄÜ
	 }
}

uint16_t AD7988_1_ReadData(AD7988_1_HandleTypeDef *hAD7988_1x)
{
	uint16_t data=0;
	uint8_t i;

	HAL_GPIO_WritePin(hAD7988_1x->PinCnv.GPIOx, hAD7988_1x->PinCnv.GPIO_Pin, GPIO_PIN_SET);		//start conversion
	delay_us(2);																																							//wait for conversion complete
	HAL_GPIO_WritePin(hAD7988_1x->PinCnv.GPIOx, hAD7988_1x->PinCnv.GPIO_Pin, GPIO_PIN_RESET);		//enable output
	delay_us(1);																																							//wait for data ready
	for(i=0;i<16;i++)
	{
		HAL_GPIO_WritePin(hAD7988_1x->PinSclk.GPIOx, hAD7988_1x->PinSclk.GPIO_Pin, GPIO_PIN_SET);
	//	delay_us(10);	//start to read data
		data|=HAL_GPIO_ReadPin(hAD7988_1x->PinSdo.GPIOx, hAD7988_1x->PinSdo.GPIO_Pin);
		HAL_GPIO_WritePin(hAD7988_1x->PinSclk.GPIOx, hAD7988_1x->PinSclk.GPIO_Pin, GPIO_PIN_RESET);	//start to read data
		if(i<15) data<<=1;
	}
	return data;
}
