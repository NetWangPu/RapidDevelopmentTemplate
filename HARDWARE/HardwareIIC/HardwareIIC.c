#include "HardwareIIC.h"

/// @brief  硬件I2C初始化
/// @param   无
/// @return  无
void Hardware_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  // 使能I2C1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能GPIOB时钟

    /*STM32F103芯片的硬件I2C: PB6 -- SCL; PB7 -- SDA */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 设置输出模式为开漏输出，需接上拉电阻
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_DeInit(I2Cx);                                                         // 将外设I2C寄存器重设为缺省值
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                // 工作模式
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                        // 时钟占空比，Tlow/Thigh = 2
    I2C_InitStructure.I2C_OwnAddress1 = 0x88;                                 // 主机的I2C地址,用不到则随便写，无影响
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                               // 使能应答位
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 设置地址长度7位
    I2C_InitStructure.I2C_ClockSpeed = 400000;                                // I2C传输速度，400K，根据自己所用芯片手册查看支持的速度。
    I2C_Init(I2Cx, &I2C_InitStructure);

    I2C_Cmd(I2Cx, ENABLE);
}

/// @brief  硬件I2C写入一个字节
/// @param addr  要操作设备内部的地址
/// @param data  要写入的数据
/// @param slave_address 从机地址
void I2C_WriteByte(uint8_t addr, uint8_t data, uint8_t slave_address)
{
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // 检测总线是否空闲
        ;
    I2C_GenerateSTART(I2Cx, ENABLE);                                    // 发送起始信号
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) // 检查是否成功生成起始信号，确保已经进入主模式
        ;
    I2C_Send7bitAddress(I2Cx, slave_address, I2C_Direction_Transmitter);              // 发送从机地址
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) // 检查从机地址发送是否成功，确保从机已经进入从模式
        ;
    I2C_SendData(I2Cx, addr);                                        // 发送要操作设备内部的地址
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 检查地址发送是否成功
        ;
    I2C_SendData(I2Cx, data);                                        // 发送数据
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 检查数据发送是否成功
        ;
    I2C_GenerateSTOP(I2Cx, ENABLE); // 发送停止信号
}

/// @brief  硬件I2C读取一个字节
/// @param addr  要操作设备内部的地址
/// @param slave_address 从机地址
/// @return 读取到的数据
uint8_t I2C_ReadByte(uint8_t addr, uint8_t slave_address)
{
    uint8_t data;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // 检测总线是否空闲
        ;
    I2C_GenerateSTART(I2Cx, ENABLE);                                    // 发送起始信号
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) // 检查是否成功生成起始信号，确保已经进入主模式
        ;
    I2C_Send7bitAddress(I2Cx, slave_address, I2C_Direction_Transmitter);              // 发送从机地址
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) // 检查从机地址发送是否成功，确保从机已经进入从模式
        ;
    I2C_SendData(I2Cx, addr);                                        // 发送要操作设备内部的地址
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 检查地址发送是否成功
        ;
    I2C_GenerateSTART(I2Cx, ENABLE);                                   // 重新发送起始信号
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) // 检查是否成功生成起始信号，确保已经进入主模式
        ;
    I2C_Send7bitAddress(I2Cx, slave_address, I2C_Direction_Receiver);               // 发送从机地址
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR) // 检查从机地址发送是否成功，确保从机已经进入从模式
        ;
    I2C_AcknowledgeConfig(I2Cx, DISABLE); // 关闭应答位
    I2C_GenerateSTOP(I2Cx, ENABLE);       // 发送停止信号
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) // 检查数据接收是否成功
        ;
    data = I2C_ReceiveData(I2Cx); // 读取数据
    return data;
}   