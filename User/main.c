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
#include "stdint.h"
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <stdio.h>
#include "lit.h"
#include "OLED.h"
#include "PWM.h"
#include "FFTDMAAD.h"
#include "usart.h"
uint16_t j,h;
uint8_t KeyNum=0;
extern u16 AD_Value[2];//AD转换缓冲区
u16 value;
extern u16 High_P,Low_P,Ave_P;
/*引脚表
PUMP--PA6
VALVE--PA5
CUFF--PA0
PULSE--PA1
ECGOUT--PC1
PPGOUT--PC0
TX--PA2
RX--PA3
SCL--PB8
SDA--PB9
KEY--PA9(BP) PA10(PPG) PA11(ECG) PA12
*/


 int main(void)
 {
	
	Pump_Init();
	Valve_Init();
	Valve_ON();
	NVIC_Configuration();	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	TIM3_Init(4999,71);    //启动包络提取(50HZ)(20点的滑动窗)
	USART2_Configuration();	//初始化串口2，波特率为115200
	Key1_Init();						//初始化按键
	
//PPG部分
	
	OLED_Init();		//OLED初始化
	TIM2_PWM_init();//AD采样时钟初始化
	
	Timer4_Init();	//波形描绘定时器初始化
	
	k = 6;//设置波形缩放幅度
	
	
	
	
	
	Adc_ECG_Init();

while(1)
	{
	KeyNum = Key_GetNum();

	if(KeyNum==1)//BP 180mmHg 当下降 20mmHg时启动波形记录
  	{
		Adc_Init();						//初始化AD 转换 
		Blood_Pr();
		
		printf("{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"HP\":{\"value\":%.d},\"LP\":{\"value\":%.d},\"AP\":{\"value\":%.d}}}",High_P,Low_P,Ave_P);
			
		
	}
	else if(KeyNum==2)//PPG
  	{
		AD_FFT_Init();	//采样通道与DMA初始化	
		TIM_Cmd(TIM2, ENABLE); 			//使能TIM2，采集脉冲波形
		TIM_Cmd(TIM4, ENABLE);			//使能TIM4，更新波形图像


			
		
	}
	else if(KeyNum==3)//v1.0 简单套用PPG代码
  	{
		   
		while(1)
		 { 
				value = Get_Adc(11);//读取ADC2的11通道
				char buffer_ECG[32]; // 减小缓冲区大小
				int index_ECG = 0;
				
			   
				
				index_ECG += sprintf(&buffer_ECG[index_ECG], "%d\r\n", value); // 发送单个数值+换行
				printf("{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"ECG\":{\"value\":%.d}}}",value);
				//UART_SendStr(buffer_ECG);
				
				Delay_ms(5); 
		 }
	 }
		
	}
}





