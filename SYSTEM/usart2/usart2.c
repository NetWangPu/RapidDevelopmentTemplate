#include "sys.h"
#include "usart2.h"	 
#include "string.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 0
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 



//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8  USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 Usart2_Receive_length = 0;//接收到的长度 
u8  Usart2_Receive_flag = 0;//接收标志位--产生空闲中断置位  
void uart2_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能USART1
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
  //USART2_RX	  GPIOA.3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  

  //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口1

	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//关闭串口接受中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//开启串口空闲中断
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);//开启DMA
  USART_Cmd(USART2, ENABLE);                    //使能串口1
	uart2_rx_dma_init(DMA1_Channel6, (u32)&USART2->DR, (u32)USART2_RX_BUF, USART2_REC_LEN);

}
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res = 0;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		
		USART2_RX_BUF[Usart2_Receive_length++] = USART_ReceiveData(USART2);
		  		 
   } 
	else if(USART_GetITStatus(USART2, USART_IT_IDLE ) != RESET){
		USART2->SR;
		USART2->DR;
		Usart2_Receive_flag = 1;
		DMA_Cmd(DMA1_Channel6, DISABLE);//需要暂时关闭DMA1通道6，防止又有数据来覆盖掉		
		Usart2_Receive_flag = 1;//接受位置位
		Usart2_Receive_length = USART2_REC_LEN-DMA_GetCurrDataCounter(DMA1_Channel6);//接收的个数

#ifdef USART2_INT_HANDLER//开启了中断处理,在中断中进行数据的处理
		Usart2_Int_Parse();//中断处理函数
#endif

	}
	else{
	
	}
}
/*
*串口1空闲中断程序中的数据处理函数
*在中断中执行
*/
#ifdef USART2_INT_HANDLER
void Usart2_Int_Parse(Fun Function){
	//-----------------------
	
	//-----------------------
	Usart2_Receive_flag = 0;	
	memset(USART2_RX_BUF,0,USART2_REC_LEN);//清空缓冲区
	DMA_SetCurrDataCounter(DMA1_Channel6, USART2_REC_LEN);//需要重新配置DMA1通道5缓存数据大小
  DMA_Cmd(DMA1_Channel6, ENABLE);	//开启DMA
}
#endif
/*
*串口数据普通处理方式
*检测标记位，产生了接收空闲中断的标记位进行回调处理
*/
#ifndef USART2_INT_HANDLER
void Usart2_Common_Parse(Fun Function){
	if(Usart2_Receive_flag){
	Function();//回调进行处理
	
	Usart2_Receive_flag = 0;	
	memset(USART2_RX_BUF,0,USART2_REC_LEN);//清空缓冲区
	DMA_SetCurrDataCounter(DMA1_Channel6, USART2_REC_LEN);//需要重新配置DMA1通道5缓存数据大小
  DMA_Cmd(DMA1_Channel6, ENABLE);	//开启DMA		
	}
}
#endif


/*
	向usart2发送字符数组
  参数，指针和长度
*/
void usart2_send_string(char* string, u16 length){
	u16 count = 0;
	for(count=0; count<length; count++){
		while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) string[count];  
	}
}

//设置DMA接收串口数据 
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
