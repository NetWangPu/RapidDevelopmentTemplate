#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "usart3.h"
#include "as608.h"


u32 AS608Addr = 0XFFFFFFFF; //Ĭ��
char str2[6] = {0};

/// @brief �������ݵ�ģ��
/// @param data 
static void MYUSART_SendData(u8 data)
{
  while((USART3->SR & 0X40) == 0);
  USART3->DR = data;
}

/// @brief ����ͷ
/// @param  ��
static void SendHead(void)
{
  MYUSART_SendData(0xEF);
  MYUSART_SendData(0x01);
}

/// @brief ���͵�ַ
/// @param  ��
static void SendAddr(void)
{
  MYUSART_SendData(AS608Addr >> 24);
  MYUSART_SendData(AS608Addr >> 16);
  MYUSART_SendData(AS608Addr >> 8);
  MYUSART_SendData(AS608Addr);
}

/// @brief  ���Ͱ���ʶ
/// @param flag ��ʶ 
static void SendFlag(u8 flag)
{
  MYUSART_SendData(flag);
}

/// @brief  ���Ͱ�����
/// @param length ���� 
static void SendLength(int length)
{
  MYUSART_SendData(length >> 8);
  MYUSART_SendData(length);
}

/// @brief  ����ָ����
/// @param cmd ָ���� 
static void Sendcmd(u8 cmd)
{
  MYUSART_SendData(cmd);
}

/// @brief  ����У���
/// @param check У��� 
static void SendCheck(u16 check)
{
  MYUSART_SendData(check >> 8);
  MYUSART_SendData(check);
}

/// @brief  �ж��жϽ��յ�������û��Ӧ���
/// @param waittime �ȴ��жϽ������ݵ�ʱ�䣨��λ1ms�� 
/// @return ���ݰ��׵�ַ
static u8 *JudgeStr(u16 waittime)
{
  char *data;
  u8 str[8];
  str[0] = 0xef;
  str[1] = 0x01;
  str[2] = AS608Addr >> 24;
  str[3] = AS608Addr >> 16;
  str[4] = AS608Addr >> 8;
  str[5] = AS608Addr;
  str[6] = 0x07;
  str[7] = '\0';
  USART3_RX_STA = 0;
  while(--waittime)
  {
    delay_ms(1);
    if(USART3_RX_STA & 0X8000) //���յ�һ������
    {
      USART3_RX_STA = 0;
      data = strstr((const char*)USART3_RX_BUF, (const char*)str);
      if(data)
        return (u8*)data;
    }
  }
  return 0;
}

/// @brief  ¼��ͼ��
/// @param  ��
/// @return  ģ�鷵��ȷ����
/// @note  ̽����ָ��̽�⵽��¼��ָ��ͼ�����ImageBuffer��
u8 PS_GetImage(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x01);
  temp =  0x01 + 0x03 + 0x01;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_GenChar(u8 BufferID)
* @Features      ����ImageBuffer�е�ԭʼͼ������ָ�������ļ�����CharBuffer1��CharBuffer2
* @InportFunction��BufferID --> charBuffer1:0x01	charBuffer1:0x02           
* @ExportFunction��ģ�鷵��ȷ����
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ���������� PS_GenChar
**********************************************************************/
u8 PS_GenChar(u8 BufferID)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x04);
  Sendcmd(0x02);
  MYUSART_SendData(BufferID);
  temp = 0x01 + 0x04 + 0x02 + BufferID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}


/**********************************************************************
* @Function      ��u8 PS_Match(void)
* @Features      ����ȷ�ȶ�CharBuffer1 ��CharBuffer2 �е������ļ�
* @InportFunction��BufferID --> charBuffer1:0x01	charBuffer1:0x02           
* @ExportFunction��ģ�鷵��ȷ����
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ����ȷ�ȶ���öָ������ PS_Match
**********************************************************************/
u8 PS_Match(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x03);
  temp = 0x01 + 0x03 + 0x03;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_Search(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
* @Features      ����CharBuffer1��CharBuffer2�е������ļ����������򲿷�ָ�ƿ�.�����������򷵻�ҳ�롣
* @InportFunction��BufferID @ref CharBuffer1	CharBuffer2           
* @ExportFunction��ģ�鷵��ȷ���֣�ҳ�루����ָ��ģ�壩
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ������ָ�� PS_Searchh
**********************************************************************/
u8 PS_Search(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x08);
  Sendcmd(0x04);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(StartPage >> 8);
  MYUSART_SendData(StartPage);
  MYUSART_SendData(PageNum >> 8);
  MYUSART_SendData(PageNum);
  temp = 0x01 + 0x08 + 0x04 + BufferID
         + (StartPage >> 8) + (u8)StartPage
         + (PageNum >> 8) + (u8)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    p->pageID   = (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  }
  else
    ensure = 0xff;
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_RegModel(void)
* @Features      ����CharBuffer1��CharBuffer2�е������ļ��ϲ����� ģ��,�������CharBuffer1��CharBuffer2
* @InportFunction��No        
* @ExportFunction��ģ�鷵��ȷ����
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ���ϲ�����������ģ�壩PS_RegModel
**********************************************************************/
u8 PS_RegModel(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x05);
  temp = 0x01 + 0x03 + 0x05;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_StoreChar(u8 BufferID, u16 PageID)
* @Features      ����CharBuffer1��CharBuffer2�е������ļ��ϲ����� ģ��,�������CharBuffer1��CharBuffer2
* @InportFunction��BufferID @ref charBuffer1:0x01	
                   charBuffer1:0x02
                   PageID��ָ�ƿ�λ�úţ�     
* @ExportFunction��ģ�鷵��ȷ����
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ������ģ�� PS_StoreChar
**********************************************************************/
u8 PS_StoreChar(u8 BufferID, u16 PageID)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x06);
  Sendcmd(0x06);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(PageID >> 8);
  MYUSART_SendData(PageID);
  temp = 0x01 + 0x06 + 0x06 + BufferID
         + (PageID >> 8) + (u8)PageID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_DeletChar(u16 PageID, u16 N)
