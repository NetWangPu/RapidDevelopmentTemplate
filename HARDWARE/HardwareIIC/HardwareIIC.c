#include "HardwareIIC.h"

/// @brief  Ӳ��I2C��ʼ��
/// @param   ��
/// @return  ��
void Hardware_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  // ʹ��I2C1ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // ʹ��GPIOBʱ��

    /*STM32F103оƬ��Ӳ��I2C: PB6 -- SCL; PB7 -- SDA */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // �������ģʽΪ��©����������������
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_DeInit(I2Cx);                                                         // ������I2C�Ĵ�������Ϊȱʡֵ
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                // ����ģʽ
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                        // ʱ��ռ�ձȣ�Tlow/Thigh = 2
    I2C_InitStructure.I2C_OwnAddress1 = 0x88;                                 // ������I2C��ַ,�ò��������д����Ӱ��
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                               // ʹ��Ӧ��λ
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // ���õ�ַ����7λ
    I2C_InitStructure.I2C_ClockSpeed = 400000;                                // I2C�����ٶȣ�400K�������Լ�����оƬ�ֲ�鿴֧�ֵ��ٶȡ�
    I2C_Init(I2Cx, &I2C_InitStructure);

    I2C_Cmd(I2Cx, ENABLE);
}

/// @brief  Ӳ��I2Cд��һ���ֽ�
/// @param addr  Ҫ�����豸�ڲ��ĵ�ַ
/// @param data  Ҫд�������
/// @param slave_address �ӻ���ַ
void I2C_WriteByte(uint8_t addr, uint8_t data, uint8_t slave_address)
{
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // ��������Ƿ����
        ;
    I2C_GenerateSTART(I2Cx, ENABLE);                                    // ������ʼ�ź�
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) // ����Ƿ�ɹ�������ʼ�źţ�ȷ���Ѿ�������ģʽ
        ;
    I2C_Send7bitAddress(I2Cx, slave_address, I2C_Direction_Transmitter);              // ���ʹӻ���ַ
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) // ���ӻ���ַ�����Ƿ�ɹ���ȷ���ӻ��Ѿ������ģʽ
        ;
    I2C_SendData(I2Cx, addr);                                        // ����Ҫ�����豸�ڲ��ĵ�ַ
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // ����ַ�����Ƿ�ɹ�
        ;
    I2C_SendData(I2Cx, data);                                        // ��������
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // ������ݷ����Ƿ�ɹ�
        ;
    I2C_GenerateSTOP(I2Cx, ENABLE); // ����ֹͣ�ź�
}

/// @brief  Ӳ��I2C��ȡһ���ֽ�
/// @param addr  Ҫ�����豸�ڲ��ĵ�ַ
/// @param slave_address �ӻ���ַ
/// @return ��ȡ��������
uint8_t I2C_ReadByte(uint8_t addr, uint8_t slave_address)
{
    uint8_t data;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // ��������Ƿ����
        ;
    I2C_GenerateSTART(I2Cx, ENABLE);                                    // ������ʼ�ź�
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) // ����Ƿ�ɹ�������ʼ�źţ�ȷ���Ѿ�������ģʽ
        ;
    I2C_Send7bitAddress(I2Cx, slave_address, I2C_Direction_Transmitter);              // ���ʹӻ���ַ
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) // ���ӻ���ַ�����Ƿ�ɹ���ȷ���ӻ��Ѿ������ģʽ
        ;
    I2C_SendData(I2Cx, addr);                                        // ����Ҫ�����豸�ڲ��ĵ�ַ
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // ����ַ�����Ƿ�ɹ�
        ;
    I2C_GenerateSTART(I2Cx, ENABLE);                                   // ���·�����ʼ�ź�
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) // ����Ƿ�ɹ�������ʼ�źţ�ȷ���Ѿ�������ģʽ
        ;
    I2C_Send7bitAddress(I2Cx, slave_address, I2C_Direction_Receiver);               // ���ʹӻ���ַ
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR) // ���ӻ���ַ�����Ƿ�ɹ���ȷ���ӻ��Ѿ������ģʽ
        ;
    I2C_AcknowledgeConfig(I2Cx, DISABLE); // �ر�Ӧ��λ
    I2C_GenerateSTOP(I2Cx, ENABLE);       // ����ֹͣ�ź�
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) // ������ݽ����Ƿ�ɹ�
        ;
    data = I2C_ReceiveData(I2Cx); // ��ȡ����
    return data;
}   