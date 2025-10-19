#include "stm32f10x.h"                  // Device header

void Init_sig(void)
{
/*ģ���ʼ��*/
	
		/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//����GPIOC��ʱ��
															//ʹ�ø�������ǰ���뿪��ʱ�ӣ����������Ĳ�����Ч
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;					//����ṹ�����
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//GPIOģʽ����ֵΪ�������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				//GPIO���ţ���ֵΪ��13������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO�ٶȣ���ֵΪ50MHz
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);					//����ֵ��Ĺ���������ݸ�GPIO_Init����
															//�����ڲ����Զ����ݽṹ��Ĳ���������Ӧ�Ĵ���
															//ʵ��GPIOC�ĳ�ʼ��
	
	/*����GPIO���ŵĸߵ͵�ƽ*/
	/*��������GPIO���ŵĵ�ƽ������GPIO��ʼ��Ϊ���������ָ������Ĭ������͵�ƽ*/
//	GPIO_SetBits(GPIOC, GPIO_Pin_13);						//��PC13��������Ϊ�ߵ�ƽ
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);						//��PC13��������Ϊ�͵�ƽ
}
