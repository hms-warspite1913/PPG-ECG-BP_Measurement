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
/*
PUMP--PA6
VALVE--PA5
CUFE--PA0
PULSE--PA1
ECGOUT--PA7
PPGOUT--PA8
TX--PA2
RXP--PA3
SCL--PB8
SDA--PB9
KEY--PA9 PA10 PA11 PA12
*/


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
	




}

