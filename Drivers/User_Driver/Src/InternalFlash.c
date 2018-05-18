#include "InternalFlash.h"

void GetFlashData_Float(Float_Union_Data* pDataArray, uint32_t flash_addr, uint8_t length)
{
	uint8_t i;
	uint32_t temp;
	for(i=0;i<length;i++)
	{
		temp=FlashRead32bit(flash_addr);
		Uint32to4Uint8(temp, &(pDataArray[i].number_uchar[0]));
		flash_addr+=4;
	}
}

void GetFlashData_U32(uint32_t* pdata, uint32_t flash_addr, uint8_t length)
{
	uint8_t i;
	for(i=0;i<length;i++)
	{
		*pdata=FlashRead32bit(flash_addr);
		flash_addr+=4;
	}
}

static uint32_t FlashRead32bit(uint32_t ReadAddr)
{
	uint32_t rtn_value;
	rtn_value=*(__IO uint32_t*)ReadAddr;//¶ÁÈ¡4¸ö×Ö½Ú.		
	return rtn_value;
}

static void Uint32to4Uint8(uint32_t uint32_data, uint8_t* pBuffer)
{
	uint8_t i;
	for(i=0;i<4;i++)
	{
		*(pBuffer+i)=(uint8_t)(uint32_data&0xFF);
		uint32_data>>=8;
	}
}

void FlashErase(uint32_t start_address,uint32_t end_address)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError = 0;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = start_address;
  EraseInitStruct.NbPages     = (end_address - start_address+1) / FLASH_PAGE_SIZE;
	while(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK);
	
}
