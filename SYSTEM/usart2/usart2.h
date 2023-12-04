#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 


#define USART2_REC_LEN  			200  	//定义最大接收字节数 200
//#define USART2_INT_HANDLER  //在中断内部处理, 注释掉不在内部处理

//串口2初始化
void uart2_init(u32 bound);

//	向usart2发送字符数组
//  参数，指针和长度
void usart2_send_string(char* string, u16 length);

extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲
extern u8  Usart2_Receive_flag;//接收标志位--产生空闲中断置位 
extern u16 Usart2_Receive_length;


typedef void (*Fun)(void);			
#ifdef USART2_INT_HANDLER
void Usart2_Int_Parse();//中断处理函数
#else
void Usart2_Common_Parse(Fun Function);//普通处理函数
#endif

//设置DMA接收串口数据 
void uart2_rx_dma_init(DMA_Channel_TypeDef* DMA_CHx, u32 paddr, u32 maddr, u16 len);
#endif


