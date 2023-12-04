#ifndef __USART1_H
#define __USART1_H
#include "stdio.h"	
#include "sys.h" 


#define USART1_REC_LEN  			300  	//定义最大接收字节数 200
//#define USART1_INT_HANDLER  //在中断内部处理, 注释掉不在内部处理
	  	
void uart1_init(u32 bound);			
			
/*
	向usart1发送字符数组
  参数，指针和长度
*/
void usart1_send_string(char* string, u16 length);			
			
typedef void (*Fun)(void);			
#ifdef USART1_INT_HANDLER
void Usart1_Int_Parse();//中断处理函数
#else
void Usart1_Common_Parse(Fun Function);//普通处理函数
#endif
	
extern u8  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲
extern u8  Usart1_Receive_flag;//接收标志位--产生空闲中断置位 
extern u16 Usart1_Receive_length;

//设置DMA接收串口数据 
void uart1_rx_dma_init(DMA_Channel_TypeDef* DMA_CHx, u32 paddr, u32 maddr, u16 len);
#endif


