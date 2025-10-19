#ifndef __MOTO_H
#define __MOTO_H
#include "sys.h"

#define Moto_PwmMax 1999
#define Moto_PwmMin 1000

void PWM_Moto(u16 MOTO3_PWM,u16 MOTO4_PWM);
void TIM2_Moto_Init(u16 arr,u16 psc);

#endif
