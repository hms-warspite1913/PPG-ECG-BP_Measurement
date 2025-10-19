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
uint8_t KeyNum;
extern u16 AD_Value[2];//AD转换缓冲区
#include "stm32f10x.h"




 int main(void)
 {
	OLED_Init();		//OLED初始化
	Pump_Init();
	Valve_Init();
	Valve_ON();
	NVIC_Configuration();	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	TIM3_Init(4999,71);    //启动包络提取(50HZ)(20点的滑动窗)
	Adc_Init();						//初始化AD 转换
	Key1_Init();						//初始化按键
	Key2_Init();
	



  while(1)
	{
		KeyNum = Key_GetNum();
    if(KeyNum==1)//180mmHg 当下降 20mmHg时启动波形记录
  	{
		    Blood_Pr(KeyNum);//依照按键值进行测量
			
		}
	}
}

