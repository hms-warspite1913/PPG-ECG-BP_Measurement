#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//¶ÁÈ¡°´¼ü1

void Key1_Init(void);
void Key2_Init(void);
uint8_t Key_GetNum(void);
#endif
