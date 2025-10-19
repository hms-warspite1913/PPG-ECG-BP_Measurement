#include "adc.h"
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
u16 AD_Value[2];;

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO,ENABLE ); //使能ADC1通道时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //72M/6=12,ADC最大时间不能超过14M 
                        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	//PA0/1作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	DMA_DeInit(DMA1_Channel1);   //将DMA的通道1寄存器重设为缺省值
  DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)ADC1_DR_Address;  //DMA外设ADC基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;  //DMA内存基地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //内存作为数据传输的目的地
  DMA_InitStructure.DMA_BufferSize = 2;  //DMA通道的DMA缓存的数据单元大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //数据宽度为16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //循环工作模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级 
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道
	
	ADC_DeInit(ADC1);  //将外设 ADC1 的全部寄存器重设为缺省值
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC工作模式:ADC1和ADC2工作在独立模式
  ADC_InitStructure.ADC_ScanConvMode =ENABLE;        //多信道扫描模式
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //模数转换工作在连续转换模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   //外部触发转换关闭
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //ADC数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 2;   //此处开2个信道（可开的为1~16）
  ADC_Init(ADC1, &ADC_InitStructure);    //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
        
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );                
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );
        
  ADC_DMACmd(ADC1, ENABLE);       //使能ADC1的DMA传输         
  ADC_Cmd(ADC1, ENABLE);           //使能指定的ADC1
  ADC_ResetCalibration(ADC1);        //复位指定的ADC1的校准寄存器
  while(ADC_GetResetCalibrationStatus(ADC1));        //获取ADC1复位校准寄存器的状态,设置状态则等待
  ADC_StartCalibration(ADC1);                //开始指定ADC1的校准状态
  while(ADC_GetCalibrationStatus(ADC1));                //获取指定ADC1的校准程序,设置状态则等待
	
	DMA_Cmd(DMA1_Channel1, ENABLE);//启动DMA通道			    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
}	

