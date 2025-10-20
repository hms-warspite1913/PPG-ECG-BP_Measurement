#include "Timer.h"
#include "key.h"
#include "adc.h"
#include "APP.h"
#include "OLED.h"

#define SAMPLE_DURATION 25    // 采集总时长（秒）
#define SAMPLE_INTERVAL 5     // 采样间隔（毫秒）
#define SAMPLE_COUNT 3000  // 总采样次数：5000次
u8 adc_flag = 0;  
u8 ms1=0;
extern u16 AD_Value[2];//声明AD转换缓冲区
u8 num=0,Flag_M,Flag_1;//包络更新标志
// 2列数组：[行][0]袖带压（PA0），[行][1]脉搏波（PA1）
uint16_t adc_data[SAMPLE_COUNT][2] = {0};  
uint32_t data_index = 0;       // 数据索引（0~4999）
        // 采集完成标志（0：未完成，1：完成）

u16 Max_1=0,Max_2=0; //包络实时值
u8 Flag_2=0; //标志
u16 VM[6]; //平稳测量缓冲区
u16 Time=0; //用于测量计算心率

//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 

	if(TIM3->SR&0X0001)//溢出中断
	{
		TIM3->SR = ~TIM_FLAG_Update;//清除TIMx的中断待处理位:TIM 中断源 
//		if(AD_Value[0]>Max_1) Max_1 = (u16)AD_Value[0]; //寻找最大值 类型转换
//		if(AD_Value[1]>Max_2) Max_2 = (u16)AD_Value[1];
 
		ms1++;//时间累加

		if(AD_Value[0]>Max_1) Max_1 = (u16)AD_Value[0]; //寻找最大值 类型转换
		if(AD_Value[1]>Max_2) Max_2 = (u16)AD_Value[1];
//		OLED_ShowString(1, 1, "F1:");
//    OLED_ShowNum(1, 4, Flag_1, 4);
		if(ms1==20)  //20个单位的移动窗
			{	
			ms1=0;
			num++;
			}
		
	  if (data_index < SAMPLE_COUNT) {
                        // 读取数据：[0]袖带压（通道0），[1]脉搏波（通道1）
            adc_data[data_index][0] = (u16)AD_Value[0];  // 低16位为通道0
            adc_data[data_index][1] = (u16)AD_Value[1];  // 高16位为通道1（扫描模式下）
            
            data_index++;  // 索引自增
        } else {
  // 采集完成，关闭定时器
            adc_flag = 1;            // 设置完成标志
        }	
	}
}

uint8_t FindMaxOscillation(uint16_t *max_pulse, uint16_t *corr_pressure) 
{

    
    uint32_t max_index = 0;
    uint16_t max_val = 0;
    uint8_t found = 0;
    
    // 遍历脉搏波数据（跳过前2个和后2个点，避免边界噪声）
    for (uint32_t i = 2; i < SAMPLE_COUNT - 2; i++) {
        // 判断局部峰值：当前点 > 前后2个点（抑制噪声）
        if (adc_data[i][1] > adc_data[i-2][1] && 
            adc_data[i][1] > adc_data[i-1][1] && 
            adc_data[i][1] > adc_data[i+1][1] && 
            adc_data[i][1] > adc_data[i+2][1]) {
            
            if (adc_data[i][1] > max_val) {
                max_val = adc_data[i][1];
                max_index = i;
                found = 1;
            }
        }
    }
    
    if (found) {
        *max_pulse = max_val;
        *corr_pressure = adc_data[max_index][0];  // 对应袖带压
        return 0;
    } else {
        return 1;  // 未找到有效峰值
    }
}


/*******************************************************
@ 定时器3 初始化
@ 这里时钟选择为APB1的2倍，而APB1为36M
@ arr：自动重装值
@ psc：时钟预分频数
*******************************************************/
void TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update ,ENABLE );//使能或者失能指定的TIM3中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
}



