#include "timer.h"
#include "usart3.h"

u8 key_num = 0;
extern vu16 USART3_RX_STA;


/**********************************************************************
* @Function      ：void TIM3_Int_Init(u16 arr,u16 psc)
* @Features      ：通用定时器3中断初始化
* @InportFunction：arr：自动重装值。psc：时钟预分频数
* @ExportFunction：No
* @Author        ：TanXiaoDong
* @ModifiedDate  ：2020.12.24
* @Remarks       ：这里时钟选择为APB1的2倍，而APB1为36M
**********************************************************************/
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                 //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr;                              //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;                            //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;              //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;          //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);                      //根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );                            //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                      //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;            //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;                   //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                      //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                                      //初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);                                               //使能TIMx					 
}

/**********************************************************************
* @Function      ：void TIM4_Int_Init(u16 arr,u16 psc)
* @Features      ：TIM4中断服务程序
* @InportFunction：No
* @ExportFunction：No
* @Author        ：TanXiaoDong
* @ModifiedDate  ：2020.12.24
* @Remarks       ：这里时钟选择为APB1的2倍，而APB1为36M
**********************************************************************/
void TIM4_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//TIM4时钟使能    
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断
	
	TIM_Cmd(TIM4,ENABLE);//开启定时器4
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}
/**********************************************************************
* @Function      ：void TIM3_IRQHandler(void)   //TIM3中断
* @Features      ：TIM3中断服务程序
* @InportFunction：No
* @ExportFunction：No
* @Author        ：TanXiaoDong
* @ModifiedDate  ：2020.12.24
* @Remarks       ：这里时钟选择为APB1的2倍，而APB1为36M
**********************************************************************/
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{	
		  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     //清除TIMx更新中断标志	 
		}
}

/**********************************************************************
* @Function      ：void TIM4_IRQHandler(void)   //TIM3中断
* @Features      ：TIM4中断服务程序
* @InportFunction：No
* @ExportFunction：No
* @Author        ：TanXiaoDong
* @ModifiedDate  ：2020.12.24
* @Remarks       ：这里时钟选择为APB1的2倍，而APB1为36M
**********************************************************************/    
void TIM4_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		USART3_RX_STA |= 1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TI4更新中断标志    
		TIM_Cmd(TIM4, DISABLE);  //关闭TIM4
	}	    
}