* @Features      ��ɾ��flash���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ��
* @InportFunction��PageID(ָ�ƿ�ģ���)��Nɾ����ģ�������     
* @ExportFunction��ģ�鷵��ȷ����
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ��ɾ��ģ�� PS_DeletCharr
**********************************************************************/
u8 PS_DeletChar(u16 PageID, u16 N)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x07);
  Sendcmd(0x0C);
  MYUSART_SendData(PageID >> 8);
  MYUSART_SendData(PageID);
  MYUSART_SendData(N >> 8);
  MYUSART_SendData(N);
  temp = 0x01 + 0x07 + 0x0C
         + (PageID >> 8) + (u8)PageID
         + (N >> 8) + (u8)N;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_Empty(void)
* @Features      ��ɾ��flash���ݿ�������ָ��ģ��
* @InportFunction��No     
* @ExportFunction��ģ�鷵��ȷ����
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       �����ָ�ƿ� PS_Empty
**********************************************************************/
u8 PS_Empty(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x0D);
  temp = 0x01 + 0x03 + 0x0D;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_WriteReg(u8 RegNum, u8 DATA)
* @Features      ��дģ��Ĵ���
* @InportFunction���Ĵ������RegNum:4\5\6
* @ExportFunction��ģ�鷵��ȷ����
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ��дϵͳ�Ĵ��� PS_WriteReg
**********************************************************************/
u8 PS_WriteReg(u8 RegNum, u8 DATA)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x05);
  Sendcmd(0x0E);
  MYUSART_SendData(RegNum);
  MYUSART_SendData(DATA);
  temp = RegNum + DATA + 0x01 + 0x05 + 0x0E;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  if(ensure == 0)
    printf("\r\n���ò����ɹ���");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_ReadSysPara(SysPara *p)
* @Features      ����ȡģ��Ļ��������������ʣ�����С��)
* @InportFunction��No
* @ExportFunction��ģ�鷵��ȷ���� + ����������16bytes��
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ����ϵͳ�������� PS_ReadSysPara
**********************************************************************/
u8 PS_ReadSysPara(SysPara *p)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x0F);
  temp = 0x01 + 0x03 + 0x0F;
  SendCheck(temp);
  data = JudgeStr(1000);
  if(data)
  {
    ensure = data[9];
    p->PS_max = (data[14] << 8) + data[15];
    p->PS_level = data[17];
    p->PS_addr = (data[18] << 24) + (data[19] << 16) + (data[20] << 8) + data[21];
    p->PS_size = data[23];
    p->PS_N = data[25];
  }
  else
    ensure = 0xff;
  if(ensure == 0x00)
  {
    printf("\r\nģ�����ָ������=%d", p->PS_max);
    printf("\r\n�Աȵȼ�=%d", p->PS_level);
    printf("\r\n��ַ=%x", p->PS_addr);
    printf("\r\n������=%d", p->PS_N * 9600);
  }
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_SetAddr(u32 PS_addr)
* @Features      ������ģ���ַ
* @InportFunction��PS_addr
* @ExportFunction��ģ�鷵��ȷ����
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ������ģ���ַ PS_SetAddr
**********************************************************************/
u8 PS_SetAddr(u32 PS_addr)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x07);
  Sendcmd(0x15);
  MYUSART_SendData(PS_addr >> 24);
  MYUSART_SendData(PS_addr >> 16);
  MYUSART_SendData(PS_addr >> 8);
  MYUSART_SendData(PS_addr);
  temp = 0x01 + 0x07 + 0x15
         + (u8)(PS_addr >> 24) + (u8)(PS_addr >> 16)
         + (u8)(PS_addr >> 8) + (u8)PS_addr;
  SendCheck(temp);
  AS608Addr = PS_addr; //������ָ�������ַ
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  AS608Addr = PS_addr;
  if(ensure == 0x00)
    printf("\r\n���õ�ַ�ɹ���");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_WriteNotepad(u8 NotePageNum, u8 *Byte32)
