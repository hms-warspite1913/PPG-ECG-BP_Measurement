#include "stm32f10x.h"                  // Device header
#include <math.h>
#include <table_fft.h>
#include <stm32_dsp.h>
#include "OLED.h"
#include "PWM.h"

#define nn 1024 									

uint16_t AD_Value_PPG[nn];						//定义用于存放AD转换结果的全局数组,一次采样1024个点用于fft计算频率
												//由于与血压adc.c的AD_Value_PPG冲突改名
uint16_t arr1,psc1;								//定义用于存放arr和psc的全局变量
uint32_t fus;											//采样频率 
float adxmin,adxmax;         //存放采集到的模拟信号最值		
uint32_t fftin[nn],fftout[nn];//fft输入信号 fft输出信号
uint32_t fftbuffer[nn/2];  		//fft变换后信号的模值
uint16_t fm,Af,Aff;										//频谱尖峰频率  信号峰峰值
uint32_t F; 											//模拟信号频率
uint16_t AV;				 //存绘图用波形数据
float Vol[128];			 //存放用于描绘波形的数据
float k;						 //绘图缩放比例

/**
  * 函    数：采样脉冲初始化
  * 参    数：arr,psc,PWM频率=ck_psc/(psc+1)/(arr+1)
							此处时钟不分频，ck_psc=72M
  * 返 回 值：无
  */
void TIM2_PWM_init(void)
{
	arr1=389;
	psc1=719;
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;			//定义定时器初始化结构体变量
	TIM_OCInitTypeDef TIM_OCInitStructure;							//定义输出比较初始化结构体变量
	TIM_OCStructInit(&TIM_OCInitStructure);							//结构体初始化，若结构体没有完整赋值

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 		//时钟使能

	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr1; 		//计数周期，即ARR的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc1; 	//预分频器，即PSC的值		
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);			//将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;							//输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输出使能	
	TIM_OCInitStructure.TIM_Pulse = 1;													//初始的CCR值,PWM占空比Duty=ccr/(arr+1)
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//输出极性，选择极性为高
	TIM_OC2Init(TIM2, & TIM_OCInitStructure);											//PA0对应TIM2_CH1,因此应使用TIM_OC1Init
	
	TIM_CtrlPWMOutputs(TIM2, ENABLE); 	//使能PWM波对外设的输出
	TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_OC2Ref);//选择TIM2上升沿作为ADC触发源
}

/**
* 函    数：FFT AD初始化,PB1初始化为ADC端口并使能DMA将转化结果转运至内存
  * 参    数：无
  * 返 回 值：无
  */
//v0.3 改为adc3 dma2 pc0
void AD_FFT_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);	//开启ADC1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//开启GPIOB的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);		//开启DMA1的时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);					//选择时钟6分频，ADCCLK = 72MHz / 6 = 12MHz
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PB1引脚初始化为模拟输入
	
	/*规则组通道配置*/
	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);	//规则组序列1的位置，配置为通道9(PB1)
		
	/*ADC初始化*/
	ADC_InitTypeDef ADC_InitStructure;											//定义结构体变量
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;							//模式，选择独立模式，即单独使用ADC1
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//数据对齐，选择右对齐
	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;//ADC_ExternalTrigConv_None;		//外部触发，使用定时器2输出脉冲触发
	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;							//连续转换，失能，每接收到一次定时器脉冲进行一次采集
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;								//扫描模式，失能，每接收到一次定时器脉冲进行一次采集
	ADC_InitStructure.ADC_NbrOfChannel = 1;										//通道数，为1，只扫描规则组的1个通道
	ADC_Init(ADC3, &ADC_InitStructure);											//将结构体变量交给ADC_Init，配置ADC1
	
	/*DMA初始化*/
	DMA_InitTypeDef DMA_InitStructure;											//定义结构体变量
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;							//外设基地址，给定形参AddrA
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据宽度，选择半字，对应16为的ADC数据寄存器
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//外设地址自增，选择失能，始终以ADC数据寄存器为源
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value_PPG;					//存储器基地址，给定存放AD转换结果的全局数组AD_Value_PPG
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//存储器数据宽度，选择半字，与源数据宽度对应
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							//存储器地址自增，选择使能，每次转运后，数组移到下一个位置
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//数据传输方向，选择由外设到存储器，ADC数据寄存器转到数组
	DMA_InitStructure.DMA_BufferSize = nn;													//转运的数据大小（转运次数），与ADC通道数一致
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;									//是否自动重装，选择循环模式，与ADC的连续转换一致
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;										//存储器到存储器，选择失能，数据由ADC外设触发转运到存储器
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;							//优先级，选择高
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);									//将结构体变量交给DMA_Init，配置DMA1的通道1
		
	/*DMA和ADC使能*/
	DMA_Cmd(DMA2_Channel5, ENABLE);							//DMA1的通道1使能
	ADC_DMACmd(ADC3, ENABLE);								//ADC1触发DMA1的信号使能
	ADC_Cmd(ADC3, ENABLE);									//ADC1使能
  
	/*ADC校准*/
	ADC_ResetCalibration(ADC3);								//固定流程，内部有电路会自动执行校准
	while (ADC_GetResetCalibrationStatus(ADC3) == SET);
	ADC_StartCalibration(ADC3);
	while (ADC_GetCalibrationStatus(ADC3) == SET);
	/*ADC触发*/
	ADC_ExternalTrigConvCmd(ADC3,ENABLE);//硬件触发ADC开始工作
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//软件触发
}

