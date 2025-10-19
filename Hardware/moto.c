#include "moto.h"

u16 MOTO3_PWM = 0;
u16 MOTO4_PWM = 0;

void PWM_Moto(u16 MOTO3_PWM,u16 MOTO4_PWM)
{		
	TIM2->CCR3 = MOTO3_PWM;
	TIM2->CCR4 = MOTO4_PWM;
}

/*******************************************************
@ 定时器2 PWM输出初始化
@ arr：自动重装值
@ psc：时钟预分频数
*******************************************************/
void TIM2_Moto_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //使能GPIO外设时钟使能
	                                                                     	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4| GPIO_Pin_5|GPIO_Pin_6 |GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr;       //当定时器从0计数到999，即为1000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = psc;	    //设置预分频：不预分频，即为72MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //初始化TIM2

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为脉宽调制PWM模式2
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于跳变时为高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出比较模式使能
  TIM_OCInitStructure.TIM_Pulse = 0;	  //跳变值
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);	 //使能通道3
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出比较模式使能
  TIM_OCInitStructure.TIM_Pulse = 0;	  //跳变值
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);	//使能通道4
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);			 // 使能TIM2重载寄存器ARR
  TIM_Cmd(TIM2, ENABLE);                   //使能定时器3	   
	
	PWM_Moto(0,0);
}

