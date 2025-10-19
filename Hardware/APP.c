#include "APP.h"
#include "Timer.h"
#include "Pump.h"
#include "Valve.h"
#include "OLED.h"
#include "Key.h"
#include "moto.h"
#include "Delay.h"

int Dt;//Dtλ����ĵ�ǰ��ֵ
u8 i,Flag_Y,Flag_T=0; //Ԥ�ϳ��־���ϳ��־
u16 RP=0,MP=0,speed=550; //���棬й���ٶ�
u16 High_P,Low_P,Ave_P; //����ѹ������ѹ
u16 num2=0;
u16 P_Max=0,H1=0,L1=0;//��¼��������ֵ
u8  mi=0; //��¼��������ֵ���ֵ�λ��
uint16_t corr_pressure=0;
extern u16 AD_Value[2]; //ADC DMA������
 
/******************************************************************************
���ܣ���ȡ�������ź�����
******************************************************************************/
//void Record(void)
//{
//	if(Flag_1==1)
//	{
//	Flag_1=0;
//	Dt=Max_2-RP; 
//	if(Flag_T==0&&Dt>20) Flag_Y=1; //��ǰ���ϳ���Dt����200���ж�ΪԤ�ϳ�
//	if(Flag_Y==1) 
//		{	
//		if(Max_2>40&&Dt>0) //��Ԥ�ϳ�׶� Max_2>400 ��Dt����0�� �����ж�Ϊ�ϳ�׶�
//			{
//			Flag_Y=0;//���Ԥ�ϳ�
//			MP=Max_1;//�������ѹ
//			Flag_T=1;//��־λ�ϳ�
//			}
//		else Flag_Y=0; //������Ԥ�ϳ��־
//		}
//	RP=Max_2;
//	if(Flag_T==1)
//		{
//			if(Dt<0) //��ֵ����
//			{
//			i++;//��ֵ����
//			Rec[i][0]=RP;
//			Rec[i][1]=MP;
//			Flag_T=0;
//			}
//		}
//	}
//	
//}
/***************************************************************************************
���ܣ���̬����
***************************************************************************************/
void Dynamic_count(void)
{

//			if(dt_r1>dt_r2) Low_P=ADCtoHg(Rec[i][1]); //�Ƚϲ�ֵѡ����ǡ����ֵ  
//			else Low_P=ADCtoHg(Rec[i-1][1]); //��AD��ת����mmHg��
//      P_Max=ADCtoHg(Rec[i][1]);
	  if(adc_flag == 1)
		{	FindMaxOscillation(&P_Max, &corr_pressure);
			High_P=ADCtoHg(corr_pressure)*1.3;
			Low_P=ADCtoHg(corr_pressure)*0.82;
			Ave_P=ADCtoHg(corr_pressure);
		}

		if(num==200)
		{
			OLED_ShowChinese(2,1,"��");
			OLED_ShowChinese(2,2,"��");
			OLED_ShowChinese(2,3,"ѹ");
			OLED_ShowString(2,8,":",OLED_8X16);
			OLED_ShowNum(2,9,High_P,3,OLED_8X16);
			OLED_ShowString(2,12,"mmHg",OLED_8X16);
			
			OLED_ShowChinese(3,1,"��");
			OLED_ShowChinese(3,2,"��");
			OLED_ShowChinese(3,3,"ѹ");
			OLED_ShowString(3,8,":",OLED_8X16);
			OLED_ShowNum(3,9,Low_P,3,OLED_8X16);
			OLED_ShowString(3,12,"mmHg",OLED_8X16);
		
			OLED_ShowChinese(4,1,"ƽ");
			OLED_ShowChinese(4,2,"��");
			OLED_ShowChinese(4,3,"ѹ");
			OLED_ShowString(4,8,":",OLED_8X16);
			OLED_ShowNum(4,9,Ave_P,3,OLED_8X16);
			OLED_ShowString(4,12,"mmHg",OLED_8X16);
			while (1); //���β�������
		}
		//	OLED_ShowString(4,12,"mmHg");			
			
	
}
/*******************************************************************************************
���ܣ�����Ѫѹ
*******************************************************************************************/
void Blood_Pr(u8 KeyNum)
{
	if(KeyNum==1)//180mmHg ���½� 20mmHgʱ�������μ�¼
		{
		Pump_ON();//��ʼ����
		while(1)
		{
			uint16_t A;
			A = AD_Value[0];
      OLED_ShowString(1, 1, "A:",OLED_8X16);	 
      OLED_ShowNum(1, 3, A, 4,OLED_8X16);	
		  if(AD_Value[0]>1862) //�䵽180mmHg
			{
			  Pump_OFF();//ֹͣ���� ������
			  while(1)
			  {
					if(AD_Value[0]<1752) TIM_Cmd(TIM3, ENABLE);//������ʱ����ȡ���� ����¼��������
					while(AD_Value[0]<1752) //��̬���� ����
					{
					Dynamic_count(); //���ж�̬����

					}
					num2++;
					OLED_ShowNum(1, 8, num2, 4,OLED_8X16);	
			  }
			}
		}
		}
}
/*******************************************************************************************
���ܣ�mmHg��ADCֵ��ת��
*******************************************************************************************/
u16 HgtoADC(u16 Hg)
{
		u16 Res;
		Res = (u16)(Hg*1024/99);
		return Res;
}
/*******************************************************************************************
���ܣ�ADC��mmHgֵ��ת��
*******************************************************************************************/
u16 ADCtoHg(u16 ADC)
{
		u16 Res;
		Res = (u16)(ADC*99/1024);
		return Res;
}