* @Features      ��ģ���ڲ�Ϊ�û�������256bytes��FLASH�ռ����ڴ��û����±�,
            	     �ü��±��߼��ϱ��ֳ� 16 ��ҳ��
* @InportFunction��NotePageNum(0~15),Byte32(Ҫд�����ݣ�32���ֽ�)
* @ExportFunction��ģ�鷵��ȷ����
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ��No
**********************************************************************/
u8 PS_WriteNotepad(u8 NotePageNum, u8 *Byte32)
{
  u16 temp;
  u8  ensure, i;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(36);
  Sendcmd(0x18);
  MYUSART_SendData(NotePageNum);
  for(i = 0; i < 32; i++)
  {
    MYUSART_SendData(Byte32[i]);
    temp += Byte32[i];
  }
  temp = 0x01 + 36 + 0x18 + NotePageNum + temp;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

/**********************************************************************
* @Function      ��u8 PS_ReadNotepad(u8 NotePageNum, u8 *Byte32)
* @Features      ����ȡFLASH�û�����128bytes����
* @InportFunction��NotePageNum(0~15)
* @ExportFunction��ģ�鷵��ȷ����+�û���Ϣ
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ��������PS_ReadNotepad
**********************************************************************/
u8 PS_ReadNotepad(u8 NotePageNum, u8 *Byte32)
{
  u16 temp;
  u8  ensure, i;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x04);
  Sendcmd(0x19);
  MYUSART_SendData(NotePageNum);
  temp = 0x01 + 0x04 + 0x19 + NotePageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    for(i = 0; i < 32; i++)
    {
      Byte32[i] = data[10 + i];
    }
  }
  else
    ensure = 0xff;
  return ensure;
}

/// @brief  ��������ָ�� 
/// @param BufferID  ������ID
/// @param StartPage  ��ʼҳ
/// @param PageNum  ҳ��
/// @param p  �������ָ��
/// @return ģ�鷵��ȷ����+ҳ�루����ָ��ģ�壩
/// @note �����������򷵻�ҳ��,��ָ����ڵ�ȷ������ָ�ƿ��� ���ҵ�¼ʱ�����ܺõ�ָ�ƣ���ܿ�������������
u8 PS_HighSpeedSearch(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x08);
  Sendcmd(0x1b);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(StartPage >> 8);
  MYUSART_SendData(StartPage);
  MYUSART_SendData(PageNum >> 8);
  MYUSART_SendData(PageNum);
  temp = 0x01 + 0x08 + 0x1b + BufferID
         + (StartPage >> 8) + (u8)StartPage
         + (PageNum >> 8) + (u8)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    p->pageID 	= (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  }
  else
    ensure = 0xff;
  return ensure;
}

/// @brief  ����Чģ�����
/// @param ValidN  ��Чģ�����ָ��
/// @return  ģ�鷵��ȷ����
u8 PS_ValidTempleteNum(u16 *ValidN)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x1d);
  temp = 0x01 + 0x03 + 0x1d;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    *ValidN = (data[10] << 8) + data[11];
  }
  else
    ensure = 0xff;

  if(ensure == 0x00)
  {
    printf("\r\n��Чָ�Ƹ���=%d", (data[10] << 8) + data[11]);
  }
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

/// @brief  ��AS608����
/// @param PS_Addr  AS608��ַָ��
/// @return  ģ�鷵���µ�ַ����ȷ��ַ��
u8 PS_HandShake(u32 *PS_Addr)
{
  SendHead();
  SendAddr();
  MYUSART_SendData(0X01);
  MYUSART_SendData(0X00);
  MYUSART_SendData(0X00);
  delay_ms(200);
  if(USART3_RX_STA & 0X8000) //���յ�����
  {
    if(//�ж��ǲ���ģ�鷵�ص�Ӧ���
      USART3_RX_BUF[0] == 0XEF
      && USART3_RX_BUF[1] == 0X01
      && USART3_RX_BUF[6] == 0X07
    )
    {
      *PS_Addr = (USART3_RX_BUF[2] << 24) + (USART3_RX_BUF[3] << 16)
                 + (USART3_RX_BUF[4] << 8) + (USART3_RX_BUF[5]);
      USART3_RX_STA = 0;
      return 0;
    }
    USART3_RX_STA = 0;
  }
  return 1;
}

