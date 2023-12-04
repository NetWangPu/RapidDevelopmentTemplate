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

//���ֿڳ�ʼ��
#define Fire_Dig PBin(0)
#define Smog_Dig PBin(1)
void Dig_Init(){
//B0 -> Fire �ߵ�ƽ��Ч
//B1 -> Smog �͵�ƽ��Ч
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOE4,3

}

//��������ֵ��ѹ
float Fire_Range = 0.0;//������ֵ  
float Smog_Range = 0.0;//������ֵ

//��ʼ����������ֵ
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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	OLED_Init(); //OLED��ʼ��
	AHT10_Init();//��ʪ�ȳ�ʼ��
	Adc_Init();  //����,����������ʼ��
	uart1_init(115200);	 //���ڳ�ʼ��Ϊ115200
	uart2_init(115200);	 //���ڳ�ʼ��Ϊ115200
	LED_Init();
	Fire_Range_Init();
	Dig_Init();
	while(1){
	 //�����ʪ������
	ReadAHT10(&Hum, &Temp);	
	memset(str,0,16);
	sprintf(str,"ʪ��:%0.2f%%",Hum);
	LCD_Print(0, 0, str, TYPE16X16,TYPE8X16);//��ʾʪ��
	
	memset(str,0,16);
	sprintf(str,"�¶�:%0.2f��C",Temp);
	LCD_Print(0, 16, str, TYPE16X16,TYPE8X16);//��ʾ�¶�
	
	//��û���-A0
	memset(str,32,16);
	//�ж��Ƿ��л���
		
	if((float)Get_Adc(0)/4096*3.3>Fire_Range){//����
			sprintf(str,"����:%d ",4096-Get_Adc(0));		
			//LED = 1;
	}else{
			sprintf(str,"����:%d ",4096-Get_Adc(0));	
			//LED = 0;
	}
		
	str[15] = 0;
	//str[strlen(str)] = 32;
	LCD_Print(0, 32, str, TYPE16X16,TYPE8X16);//��ʾ����

	//�������-A1
	memset(str,32,16);
	sprintf(str,"����:%d", Get_Adc(1));
	str[14] = 0;
	str[strlen(str)] = 32;
	LCD_Print(0, 48, str, TYPE16X16,TYPE8X16);//��ʾ����
	
	//printf("Hello world\r\n");
	Usart1_Common_Parse(main_usart1_fun);
	
	//��ⱨ��
	if(Fire_Dig || !Smog_Dig){
		LED = 0;
	}else{
	  LED = 1 ;
	}
	//===============================
	//���͸��ƶ�����
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


//��ʼ����������ֵ
void Fire_Range_Init(){
	float Fire[16] = {0};	
	u8 count = 0;
	float min = 3.3;
	//�����ɼ�16��
	for(count=0; count<16; count++){
		Fire[count] = (float)Get_Adc(0)/4096*3.3;
		delay_ms(10);
	}
		
	//ȡ��С�����ݵ�0.8����Ϊ��ֵ
		for(count=0; count<16; count++)
			min = min>Fire[count]? Fire[count]:min;
		Fire_Range = min*0.8;

}
void Smog_Range_Init(){


}




















