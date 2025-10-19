#ifndef _Timer_H_
#define _Timer_H_
#include "sys.h"

extern u16 Max_1,Max_2,Time;
extern u8 Flag_1,num,Flag_2,Flag_M;//包络更新标志
extern u8 adc_flag; 
void TIM3_Init(u16 arr,u16 psc);
void TIM3_IRQHandler(void);
uint8_t FindMaxOscillation(uint16_t *max_pulse, uint16_t *corr_pressure);
#endif