/**********************************************************************
* @Function      ��const char *EnsureMessage(u8 ensure)
* @Features      ������ȷ���������Ϣ������Ϣ
* @InportFunction��ensure
* @ExportFunction�����ؽ����������ݶ�Ӧֵ
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ��ģ��Ӧ���ȷ������Ϣ����
**********************************************************************/
const char *EnsureMessage(u8 ensure)
{
  const char *p;
  switch(ensure)
  {
  case  0x00:
    p = "       OK       ";
    break;
  case  0x01:
    p = " ���ݰ����մ��� ";
    break;
  case  0x02:
    p = "��������û����ָ";
    break;
  case  0x03:
    p = "¼��ָ��ͼ��ʧ��";
    break;
  case  0x04:
    p = " ָ��̫�ɻ�̫�� ";
    break;
  case  0x05:
    p = " ָ��̫ʪ��̫�� ";
    break;
  case  0x06:
    p = "  ָ��ͼ��̫��  ";
    break;
  case  0x07:
    p = " ָ��������̫�� ";
    break;
  case  0x08:
    p = "  ָ�Ʋ�ƥ��    ";
    break;
  case  0x09:
    p = " û��������ָ�� ";
    break;
  case  0x0a:
    p = "   �����ϲ�ʧ�� ";
    break;
  case  0x0b:
    p = "��ַ��ų�����Χ";
  case  0x10:
    p = "  ɾ��ģ��ʧ��  ";
    break;
  case  0x11:
    p = " ���ָ�ƿ�ʧ�� ";
    break;
  case  0x15:
    p = "������������Чͼ";
    break;
  case  0x18:
    p = " ��дFLASH����  ";
    break;
  case  0x19:
    p = "   δ�������   ";
    break;
  case  0x1a:
    p = "  ��Ч�Ĵ�����  ";
    break;
  case  0x1b:
    p = " �Ĵ������ݴ��� ";
    break;
  case  0x1c:
    p = " ���±�ҳ����� ";
    break;
  case  0x1f:
    p = "    ָ�ƿ���    ";
    break;
  case  0x20:
    p = "    ��ַ����    ";
    break;
  default :
    p = " ����ȷ�������� ";
    break;
  }
  return p;
}

//��ʾȷ���������Ϣ
/**********************************************************************
* @Function      ��void ShowErrMessage(u8 ensure)
* @Features      ����ʾȷ���������Ϣ
* @InportFunction��ensure
* @ExportFunction��No
* @Author        ��TXD
* @ModifiedDate  ��2020.12.29
* @Remarks       ��������ʾ
**********************************************************************/
void ShowErrMessage(u8 ensure)
{
	printf("Error \r\n");
	printf("ensure = %d \r\n",ensure);
}


// /**********************************************************************
// * @Function      ��void Add_FR(void)
// * @Features      �����ָ��
// * @InportFunction��No                
// * @ExportFunction��No
// * @Author        ��TXD
// * @ModifiedDate  ��2020.12.29
// * @Remarks       ��No
// **********************************************************************/
// void Add_FR(void)
// {
//   u8 i, ensure, processnum = 0;
//   u8 ID_NUM = 0;
// 	char str[20];
// 	OLED_Clear();
// /* ----------------------------------------------------------------------------------------------------------------- */	
//   while(1)
//   {
//     switch (processnum)
//     {
//     case 0:
//       i++;
// 		  Gui_DrawFont_GBK16(35,15,RED,WHITE,"�밴��ָ");
// 		  showimage(gImage_Finger65,33,40,97,104,65,65);    //ָ��ͼƬ��ʾʾ��
// 		    //OLED_ShowString(1,1,"Press");
// 		    OLED_ShowCharChinese(1,1,17);OLED_ShowCharChinese(1,3,19);
// 		    OLED_ShowCharChinese(1,5,21);OLED_ShowCharChinese(1,7,5);
// /* ----------------------------------------------------------------------------------------------------------------- */		
//       ensure = PS_GetImage();
//       if(ensure == 0x00)
//       {
//         ensure = PS_GenChar(CharBuffer1); //��������
//         if(ensure == 0x00)
//         {
// 					Gui_DrawFont_GBK16(35,15,GREEN,WHITE,"ָ������");
// 					  //OLED_ShowString(1,1,"normal");
// 					  OLED_ShowCharChinese(1,1,5);OLED_ShowCharChinese(1,3,7);
// 					  OLED_ShowCharChinese(1,5,23);OLED_ShowCharChinese(1,7,25);
// /* ----------------------------------------------------------------------------------------------------------------- */					
// 					printf("ָ������ \r\n");
// 					delay_ms(1500);
//           i = 0;
//           processnum = 1;                 //�����ڶ���
//         }
//         else ShowErrMessage(ensure);
//       }
//       else
// 			{				
// 				ShowErrMessage(ensure);
// 			}
//       break;

