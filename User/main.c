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
uint16_t j;
uint8_t KeyNum;
extern u16 AD_Value[2];//AD转换缓冲区
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
KEY--PA9(BP) PA10(PPG) PA11(ECG) PA12
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
//PPG部分
	Init_sig();			//让板子上的灯亮起来方便判断板子坏没坏
	OLED_Init();		//OLED初始化
	TIM2_PWM_init();//AD采样时钟初始化
	AD_FFT_Init();	//采样通道与DMA初始化	
	Timer4_Init();	//波形描绘定时器初始化
	USART2_Configuration();	//初始化串口2，波特率为9600
	PWM_Init(100,720);//在这句设置PWM的频率，第一个参数是arr，第二个参数是psc
	dt = 66;//初始占空比设置为66%
	PWM_SetCompare3(dt);//这句设置占空比
	k = 6;//设置波形缩放幅度
	
	TIM_Cmd(TIM2, ENABLE); 			//使能TIM2，采集脉冲波形
	TIM_Cmd(TIM4, ENABLE);			//使能TIM4，更新波形图像
	TIM_Cmd(TIM3, ENABLE);			//使能TIM3，PB0输出PWM

while(1)
	{
	KeyNum = Key_GetNum();
    if(KeyNum==1)//BP 180mmHg 当下降 20mmHg时启动波形记录
  	{
		 Blood_Pr();
	}
	else if(KeyNum==2)//PPG
  	{
		while (1)
{
		// 每次发送一组完整的波形数据
		char buffer[256]; // 缓冲区大小根据实际调整
		int index = 0;
		
		// 创建标题行
	//    index += sprintf(&buffer[index], "ADC Values:");
		
		// 添加数据点
		for(j = 0; j < 64; j++) // 减少点数到64，提高刷新速度
		{
			index += sprintf(&buffer[index], " %d", AD_Value_PPG[j]);
		}
		index += sprintf(&buffer[index], "\r\n"); // 换行符
		
		// 一次性发送整个缓冲区
		UART_SendStr(buffer);
		
		// 减小时延，提高刷新率
		Delay_ms(1); // 从5ms改为1ms
}    
			
		
	}
	else if(KeyNum==3)
  	{
		   
			
		
	}
}
}




