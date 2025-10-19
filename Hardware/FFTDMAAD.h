#include "stdint.h"
#ifndef __FFTAD_H
#define __FFTAD_H
#define nn 1024

extern uint16_t AD_Value[nn];//存放采样得到的模拟信号
extern uint16_t arr1,psc1;	 //存放采样信号参数
extern uint32_t fftin[nn],fftout[nn];//存放FFT的输入、输出信号
extern uint32_t fftbuffer[nn/2];  		//存放输出信号的模值
extern uint16_t fus;					//存放采样信号频率
extern uint16_t adxmin,adxmax;         //存放模拟信号最值
extern uint16_t fm,Af,Aff;			 //存放尖峰所在频率、峰峰值
extern uint32_t F;					 //存放计算得到的模拟信号频率
extern float Vol[128];			 //存放用于描绘波形的数据
extern float k;							 //绘图缩放比例
extern uint16_t AV;			//定义AD值变量

void TIM2_PWM_init(void);
void AD_FFT_Init(void);
void Timer4_Init(void);
void fre_calcu(void);
void TIM4_IRQHandler(void);

#endif