//     case 1:
//       i++;
// 		  printf("���ٰ�һ�� \r\n");
// 		  Gui_DrawFont_GBK16(24,15,RED,WHITE,"���ٰ�һ��");
// 		    //OLED_ShowString(1,1,"Press");
// 		    OLED_ShowCharChinese(1,1,17);OLED_ShowCharChinese(1,3,27);
// 				OLED_ShowCharChinese(1,5,19);OLED_ShowCharChinese(1,7,21);OLED_ShowCharChinese(1,9,5);
// /* ----------------------------------------------------------------------------------------------------------------- */		
// 		  delay_ms(1500);
// 		  delay_ms(1500);
//       ensure = PS_GetImage();
//       if(ensure == 0x00)
//       {
//         ensure = PS_GenChar(CharBuffer2);    //��������
//         if(ensure == 0x00)
//         {
// 					printf("ָ������ \r\n");
// 					  //OLED_ShowString(1,1,"normal");
// 					  OLED_ShowCharChinese(1,1,5);OLED_ShowCharChinese(1,3,7);
// 					  OLED_ShowCharChinese(1,5,23);OLED_ShowCharChinese(1,7,25);OLED_ShowString(1,9,"  ");					
// /* ----------------------------------------------------------------------------------------------------------------- */					
// 					delay_ms(1500);
//           i = 0;
//           processnum = 2;                    //����������
//         }
//         else ShowErrMessage(ensure);
//       }
//       else ShowErrMessage(ensure);
//       break;

//     case 2:
// 			printf("�Ա�����ָ�� \r\n");
// 		    //OLED_ShowString(1,1,"matching");
// 		    OLED_ShowCharChinese(1,1,29);OLED_ShowCharChinese(1,3,31);
// 	    	OLED_ShowCharChinese(1,5,33);OLED_ShowCharChinese(1,7,35);
// 		    OLED_ShowCharChinese(1,9,5);OLED_ShowCharChinese(1,11,7);
// /* ----------------------------------------------------------------------------------------------------------------- */		
//       ensure = PS_Match();
//       if(ensure == 0x00)
//       {
// 				Gui_DrawFont_GBK16(16,15,BLUE,WHITE,"�Ա�ָ�Ƴɹ�");
// 				  //OLED_ShowString(1,1,"matching OK");
// 				  OLED_ShowCharChinese(1,1,29);OLED_ShowCharChinese(1,3,31);
// 				  OLED_ShowCharChinese(1,5,5);OLED_ShowCharChinese(1,7,7);
// 				  OLED_ShowCharChinese(1,9,37);OLED_ShowCharChinese(1,11,39);
// /* ----------------------------------------------------------------------------------------------------------------- */				
// 				delay_ms(1500);
// 				printf("�Աȳɹ� \r\n");
//         processnum = 3; //�������Ĳ�
//       }
//       else
//       {
// 				printf("�Ա�ʧ�� \r\n");
// 				Gui_DrawFont_GBK16(16,15,BLUE,WHITE,"�Ա�ָ��ʧ��");
// 				delay_ms(1500);
// 				Gui_DrawFont_GBK16(16,15,BLUE,WHITE,"           ");
//         ShowErrMessage(ensure);
//         i = 0;
//         processnum = 0; //���ص�һ��
//       }
//       delay_ms(500);
//       break;

//     case 3:
// 			printf("����ָ��ģ�� \r\n");
// 		  Gui_DrawFont_GBK16(16,15,BLUE,WHITE,"��������ָ��");
// 				OLED_Clear();
// 		    //OLED_ShowString(1,1,"waiting---");
// 		    OLED_ShowCharChinese(1,1,41);OLED_ShowCharChinese(1,3,37);
// 		    OLED_ShowCharChinese(1,5,5);OLED_ShowCharChinese(1,7,7);
// 		    OLED_ShowCharChinese(1,9,43);OLED_ShowCharChinese(1,11,45);
// /* ----------------------------------------------------------------------------------------------------------------- */		
//       delay_ms(1500);
// 		  delay_ms(1500);
//       ensure = PS_RegModel();
//       if(ensure == 0x00)
//       {
// 				delay_ms(1500);
// 				showimage(gImage_finish65,33,40,97,104,65,65);    //ָ��ͼƬ��ʾʾ��
// 				Gui_DrawFont_GBK16(16,15,GREEN,WHITE,"ָ�����ɳɹ�");
// 					OLED_Clear();
// 				  //OLED_ShowString(1,1,"succeed");
// 				  OLED_ShowCharChinese(1,1,5);OLED_ShowCharChinese(1,3,7);
// 				  OLED_ShowCharChinese(1,5,41);OLED_ShowCharChinese(1,7,37);
// 				  OLED_ShowCharChinese(1,9,37);OLED_ShowCharChinese(1,11,39);
// /* ----------------------------------------------------------------------------------------------------------------- */				
// 				printf("����ָ��ģ��ɹ� \r\n");	
// 				delay_ms(1500);
//         processnum = 4; //�������岽
//       }
//       else
//       {
//         processnum = 0;
//         ShowErrMessage(ensure);
//       }
//       delay_ms(1500);
//       break;

