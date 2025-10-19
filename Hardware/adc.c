#include "adc.h"
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
u16 AD_Value[2];;

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO,ENABLE ); //ʹ��ADC1ͨ��ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //72M/6=12,ADC���ʱ�䲻�ܳ���14M 
                        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	//PA0/1��Ϊģ��ͨ����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	DMA_DeInit(DMA1_Channel1);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
  DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)ADC1_DR_Address;  //DMA����ADC����ַ
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;  //DMA�ڴ����ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
  DMA_InitStructure.DMA_BufferSize = 2;  //DMAͨ����DMA��������ݵ�Ԫ��С
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //ѭ������ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ� 
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��
	
	ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
  ADC_InitStructure.ADC_ScanConvMode =ENABLE;        //���ŵ�ɨ��ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //ģ��ת������������ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   //�ⲿ����ת���ر�
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //ADC�����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 2;   //�˴���2���ŵ����ɿ���Ϊ1~16��
  ADC_Init(ADC1, &ADC_InitStructure);    //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
        
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );                
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );
        
  ADC_DMACmd(ADC1, ENABLE);       //ʹ��ADC1��DMA����         
  ADC_Cmd(ADC1, ENABLE);           //ʹ��ָ����ADC1
  ADC_ResetCalibration(ADC1);        //��λָ����ADC1��У׼�Ĵ���
  while(ADC_GetResetCalibrationStatus(ADC1));        //��ȡADC1��λУ׼�Ĵ�����״̬,����״̬��ȴ�
  ADC_StartCalibration(ADC1);                //��ʼָ��ADC1��У׼״̬
  while(ADC_GetCalibrationStatus(ADC1));                //��ȡָ��ADC1��У׼����,����״̬��ȴ�
	
	DMA_Cmd(DMA1_Channel1, ENABLE);//����DMAͨ��			    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
}	

