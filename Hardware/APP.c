#include "APP.h"
#include "Timer.h"
#include "Pump.h"
#include "Valve.h"
#include "OLED.h"
#include "Key.h"
#include "moto.h"
#include "Delay.h"

int Dt;//Dt位计算的当前差值
u8 i,Flag_Y,Flag_T=0; //预上冲标志，上冲标志
u16 RP=0,MP=0,speed=550; //缓存，泄气速度
u16 High_P,Low_P,Ave_P; //收缩压，舒张压
u16 num2=0;
u16 P_Max=0,H1=0,L1=0;//记录脉搏波峰值
u8  mi=0; //记录脉搏波峰值出现的位置
uint16_t corr_pressure=0;
extern u16 AD_Value[2]; //ADC DMA缓冲区
 
/******************************************************************************
功能：提取脉搏波信号特征
******************************************************************************/
//void Record(void)
//{
//	if(Flag_1==1)
//	{
//	Flag_1=0;
//	Dt=Max_2-RP; 
//	if(Flag_T==0&&Dt>20) Flag_Y=1; //当前无上冲且Dt大于200则判定为预上冲
//	if(Flag_Y==1) 
//		{	
//		if(Max_2>40&&Dt>0) //在预上冲阶段 Max_2>400 且Dt大于0则 可以判定为上冲阶段
//			{
//			Flag_Y=0;//清除预上冲
//			MP=Max_1;//保存袖带压
//			Flag_T=1;//标志位上冲
//			}
//		else Flag_Y=0; //否侧清除预上冲标志
//		}
//	RP=Max_2;
//	if(Flag_T==1)
//		{
//			if(Dt<0) //峰值出现
//			{
//			i++;//峰值计数
//			Rec[i][0]=RP;
//			Rec[i][1]=MP;
//			Flag_T=0;
//			}
//		}
//	}
//	
//}
/***************************************************************************************
功能：动态计算
***************************************************************************************/
void Dynamic_count(void)
{

//			if(dt_r1>dt_r2) Low_P=ADCtoHg(Rec[i][1]); //比较产值选择最恰当的值  
//			else Low_P=ADCtoHg(Rec[i-1][1]); //将AD量转化成mmHg量
//      P_Max=ADCtoHg(Rec[i][1]);
	  if(adc_flag == 1)
		{	FindMaxOscillation(&P_Max, &corr_pressure);
			High_P=ADCtoHg(corr_pressure)*1.2;
			Low_P=ADCtoHg(corr_pressure)*0.8;
			Ave_P=ADCtoHg(corr_pressure);
		}

		if(num==200)
		{
			OLED_ClearArea(0,0,128,64);
			OLED_Update();
			OLED_ShowChinese(0,0,"你");
			OLED_ShowChinese(16,0,"缩");
			OLED_ShowChinese(32,0,"压");
			OLED_ShowString(48,0,":",OLED_8X16);
			OLED_ShowNum(56,0,High_P,3,OLED_8X16);
			OLED_ShowString(80,0,"mmHg",OLED_8X16);
			
			OLED_ShowChinese(0,16,"舒");
			OLED_ShowChinese(16,16,"张");
			OLED_ShowChinese(32,16,"压");
			OLED_ShowString(48,16,":",OLED_8X16);
			OLED_ShowNum(56,16,Low_P,3,OLED_8X16);
			OLED_ShowString(80,16,"mmHg",OLED_8X16);
		
			OLED_ShowChinese(0,32,"平");
			OLED_ShowChinese(16,32,"均");
			OLED_ShowChinese(32,32,"压");
			OLED_ShowString(48,32,":",OLED_8X16);
			OLED_ShowNum(56,32,Ave_P,3,OLED_8X16);
			OLED_ShowString(80,32,"mmHg",OLED_8X16);
			OLED_Update();
			
			while (1); //本次测量结束
		}
		//	OLED_ShowString(4,12,"mmHg");			
			
	
}
/*******************************************************************************************
功能：测量血压
*******************************************************************************************/
//去除按键值KEYNUM输入 v0.2
void Blood_Pr(void)
{
	//180mmHg 当下降 20mmHg时启动波形记录
		
		Pump_ON();//开始充气
		while(1)
		{
			uint16_t A;
			A = AD_Value[0];
		OLED_ClearArea(0,0,128,64);	
      OLED_ShowString(1, 1, "A:",OLED_6X8);	 
      OLED_ShowNum(10, 1, A, 4,OLED_6X8);	
		OLED_ShowChinese(16,16,"你");
	OLED_Update();
			OLED_ClearArea(0,0,128,64);
			
		  if(AD_Value[0]>1862) //充到180mmHg
			{
			  Pump_OFF();//停止充气 并放气
			  while(1)
			  {
					if(AD_Value[0]<1752) TIM_Cmd(TIM3, ENABLE);//启动定时器提取包络 并记录波形特征
					while(AD_Value[0]<1752) //动态计算 放气
					{
					Dynamic_count(); //进行动态计算

					}
					num2++;
					OLED_ClearArea(0,0,128,64);
					OLED_ShowNum(1, 8, num2, 4,OLED_6X8);
					OLED_Update();
					
					
			  }
			}
		}
		
}
/*******************************************************************************************
功能：mmHg和ADC值间转换
*******************************************************************************************/
u16 HgtoADC(u16 Hg)
{
		u16 Res;
		Res = (u16)(Hg*1024/99);
		return Res;
}
/*******************************************************************************************
功能：ADC和mmHg值间转换
*******************************************************************************************/
u16 ADCtoHg(u16 ADC)
{
		u16 Res;
		Res = (u16)(ADC*99/1024);
		return Res;
}
