#include "sys.h"
#include "AHT10.h"

/*AHT10地址*/
#define AHT10_ADDRESS 0x38<<1 //从机地址是7位，最后一位是传输方向位，所以左移一位

/*设置使用哪一个I2C*/
#define I2Cx I2C1

/*
https://blog.zeruns.tech
*/


/*发送起始信号*/
void AHT10_I2C_START(){
    while( I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));//等待总线空闲
	I2C_GenerateSTART(I2Cx, ENABLE);//发送起始信号
	while( I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT)==ERROR);//检测EV5事件
}

/*发送停止信号*/
void AHT10_I2C_STOP(){
    I2C_GenerateSTOP(I2Cx, ENABLE);//发送停止信号
}

/**
  * @brief  发送3个字节数据
  * @param  cmd 命令字节
  * @param  DATA0 第0个参数
  * @param  DATA1 第1个参数
  * @retval 无
  */
void AHT10_WriteByte(uint8_t cmd, uint8_t DATA0, uint8_t DATA1)
{
	AHT10_I2C_START();  //发送起始信号
	
	I2C_Send7bitAddress(I2Cx, AHT10_ADDRESS, I2C_Direction_Transmitter);    //发送设备写地址
	while(I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)==ERROR);  //检测EV6事件

    I2C_SendData(I2Cx, cmd);//发送命令
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//检测EV8事件

    I2C_SendData(I2Cx, DATA0);//发送命令参数高8位数据
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//检测EV8事件
   
	I2C_SendData(I2Cx, DATA1);//发送命令参数低8位数据
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//检测EV8事件
	
	I2C_GenerateSTOP(I2Cx, ENABLE);//发送停止信号	
}


/**
  * @brief  发送命令读取AHT10的状态
  * @retval 读取到的状态字节
  */
/*uint8_t AHT10_ReadStatus(void){
    AHT10_I2C_START();//发送起始信号  
	I2C_Send7bitAddress(I2Cx,AHT10_ADDRESS,I2C_Direction_Receiver);//发送设备读地址
	while( I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED )==ERROR);//检测EV6事件
    while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED));//检测EV7事件
	I2C_AcknowledgeConfig(I2Cx, DISABLE); //关闭应答信号
	uint8_t status = I2C_ReceiveData(I2Cx);//读取数据并返回
	AHT10_I2C_STOP();   //发送停止信号
	I2C_AcknowledgeConfig(I2Cx,ENABLE);//重新开启应答信号
	return status;
}*/

/**
  * @brief  读取数据
  * @retval 读取到的字节数据
  */
uint8_t AHT10_ReadData(void)
{
    while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED));//检测EV7事件
	return I2C_ReceiveData(I2Cx);//读取数据并返回
}

/*软件复位AHT10*/
void AHT10_SoftReset(void)                    
{
    AHT10_I2C_START();  //发送起始信号
	I2C_Send7bitAddress(I2Cx, AHT10_ADDRESS, I2C_Direction_Transmitter);    //发送设备写地址
	while(I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)==ERROR);  //检测EV6事件
    I2C_SendData(I2Cx, 0xBA);//发送软复位命令
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//检测EV8事件
    I2C_GenerateSTOP(I2Cx, ENABLE);//发送停止信号
	//Delay_ms(20);
	delay_ms(20);
}


/*引脚初始化*/
void AHT10_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;      
	I2C_InitTypeDef  I2C_InitStructure;                 //定义结构体配置I2C
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);	//使能I2C1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOB时钟
 
	/*STM32F103芯片的硬件I2C1: PB6 -- SCL; PB7 -- SDA */         //定义结构体配置GPIO
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;		//设置输出模式为开漏输出，需接上拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);              //初始化GPIO
	
	I2C_DeInit(I2Cx);	//将外设I2C寄存器重设为缺省值
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;			//工作模式
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;	//时钟占空比，Tlow/Thigh = 2
	I2C_InitStructure.I2C_OwnAddress1 = 0x88;	//主机的I2C地址,用不到则随便写，无影响
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;	//使能应答位
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//设置地址长度7位
	I2C_InitStructure.I2C_ClockSpeed = 400000;	//I2C传输速度，400K，根据自己所用芯片手册查看支持的速度。	
	I2C_Init(I2Cx, &I2C_InitStructure);         //初始化I2C

	I2C_Cmd(I2Cx, ENABLE);  //启用I2C
	delay_ms(20);//上电延时
	AHT10_WriteByte(0XE1,0X08,0x00);//发送指令初始化
	delay_ms(20);

}

/**
  * @brief  读取AHT10数据
  * @param  *Hum 湿度
  * @param  *Temp 温度
  * @retval 1 - 读取成功；0 - 读取失败
  */
uint8_t ReadAHT10(float *Hum,float *Temp)
{
  uint8_t Data[5];//声明变量存放读取的数据
	uint8_t i;

	AHT10_WriteByte(0XAC,0X33,0x00);//发送指令触发测量

	delay_ms(70);	//延时70毫秒等待测量完成

    AHT10_I2C_START();//发送起始信号  
	I2C_Send7bitAddress(I2Cx,AHT10_ADDRESS,I2C_Direction_Receiver);//发送设备读地址
	while( I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED )==ERROR);//检测EV6事件
	
	for(i=0;i<6;i++)//循环6次读取6个字节数据
	{
		if (i == 5)	//读取最后1个字节时关闭应答信号
		{
			I2C_AcknowledgeConfig(I2Cx, DISABLE); //关闭应答信号
		}
		Data[i] = AHT10_ReadData();	//读取数据
		if (i == 5)
			I2C_GenerateSTOP(I2Cx, ENABLE); //发送停止信号
	}
	I2C_AcknowledgeConfig(I2Cx,ENABLE);//重新开启应答信号

	if( (Data[0]&0x08) == 0 )//0x08(00001000)检查状态字节第3位(校准使能位)是否为0
	{
		AHT10_WriteByte(0XE1,0X08,0x00);	//发送指令初始化
		delay_ms(20);
		return 0;
	}
	else if( (Data[0]&0x80) == 0 )//0x80(10000000)检查状态字节第7位(忙闲指示)是否为0
	{
		
		uint32_t SRH = (Data[1]<<12) | (Data[2]<<4) | (Data[3]>>4);	//湿度数据处理
		uint32_t ST = ((Data[3]&0x0f)<<16) | (Data[4]<<8) | Data[5];//温度数据处理

		*Hum = (SRH * 100.0) / 1024.0 / 1024;	   //根据手册给的公式转换湿度数据
		*Temp = (ST * 200.0) / 1024.0 / 1024 - 50; //根据手册给的公式转换温度数据

		return 1;
	}

	I2C_GenerateSTOP(I2Cx, ENABLE);//发送停止信号
	return 0;	

}

/*
https://blog.zeruns.tech
*/