/**
* 函    数：数据处理，运行此函数可获得ADC端口输入信号的频率、振幅等参数
  * 参    数：无
  * 返 回 值：无
  */
//太大了占用7KB

void fre_calcu(void)
{
	signed short lX,lY;//存放变换后信号的实部和虚部
  float X,Y;//存放变换为浮点型数据的信号的实部和虚部
	float Mag;//存放信号的模值
  unsigned short i;
	
	fus=72000000/(arr1*psc1);
	adxmax=AD_Value_PPG[0];
	adxmin=AD_Value_PPG[0];
	for(i=0;i<nn;i++)
	{
		fftin[i] = 0;
		fftin[i] =((u32)(AD_Value_PPG[i] ))<< 16;
		
		if(AD_Value_PPG[i] >= adxmax)
		{	    
			adxmax = AD_Value_PPG[i];
		}			
		if(AD_Value_PPG[i] <= adxmin)
		{		    
			adxmin = AD_Value_PPG[i];
		}						
	}
	adxmin=(float)adxmin*3.3/4095; adxmax =(float)adxmax *3.3/4095;
	Af=adxmax -adxmin ;//最大值、最小值、幅值
	if (Af<0.1)
	{
		Af = adxmax;
		Aff = 10*(adxmax-Af);
	}
	else
	{
		Aff = 10*adxmax - 10*adxmin;
		if(Aff>10)Aff = Aff - 10*Af;
	}
  cr4_fft_1024_stm32(fftout ,fftin ,nn);
  for(i=0; i<nn/2; i++)
  {
      lX  = (fftout [i] << 16) >> 16;//取变换后数据的虚部
      lY  = (fftout [i] >> 16);			 //取变换后数据的实部
    //将信号变换为浮点型数据 
      X = nn * ((float)lX) / 32768;
      Y = nn * ((float)lY) / 32768;
		
      Mag = sqrt(X * X + Y * Y) / nn;
      fftbuffer[i] = (unsigned long)(Mag * 32768);//将模值变回长整型存入数组中
  }
  Mag=fm =0;
  for(i=1;i<nn/2;i++)//找出尖峰幅值
  {
    if(Mag<fftbuffer [i])
		{
			Mag=fftbuffer [i];
			fm =i;
		}
  }
	F=(u32)(fm*fus/nn);
}


/**
* 函    数：定时器4中断初始化，该定时器用于更新显示屏内容
  * 参    数：无
  * 返 回 值：无
  */
void Timer4_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//开启TIM4的时钟
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM4);		//选择TIM4为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 390 - 1;				//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM4的时基单元	
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);					//开启TIM4的更新中断
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
																//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;											//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;						//选择配置NVIC的TIM2线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);														//将结构体变量交给NVIC_Init，配置NVIC外设
}

/**
* 函    数：波形更新
  * 参    数：无
  * 返 回 值：无
  */
void TIM4_IRQHandler(void)
{
	unsigned short i;
	unsigned short j;
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		
		/*显示波形参数*/
		fre_calcu();																				
		OLED_ClearArea(0,0,128,34);
		OLED_ShowString(0,0,"Frequency:",OLED_6X8);
		OLED_ShowString(0,10,"Voltage:",OLED_6X8);
		
		OLED_ShowString(90,0,"Hz",OLED_6X8);
		OLED_ShowString(90,10,"V",OLED_6X8);
		
		OLED_ShowNum(64,0,F,5,OLED_6X8);
		
		OLED_ShowNum(64,10,Af,1,OLED_6X8);
		OLED_ShowString(70,10,".",OLED_6X8);
		OLED_ShowNum(76,10,Aff,1,OLED_6X8);
		
		/*更新波形*/
		OLED_ClearArea(0,30,128,34);	
		
		for(i=0; i<128; i++) {
			float sum = 0;
			for(j=0; j<8; j++) sum += AD_Value_PPG[i*8 + j]; // 每8点取平均
			Vol[i] = (sum / 8) / 4095 * 3.3;
		}

		for(i=0;i<128;i++)
		{
			if((Vol[i]-Vol[i+1])>0.5||(Vol[i]-Vol[i+1])<0.5)  //如果两个值相差太大，值绘制直线
			{
				OLED_DrawLine(i,64-Vol[i]*k,i,64-Vol[i+1]*k);
			}
			else
			{
				OLED_DrawPoint(i, 64-Vol[i]*k); 
			}
		}
		OLED_Update();
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
