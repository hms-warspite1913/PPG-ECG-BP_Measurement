#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Valve.h"
#include "delay.h"
#include "sys.h"
#include "Timer.h"
#include "Key.h"
#include "adc.h"
#include "APP.h"
#include "Pump.h"
#include "key.h"                 

//Ѫѹ��������� v0.2
uint8_t KeyNum;
extern u16 AD_Value[2];//ADת��������
void EXTI9_5_IRQHandler(void) 
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		while(1)
		{
			Blood_Pr();//���հ���ֵ���в���
		}
	}
}