//     case 4:
// 			printf("��K4��,��K2�� \r\n");
// 		    OLED_ShowString(2,1,"K4+ID;K2-ID");
// /* ----------------------------------------------------------------------------------------------------------------- */		
// 		  printf("��K3���� \r\n");
// 			  OLED_ShowString(3,1,"K3:");
// 		    OLED_ShowCharChinese(3,4,51);OLED_ShowCharChinese(3,6,53);
// /* ----------------------------------------------------------------------------------------------------------------- */		
// 		  printf("0=< ID <=99  \r\n");
// 		  Gui_DrawFont_GBK16(16,15,GREEN,WHITE,"              ");
// 		  Gui_DrawFont_GBK16(16,8,RED,WHITE,"K2:ID+,K4:ID-");
// 		  Gui_DrawFont_GBK16(16,28,RED,WHITE,"�밴 K3 ����");
//       while(key_num != 3)
//       {
//         key_num = KEY_Scan();
//         if(key_num == 2)
//         {
//           key_num = 0;
//           if(ID_NUM > 0)
//             ID_NUM--;
//         }
//         if(key_num == 4)
//         {
//           key_num = 0;
//           if(ID_NUM < 99)
//             ID_NUM++;
//         }
// 			 sprintf(str, " ָ��ID: %d ",ID_NUM);	
// 			 Gui_DrawFont_GBK16(16,105,RED,WHITE,(u8*)str);
// 				OLED_ShowString(4,1,"ID:");
// 				OLED_ShowNum(4,4,ID_NUM,3);
// /* ----------------------------------------------------------------------------------------------------------------- */				
//       }
//       key_num = 0;
// 			printf("����ģ�� \r\n");
// 			printf("ID = %d \r\n",ID_NUM);
//       ensure = PS_StoreChar(CharBuffer2, ID_NUM); //����ģ��
//       if(ensure == 0x00)
//       {				
// 				Lcd_Clear(WHITE);
// 				Gui_DrawFont_GBK16(16,10,GREEN,WHITE,"¼��ָ�Ƴɹ�");
// 					OLED_Clear();
// 				  //OLED_ShowString(1,1,"OK!!!");
// 	        OLED_ShowCharChinese(1,1,1);OLED_ShowCharChinese(1,3,3);
// 	        OLED_ShowCharChinese(1,5,5);OLED_ShowCharChinese(1,7,7);
// 				  OLED_ShowCharChinese(1,9,37);OLED_ShowCharChinese(1,11,39);
// 					OLED_ShowString(2,1,"K4:");
// 					OLED_ShowCharChinese(2,4,47);OLED_ShowCharChinese(2,6,49);				 
// /* ----------------------------------------------------------------------------------------------------------------- */				
// 				Gui_DrawFont_GBK16(32,30,GREEN,WHITE,"��лʹ��");
// 				Gui_DrawFont_GBK16(16,50,GREEN,WHITE,"ָ��ʶ��ϵͳ");
// 				printf("¼��ָ�Ƴɹ� \r\n");
// 				delay_ms(1500);
// 				delay_ms(1500);
// 				Lcd_Clear(WHITE);
// 				 while(KEY_Scan() != 4){;}
// 	       MAIN_OLED();
// 		     delay_ms(100);
// /* ----------------------------------------------------------------------------------------------------------------- */					 
// 				Redraw_Mainmenu();
//         return ;
//       }
//       else
//       {
//         Lcd_Clear(WHITE);
//         processnum = 0;
//         ShowErrMessage(ensure);
//       }
//       break;
//     }
		
//     delay_ms(400);
//     if(i == 10)                            //����5��û�а���ָ���˳�
//     {
// 			Gui_DrawFont_GBK16(35,15,RED,WHITE,"��֤��ʱ");
// 			delay_ms(1500);
// 			Redraw_Mainmenu();
//       break;
//     }
//   }
// }

