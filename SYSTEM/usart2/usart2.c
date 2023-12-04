#include "sys.h"
#include "usart2.h"	 
#include "string.h"

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 0
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
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 



//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8  USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 Usart2_Receive_length = 0;//���յ��ĳ��� 
u8  Usart2_Receive_flag = 0;//���ձ�־λ--���������ж���λ  
void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��USART1
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
  //USART2_RX	  GPIOA.3��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  

  //Usart2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������1

	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//�رմ��ڽ����ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//�������ڿ����ж�
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);//����DMA
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1
	uart2_rx_dma_init(DMA1_Channel6, (u32)&USART2->DR, (u32)USART2_RX_BUF, USART2_REC_LEN);

}
void USART2_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res = 0;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		
		USART2_RX_BUF[Usart2_Receive_length++] = USART_ReceiveData(USART2);
		  		 
   } 
	else if(USART_GetITStatus(USART2, USART_IT_IDLE ) != RESET){
		USART2->SR;
		USART2->DR;
		Usart2_Receive_flag = 1;
		DMA_Cmd(DMA1_Channel6, DISABLE);//��Ҫ��ʱ�ر�DMA1ͨ��6����ֹ�������������ǵ�		
		Usart2_Receive_flag = 1;//����λ��λ
		Usart2_Receive_length = USART2_REC_LEN-DMA_GetCurrDataCounter(DMA1_Channel6);//���յĸ���

#ifdef USART2_INT_HANDLER//�������жϴ���,���ж��н������ݵĴ���
		Usart2_Int_Parse();//�жϴ�����
#endif

	}
	else{
	
	}
}
/*
*����1�����жϳ����е����ݴ�����
*���ж���ִ��
*/
#ifdef USART2_INT_HANDLER
void Usart2_Int_Parse(Fun Function){
	//-----------------------
	
	//-----------------------
	Usart2_Receive_flag = 0;	
	memset(USART2_RX_BUF,0,USART2_REC_LEN);//��ջ�����
	DMA_SetCurrDataCounter(DMA1_Channel6, USART2_REC_LEN);//��Ҫ��������DMA1ͨ��5�������ݴ�С
  DMA_Cmd(DMA1_Channel6, ENABLE);	//����DMA
}
#endif
/*
*����������ͨ����ʽ
*�����λ�������˽��տ����жϵı��λ���лص�����
*/
#ifndef USART2_INT_HANDLER
void Usart2_Common_Parse(Fun Function){
	if(Usart2_Receive_flag){
	Function();//�ص����д���
	
	Usart2_Receive_flag = 0;	
	memset(USART2_RX_BUF,0,USART2_REC_LEN);//��ջ�����
	DMA_SetCurrDataCounter(DMA1_Channel6, USART2_REC_LEN);//��Ҫ��������DMA1ͨ��5�������ݴ�С
  DMA_Cmd(DMA1_Channel6, ENABLE);	//����DMA		
	}
}
#endif


/*
	��usart2�����ַ�����
  ������ָ��ͳ���
*/
void usart2_send_string(char* string, u16 length){
	u16 count = 0;
	for(count=0; count<length; count++){
		while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
    USART2->DR = (u8) string[count];  
	}
}

//����DMA���մ������� 
void uart2_rx_dma_init(DMA_Channel_TypeDef* DMA_CHx, u32 paddr, u32 maddr, u16 len){
      DMA_InitTypeDef   dma_init_struct;

      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
      DMA_DeInit(DMA_CHx);
      
      dma_init_struct.DMA_PeripheralBaseAddr = paddr;
      dma_init_struct.DMA_MemoryBaseAddr = maddr;
      dma_init_struct.DMA_DIR = DMA_DIR_PeripheralSRC;
      dma_init_struct.DMA_BufferSize  = len;
      dma_init_struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
      dma_init_struct.DMA_MemoryInc = DMA_MemoryInc_Enable;
      dma_init_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
      dma_init_struct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
      dma_init_struct.DMA_Mode = DMA_Mode_Normal;
      dma_init_struct.DMA_Priority = DMA_Priority_VeryHigh;
      dma_init_struct.DMA_M2M = DMA_M2M_Disable;
      DMA_Init(DMA_CHx, &dma_init_struct);
      DMA_Cmd(DMA_CHx, ENABLE);  
}
