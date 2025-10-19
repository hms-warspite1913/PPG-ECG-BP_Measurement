#include "moto.h"

u16 MOTO3_PWM = 0;
u16 MOTO4_PWM = 0;

void PWM_Moto(u16 MOTO3_PWM,u16 MOTO4_PWM)
{		
	TIM2->CCR3 = MOTO3_PWM;
	TIM2->CCR4 = MOTO4_PWM;
}

/*******************************************************
@ ��ʱ��2 PWM�����ʼ��
@ arr���Զ���װֵ
@ psc��ʱ��Ԥ��Ƶ��
*******************************************************/
void TIM2_Moto_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��GPIO����ʱ��ʹ��
	                                                                     	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4| GPIO_Pin_5|GPIO_Pin_6 |GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr;       //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = psc;	    //����Ԥ��Ƶ����Ԥ��Ƶ����Ϊ72MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//����ʱ�ӷ�Ƶϵ��������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //��ʼ��TIM2

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����Ϊ�������PWMģʽ2
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС������ʱΪ�ߵ�ƽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//����Ƚ�ģʽʹ��
  TIM_OCInitStructure.TIM_Pulse = 0;	  //����ֵ
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);	 //ʹ��ͨ��3
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//����Ƚ�ģʽʹ��
  TIM_OCInitStructure.TIM_Pulse = 0;	  //����ֵ
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);	//ʹ��ͨ��4
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);			 // ʹ��TIM2���ؼĴ���ARR
  TIM_Cmd(TIM2, ENABLE);                   //ʹ�ܶ�ʱ��3	   
	
	PWM_Moto(0,0);
}