// SysPara AS608Para;//ָ��ģ��AS608����
// /**********************************************************************
// * @Function      ��void press_FR(void)
// * @Features      ��ˢָ��
// * @InportFunction��No                
// * @ExportFunction��No
// * @Author        ��TXD
// * @ModifiedDate  ��2020.12.29
// * @Remarks       ��No
// **********************************************************************/
// void press_FR(void)
// {
//   SearchResult seach;
//   u8 ensure;
//   char str[20];
// 	Gui_DrawFont_GBK16(35,15,RED,WHITE,"�밴��ָ");
// 	showimage(gImage_Finger65,33,40,97,104,65,65);    //ָ��ͼƬ��ʾʾ��
// 	  OLED_Clear();
// 	  //OLED_ShowString(1,1,"Press");
// 		OLED_ShowCharChinese(1,1,17);OLED_ShowCharChinese(1,3,19);
// 		OLED_ShowCharChinese(1,5,21);OLED_ShowCharChinese(1,7,5);	  
// /* ----------------------------------------------------------------------------------------------------------------- */	
//   while(key_num != 1)
//   {
//     key_num = KEY_Scan();
//     ensure = PS_GetImage();
//     if(ensure == 0x00) 
//     {
// 			printf("��ȡͼ��ɹ� \r\n");
//       ensure = PS_GenChar(CharBuffer1);
// 				OLED_Clear();
// 	      //OLED_ShowString(1,1,"gain success");
// 			  OLED_ShowCharChinese(1,1,55);OLED_ShowCharChinese(1,3,57);
// 			  OLED_ShowCharChinese(1,5,59);OLED_ShowCharChinese(1,7,61);
// 			  OLED_ShowCharChinese(1,9,37);OLED_ShowCharChinese(1,11,39);
// /* ----------------------------------------------------------------------------------------------------------------- */			
//       if(ensure == 0x00) 
//       {
// 				printf("���������ɹ� \r\n");
//         ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);				
// 				  //OLED_ShowString(2,1,"matching--");
// 				  OLED_ShowCharChinese(2,1,41);OLED_ShowCharChinese(2,3,37);
// 			    OLED_ShowCharChinese(2,5,63);OLED_ShowCharChinese(2,7,65);
// 			    OLED_ShowCharChinese(2,9,37);OLED_ShowCharChinese(2,11,39);			
// /* ----------------------------------------------------------------------------------------------------------------- */				
//         if(ensure == 0x00) 
//         {
// 					printf("��֤�ɹ� \r\n");
// 					Lcd_Clear(WHITE);
//           Gui_DrawFont_GBK16(16,15,GREEN,WHITE,"ָ����֤�ɹ�");
// 			    OLED_ShowCharChinese(3,1,13);OLED_ShowCharChinese(3,3,15);
// 			    OLED_ShowCharChinese(3,5,37);OLED_ShowCharChinese(3,7,39);							
// 					 OLED_ShowString(3,10,"ID:");
// /* ----------------------------------------------------------------------------------------------------------------- */					
//           sprintf(str, " ָ��ID:%d  ", seach.pageID);				
//           Gui_DrawFont_GBK16(16,35,RED,WHITE,(u8*)str);
// 					  OLED_ShowNum(3,13,seach.pageID,3);	
// 					  OLED_ShowString(4,1,"K4:");
// 					  OLED_ShowCharChinese(4,4,47);OLED_ShowCharChinese(4,6,49);
// /* ----------------------------------------------------------------------------------------------------------------- */					
//           delay_ms(1500);
//           delay_ms(1500);
// 					break;
//         }
//         else
//         {
// 					Gui_DrawFont_GBK16(16,15,RED,WHITE,"ָ����֤ʧ��");
// 					Gui_DrawFont_GBK16(24,105,RED,WHITE,"���޴�ָ��");
// 					printf("��֤ʧ�� \r\n");
// 					  //OLED_ShowString(4,1,"fail !!!");
// 					  OLED_ShowCharChinese(3,1,13);OLED_ShowCharChinese(3,3,15);
// 					  OLED_ShowCharChinese(3,5,77);OLED_ShowCharChinese(3,7,79);
					
// 					  OLED_ShowString(4,1,"K4:");
// 					  OLED_ShowCharChinese(4,4,47);OLED_ShowCharChinese(4,6,49);	
// /* ----------------------------------------------------------------------------------------------------------------- */					
// 					delay_ms(1500);
// 					delay_ms(1500);
// 					break;
//         }
//       }
//       else
// 			{
				
// 			};
//     }
//   }
//    Lcd_Clear(WHITE);
// 	 Redraw_Mainmenu();//�������˵�(�����������ڷֱ��ʳ�������ֵ�����޷���ʾ)	
	
// 	   while(KEY_Scan() != 4){;}
// 	   MAIN_OLED();
// 		 delay_ms(100);
// /* ----------------------------------------------------------------------------------------------------------------- */	
// }

// void Del_FR(void)
// {
//   u8  ensure;
//   u16 ID_NUM = 0;
// 	char DelStr[20];

// 	DisplayButtonUp(15,10,115,30); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,11,RED,WHITE,"K1:����");
// 	  OLED_Clear();
// 	  OLED_ShowString(1,1,"K1:");
// 	  OLED_ShowCharChinese(1,4,47);OLED_ShowCharChinese(1,6,49);
// /* ----------------------------------------------------------------------------------------------------------------- */	
	
