#include "sys.h"
#include "Pump.h"

void Pump_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

void Pump_ON(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
}

void Pump_OFF(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
}

void Pump_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_6) ==0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	}
}
