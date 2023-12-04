#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 


#define USART2_REC_LEN  			200  	//�����������ֽ��� 200
//#define USART2_INT_HANDLER  //���ж��ڲ�����, ע�͵������ڲ�����

//����2��ʼ��
void uart2_init(u32 bound);

//	��usart2�����ַ�����
//  ������ָ��ͳ���
void usart2_send_string(char* string, u16 length);

extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���
extern u8  Usart2_Receive_flag;//���ձ�־λ--���������ж���λ 
extern u16 Usart2_Receive_length;


typedef void (*Fun)(void);			
#ifdef USART2_INT_HANDLER
void Usart2_Int_Parse();//�жϴ�����
#else
void Usart2_Common_Parse(Fun Function);//��ͨ������
#endif

//����DMA���մ������� 
void uart2_rx_dma_init(DMA_Channel_TypeDef* DMA_CHx, u32 paddr, u32 maddr, u16 len);
#endif


