#ifndef __USART1_H
#define __USART1_H
#include "stdio.h"	
#include "sys.h" 


#define USART1_REC_LEN  			300  	//�����������ֽ��� 200
//#define USART1_INT_HANDLER  //���ж��ڲ�����, ע�͵������ڲ�����
	  	
void uart1_init(u32 bound);			
			
/*
	��usart1�����ַ�����
  ������ָ��ͳ���
*/
void usart1_send_string(char* string, u16 length);			
			
typedef void (*Fun)(void);			
#ifdef USART1_INT_HANDLER
void Usart1_Int_Parse();//�жϴ�����
#else
void Usart1_Common_Parse(Fun Function);//��ͨ������
#endif
	
extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���
extern u8  Usart1_Receive_flag;//���ձ�־λ--���������ж���λ 
extern u16 Usart1_Receive_length;

//����DMA���մ������� 
void uart1_rx_dma_init(DMA_Channel_TypeDef* DMA_CHx, u32 paddr, u32 maddr, u16 len);
#endif


