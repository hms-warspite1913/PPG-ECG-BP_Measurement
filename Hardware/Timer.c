#include "Timer.h"
#include "key.h"
#include "adc.h"
#include "APP.h"
#include "OLED.h"

#define SAMPLE_DURATION 25    // �ɼ���ʱ�����룩
#define SAMPLE_INTERVAL 5     // ������������룩
#define SAMPLE_COUNT 3000  // �ܲ���������5000��
u8 adc_flag = 0;  
u8 ms1=0;
extern u16 AD_Value[2];//����ADת��������
u8 num=0,Flag_M,Flag_1;//������±�־
// 2�����飺[��][0]���ѹ��PA0����[��][1]��������PA1��
uint16_t adc_data[SAMPLE_COUNT][2] = {0};  
uint32_t data_index = 0;       // ����������0~4999��
        // �ɼ���ɱ�־��0��δ��ɣ�1����ɣ�

u16 Max_1=0,Max_2=0; //����ʵʱֵ
u8 Flag_2=0; //��־
u16 VM[6]; //ƽ�Ȳ���������
u16 Time=0; //���ڲ�����������

//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 

	if(TIM3->SR&0X0001)//����ж�
	{
		TIM3->SR = ~TIM_FLAG_Update;//���TIMx���жϴ�����λ:TIM �ж�Դ 
//		if(AD_Value[0]>Max_1) Max_1 = (u16)AD_Value[0]; //Ѱ�����ֵ ����ת��
//		if(AD_Value[1]>Max_2) Max_2 = (u16)AD_Value[1];
 
		ms1++;//ʱ���ۼ�

		if(AD_Value[0]>Max_1) Max_1 = (u16)AD_Value[0]; //Ѱ�����ֵ ����ת��
		if(AD_Value[1]>Max_2) Max_2 = (u16)AD_Value[1];
//		OLED_ShowString(1, 1, "F1:");
//    OLED_ShowNum(1, 4, Flag_1, 4);
		if(ms1==20)  //20����λ���ƶ���
			{	
			ms1=0;
			num++;
			}
		
	  if (data_index < SAMPLE_COUNT) {
                        // ��ȡ���ݣ�[0]���ѹ��ͨ��0����[1]��������ͨ��1��
            adc_data[data_index][0] = (u16)AD_Value[0];  // ��16λΪͨ��0
            adc_data[data_index][1] = (u16)AD_Value[1];  // ��16λΪͨ��1��ɨ��ģʽ�£�
            
            data_index++;  // ��������
        } else {
  // �ɼ���ɣ��رն�ʱ��
            adc_flag = 1;            // ������ɱ�־
        }	
	}
}

uint8_t FindMaxOscillation(uint16_t *max_pulse, uint16_t *corr_pressure) 
{

    
    uint32_t max_index = 0;
    uint16_t max_val = 0;
    uint8_t found = 0;
    
    // �������������ݣ�����ǰ2���ͺ�2���㣬����߽�������
    for (uint32_t i = 2; i < SAMPLE_COUNT - 2; i++) {
        // �жϾֲ���ֵ����ǰ�� > ǰ��2���㣨����������
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
        *corr_pressure = adc_data[max_index][0];  // ��Ӧ���ѹ
        return 0;
    } else {
        return 1;  // δ�ҵ���Ч��ֵ
    }
}


/*******************************************************
@ ��ʱ��3 ��ʼ��
@ ����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
@ arr���Զ���װֵ
@ psc��ʱ��Ԥ��Ƶ��
*******************************************************/
void TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update ,ENABLE );//ʹ�ܻ���ʧ��ָ����TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
}



