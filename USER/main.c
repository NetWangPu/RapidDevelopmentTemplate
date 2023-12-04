#include "delay.h"
#include "usart1.h"
#include "sys.h"
#include "stdio.h"
#include "string.h"
#include "usart1.h"
#include "usart2.h"
#include "OLED.h"
#include "AHT10.h"
#include "soil.h"
#include "led.h"

//数字口初始化
#define Fire_Dig PBin(0)
#define Smog_Dig PBin(1)
void Dig_Init(){
//B0 -> Fire 高电平有效
//B1 -> Smog 低电平有效
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOE4,3

}

//传感器阈值电压
float Fire_Range = 0.0;//火焰阈值  
float Smog_Range = 0.0;//烟雾阈值

//初始化环境量阈值
void Fire_Range_Init();
void Smog_Range_Init();


void main_usart1_fun();
void main_usart2_fun();

 int main(void){
 	u16 t=0;  
	u16 len;	
	u16 times=0;

	float Hum=0;
	float Temp=0; 
	u8 str[16] = {0};
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	OLED_Init(); //OLED初始化
	AHT10_Init();//温湿度初始化
	Adc_Init();  //火焰,烟雾传感器初始化
	uart1_init(115200);	 //串口初始化为115200
	uart2_init(115200);	 //串口初始化为115200
	LED_Init();
	Fire_Range_Init();
	Dig_Init();
	while(1){
	 //获得温湿度数据
	ReadAHT10(&Hum, &Temp);	
	memset(str,0,16);
	sprintf(str,"湿度:%0.2f%%",Hum);
	LCD_Print(0, 0, str, TYPE16X16,TYPE8X16);//显示湿度
	
	memset(str,0,16);
	sprintf(str,"温度:%0.2f°C",Temp);
	LCD_Print(0, 16, str, TYPE16X16,TYPE8X16);//显示温度
	
	//获得火焰-A0
	memset(str,32,16);
	//判断是否有火焰
		
	if((float)Get_Adc(0)/4096*3.3>Fire_Range){//正常
			sprintf(str,"火焰:%d ",4096-Get_Adc(0));		
			//LED = 1;
	}else{
			sprintf(str,"火焰:%d ",4096-Get_Adc(0));	
			//LED = 0;
	}
		
	str[15] = 0;
	//str[strlen(str)] = 32;
	LCD_Print(0, 32, str, TYPE16X16,TYPE8X16);//显示火焰

	//获得烟雾-A1
	memset(str,32,16);
	sprintf(str,"烟雾:%d", Get_Adc(1));
	str[14] = 0;
	str[strlen(str)] = 32;
	LCD_Print(0, 48, str, TYPE16X16,TYPE8X16);//显示烟雾
	
	//printf("Hello world\r\n");
	Usart1_Common_Parse(main_usart1_fun);
	
	//检测报警
	if(Fire_Dig || !Smog_Dig){
		LED = 0;
	}else{
	  LED = 1 ;
	}
	//===============================
	//发送给云端数据
	if(++t == 10){
		t = 0;
		printf("{\"temperature\": %0.2f, \"humidity\": %0.2f, \"smoke\": %d, \"flame\": %d}",Temp,Hum,Get_Adc(1),4096-Get_Adc(0));
		}
	}	 
	 
 	while(1)
	{	
		Usart2_Common_Parse(main_usart2_fun);
		Usart1_Common_Parse(main_usart1_fun);
	//	delay_ms(100);
	}	 
 }

void main_usart1_fun(){
	//printf("%s\r\n", USART1_RX_BUF);
	usart1_send_string(USART1_RX_BUF,strlen(USART1_RX_BUF));
}

void main_usart2_fun(){
	usart1_send_string(USART2_RX_BUF,strlen(USART2_RX_BUF));
	//printf("%s\r\n", USART2_RX_BUF);
}
//======================================================================


//初始化环境量阈值
void Fire_Range_Init(){
	float Fire[16] = {0};	
	u8 count = 0;
	float min = 3.3;
	//连续采集16次
	for(count=0; count<16; count++){
		Fire[count] = (float)Get_Adc(0)/4096*3.3;
		delay_ms(10);
	}
		
	//取最小的数据的0.8倍作为阈值
		for(count=0; count<16; count++)
			min = min>Fire[count]? Fire[count]:min;
		Fire_Range = min*0.8;

}
void Smog_Range_Init(){


}




















