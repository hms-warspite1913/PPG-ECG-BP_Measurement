#include "stdint.h"
#ifndef __PWM_H
#define __PWM_H

extern uint8_t dt;//Õ¼¿Õ±È

void PWM_Init(uint16_t arr_0,uint16_t psc_0);
void PWM_SetCompare3(uint16_t Compare);
void PWM_SetFrq3(char *Frq);
void PWM_OP(void);
void PWM_STOP(void);

#endif
