#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART3_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
/*
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 
    USART_SendData(USART2,(uint8_t)ch);    
	return ch;
}
*/
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
u16 USART1_RX_LENGTH;
u8 USART1_RX_BUFF[200];

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

void uart_init(u32 bound){
    //GPIO�˿�����
   GPIO_InitTypeDef GPIO_InitStructure;				   //IO�����ýṹ��
	USART_InitTypeDef USART_InitStructure;				   //�������ýṹ��
	NVIC_InitTypeDef NVIC_InitStructure;				   //�ж����ýṹ��
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);          //��λ����1  �ڿ�������ǰ ��ö�Ҫ��λһ������
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������	 PA.9ѡ����״̬���ܽ��봮��ģʽ
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10




   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;                //���������� һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;        //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //�շ�ģʽ   ����������ý��� �����
		
    USART_Init(USART1, &USART_InitStructure); //��ʼ������
		
		

#if EN_USART1_RX		  //���ʹ���˽���  
   //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�	 ���յ����ݽ����ж�
	 

#endif

    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}




void Uart1_SendStr(char* SendBuff)
{
	while(*SendBuff)
	{
		while((USART1->SR & 0x40) == 0);
		USART1->DR = (u8) *SendBuff;
		SendBuff ++;
	}
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{
		USART1_RX_BUFF[USART1_RX_LENGTH ++] = USART1->DR;
	}
}



void USART2_Configuration(void)
{
	
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	/* Configure USART2 Rx (PA.3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		  //
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART2 Tx (PA.2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;						//
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//
	USART_InitStructure.USART_Parity = USART_Parity_No;				//
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//
	/* Configure USART2 */
	USART_Init(USART2, &USART_InitStructure);							//
	/* Enable USART2 Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                    //
	//USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//
	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);	                  //
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	   	//
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		   //
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//
	NVIC_Init(&NVIC_InitStructure);
	
	
	
}

// ��Ч�������ͺ���
void UART_SendStr(char* str)
{
    while(*str)
    {
        // �ȴ����ͼĴ���Ϊ��
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        
        USART_SendData(USART2, (uint8_t)(*str));
        str++;
    }
    
    // ȷ�����һ���ַ��ѷ���
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

 /* ����  ���ض���c�⺯��printf��USART1*/ 
int fputc(int ch, FILE *f)
{
/* ��Printf���ݷ������� */
  USART_SendData(USART2, (unsigned char) ch);
  while (!(USART2->SR & USART_FLAG_TXE));
 
  return (ch);
}
#endif	