// 	DisplayButtonUp(15,32,115,52); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,33,RED,WHITE,"K2:��һ��ID ");
// 	  OLED_ShowString(2,1,"K4:ID+");	
// /* ----------------------------------------------------------------------------------------------------------------- */	
	
// 	DisplayButtonUp(15,54,115,74); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,55,RED,WHITE,"K3:ȷ��");
// 	  OLED_ShowString(3,1,"K3:");	
// 	  OLED_ShowCharChinese(3,4,67);OLED_ShowCharChinese(3,6,69);
// /* ----------------------------------------------------------------------------------------------------------------- */	
	
// 	DisplayButtonUp(15,76,115,96); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,77,RED,WHITE,"K4:��һ��ID");
// 	  OLED_ShowString(2,8,"K2:ID-");
// /* ----------------------------------------------------------------------------------------------------------------- */		
	
// 	DisplayButtonUp(15,98,115,118); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,99,RED,WHITE,"K5:���ָ�ƿ�");
// 	  OLED_ShowString(4,1,"K5:");
// 	  OLED_ShowCharChinese(4,4,71);OLED_ShowCharChinese(4,6,73);
// 		OLED_ShowCharChinese(4,8,5);OLED_ShowCharChinese(4,10,7);
// 		OLED_ShowCharChinese(4,12,75);
// /* ----------------------------------------------------------------------------------------------------------------- */		
// 	delay_ms(1500);
	
// 	Lcd_Clear(WHITE);
// 	DisplayButtonUp(15,10,115,30); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,11,RED,WHITE,"K2:��һ��ID");
	
// 	DisplayButtonUp(15,32,115,52); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,33,RED,WHITE,"K4:��һ��ID ");
	
// 	DisplayButtonUp(15,76,115,96); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,77,RED,WHITE,"K3:ȷ��");
	
// 			while(key_num != 3)
// 			{
// 				key_num = KEY_Scan();
// 				if(key_num == 2)
// 				{
// 					key_num = 0;
// 					if(ID_NUM > 0)
// 						ID_NUM--;
// 				}
// 				if(key_num == 4)
// 				{
// 					key_num = 0;
// 					if(ID_NUM < 99)
// 						ID_NUM++;
// 				}
// 				if(key_num == 1)
// 					goto MENU ;             //������ҳ��
// 				if(key_num == 5) 
// 				{
// 					key_num = 0;
// 					ensure = PS_Empty();   //���ָ�ƿ�
// 					if(ensure == 0)
// 					{					
// 						Lcd_Clear(WHITE);
// 						Gui_DrawFont_GBK16(10,15,GREEN,WHITE,"���ָ�ƿ�ɹ�");
// 						  		OLED_Clear();
// 	                OLED_ShowString(1,1,"succeed");
// /* ----------------------------------------------------------------------------------------------------------------- */						
// 						printf("���ָ�ƿ�ɹ� \r\n");
// 					}
// 					else
// 						ShowErrMessage(ensure);
// 						delay_ms(1500);
// 					goto MENU ;             //������ҳ��
// 				}

// 				sprintf(DelStr, "ָ��ID: %d ",ID_NUM);	
// 				DisplayButtonUp(15,54,115,74); //x1,y1,x2,y2
// 	      Gui_DrawFont_GBK16(16,55,RED,WHITE,(u8*)DelStr);
// 				printf("ID_NUM = %d \r\n",ID_NUM);
				  
// 				  OLED_ShowString(3,10,"ID:");
// 				  OLED_ShowNum(3,13,ID_NUM,3);
// /* ----------------------------------------------------------------------------------------------------------------- */				
// 			}

	
//   ensure = PS_DeletChar(ID_NUM, 1); //ɾ������ָ��
//   if(ensure == 0)
//   {
// 		Lcd_Clear(WHITE);
// 		sprintf(DelStr, "ɾ��ָ��ID: %d ",ID_NUM);	
// 		Gui_DrawFont_GBK16(16,55,RED,WHITE,(u8*)DelStr);
// 		Gui_DrawFont_GBK16(32,75,GREEN,WHITE,"ɾ���ɹ�");
// 		printf("ɾ���ɹ� \r\n");
// 		  OLED_Clear();
// 		  OLED_ShowString(1,1,"succeed OK");
// /* ----------------------------------------------------------------------------------------------------------------- */		
//   }
//   else
//     ShowErrMessage(ensure);
//     delay_ms(1500);
// MENU:
//    Lcd_Clear(WHITE);
// 	 Redraw_Mainmenu();//�������˵�(�����������ڷֱ��ʳ�������ֵ�����޷���ʾ)	
// 	 MAIN_OLED();
//    key_num = 0;
// }
