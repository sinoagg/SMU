#include "AD5689R.h"


static General_GPIOTypeDef AD5689R_Pin_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	General_GPIOTypeDef PinName; 
	PinName.GPIOx=GPIOx;
	PinName.GPIO_Pin=GPIO_Pin;
	return PinName;
}

void AD5689R_Init(AD5689R_HandleTypeDef *hAD5689Rx, int8_t DAC_num)
{
	hAD5689Rx->PinReset = AD5689R_Pin_Init(DAC1_RESET_GPIO_Port, DAC1_RESET_Pin);
	hAD5689Rx->PinFsync = AD5689R_Pin_Init(DAC1_FSYNC_GPIO_Port, DAC1_FSYNC_Pin);
	hAD5689Rx->PinSclk = AD5689R_Pin_Init(DAC1_SCLK_GPIO_Port, DAC1_SCLK_Pin);
	hAD5689Rx->PinSdata = AD5689R_Pin_Init(DAC1_SDATA_GPIO_Port, DAC1_SDATA_Pin);
	HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(hAD5689Rx->PinFsync.GPIOx, hAD5689Rx->PinFsync.GPIO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(hAD5689Rx->PinReset.GPIOx, hAD5689Rx->PinReset.GPIO_Pin, GPIO_PIN_SET);
	delay_us(100);
	AD5689R_ChangeMode(&(*hAD5689Rx));
}

static void AD5689R_WriteCmdCode(AD5689R_HandleTypeDef *hAD5689Rx, uint8_t Cmd)
{
	uint8_t i;
	uint8_t j=0x08;

	for(i=0;i<4;i++)
	{
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_SET);
		if(j&Cmd)
		HAL_GPIO_WritePin(hAD5689Rx->PinSdata.GPIOx, hAD5689Rx->PinSdata.GPIO_Pin, GPIO_PIN_SET);
		else
		HAL_GPIO_WritePin(hAD5689Rx->PinSdata.GPIOx, hAD5689Rx->PinSdata.GPIO_Pin, GPIO_PIN_RESET);
		j>>=1;
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_RESET);
	}
}


static void AD5689R_ChangeMode(AD5689R_HandleTypeDef *hAD5689Rx)					//default mode is power down
{
	uint8_t i;
	uint16_t j=0x80;

	delay_us(10);
	HAL_GPIO_WritePin(hAD5689Rx->PinFsync.GPIOx, hAD5689Rx->PinFsync.GPIO_Pin, GPIO_PIN_RESET);
	delay_us(10);
	AD5689R_WriteCmdCode(&(*hAD5689Rx), 0x04);
	for(i=0;i<16;i++)
	{
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_SET);
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_RESET);
	}
	for(i=0;i<8;i++)
	{
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_SET);
		if(j&0x3C)
		HAL_GPIO_WritePin(hAD5689Rx->PinSdata.GPIOx, hAD5689Rx->PinSdata.GPIO_Pin, GPIO_PIN_SET);
		else
		HAL_GPIO_WritePin(hAD5689Rx->PinSdata.GPIOx, hAD5689Rx->PinSdata.GPIO_Pin, GPIO_PIN_RESET);
		j>>=1;
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_RESET);
	}
	delay_us(100);
	HAL_GPIO_WritePin(hAD5689Rx->PinFsync.GPIOx, hAD5689Rx->PinFsync.GPIO_Pin, GPIO_PIN_SET);
}

void AD5689R_WriteIR(AD5689R_HandleTypeDef *hAD5689Rx, uint8_t channel, uint16_t val)
{
	uint8_t i;
	uint16_t j=0x8000;

	delay_us(10);
	HAL_GPIO_WritePin(hAD5689Rx->PinFsync.GPIOx, hAD5689Rx->PinFsync.GPIO_Pin, GPIO_PIN_RESET);
	delay_us(10);
	AD5689R_WriteCmdCode(&(*hAD5689Rx), 0x01);					//write and update channel n
	AD5689R_WriteChCode(&(*hAD5689Rx), channel);				//write channel
	for(i=0;i<16;i++)
	{
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_SET);
		if(j&val)
		HAL_GPIO_WritePin(hAD5689Rx->PinSdata.GPIOx, hAD5689Rx->PinSdata.GPIO_Pin, GPIO_PIN_SET);
		else
		HAL_GPIO_WritePin(hAD5689Rx->PinSdata.GPIOx, hAD5689Rx->PinSdata.GPIO_Pin, GPIO_PIN_RESET);
		j>>=1;
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_RESET);
	}
	delay_us(10);
	HAL_GPIO_WritePin(hAD5689Rx->PinFsync.GPIOx, hAD5689Rx->PinFsync.GPIO_Pin, GPIO_PIN_SET);
}

void AD5689R_WriteChCode(AD5689R_HandleTypeDef *hAD5689Rx, uint8_t ch)
{
	uint8_t i;
	uint8_t j=0x08;

	if(ch==0)
		ch=1<<3;
	for(i=0;i<4;i++)
	{
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_SET);
		if(j&ch)
		HAL_GPIO_WritePin(hAD5689Rx->PinSdata.GPIOx, hAD5689Rx->PinSdata.GPIO_Pin, GPIO_PIN_SET);
		else
		HAL_GPIO_WritePin(hAD5689Rx->PinSdata.GPIOx, hAD5689Rx->PinSdata.GPIO_Pin, GPIO_PIN_RESET);
		j>>=1;
		delay_us(10);
		HAL_GPIO_WritePin(hAD5689Rx->PinSclk.GPIOx, hAD5689Rx->PinSclk.GPIO_Pin, GPIO_PIN_RESET);
	}	
}
