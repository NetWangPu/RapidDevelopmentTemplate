#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "usart3.h"
#include "as608.h"


u32 AS608Addr = 0XFFFFFFFF; //默认
char str2[6] = {0};

/// @brief 发送数据到模块
/// @param data 
static void MYUSART_SendData(u8 data)
{
  while((USART3->SR & 0X40) == 0);
  USART3->DR = data;
}

/// @brief 发送头
/// @param  无
static void SendHead(void)
{
  MYUSART_SendData(0xEF);
  MYUSART_SendData(0x01);
}

/// @brief 发送地址
/// @param  无
static void SendAddr(void)
{
  MYUSART_SendData(AS608Addr >> 24);
  MYUSART_SendData(AS608Addr >> 16);
  MYUSART_SendData(AS608Addr >> 8);
  MYUSART_SendData(AS608Addr);
}

/// @brief  发送包标识
/// @param flag 标识 
static void SendFlag(u8 flag)
{
  MYUSART_SendData(flag);
}

/// @brief  发送包长度
/// @param length 长度 
static void SendLength(int length)
{
  MYUSART_SendData(length >> 8);
  MYUSART_SendData(length);
}

/// @brief  发送指令码
/// @param cmd 指令码 
static void Sendcmd(u8 cmd)
{
  MYUSART_SendData(cmd);
}

/// @brief  发送校验和
/// @param check 校验和 
static void SendCheck(u16 check)
{
  MYUSART_SendData(check >> 8);
  MYUSART_SendData(check);
}

/// @brief  判断中断接收的数组有没有应答包
/// @param waittime 等待中断接收数据的时间（单位1ms） 
/// @return 数据包首地址
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
    if(USART3_RX_STA & 0X8000) //接收到一次数据
    {
      USART3_RX_STA = 0;
      data = strstr((const char*)USART3_RX_BUF, (const char*)str);
      if(data)
        return (u8*)data;
    }
  }
  return 0;
}

/// @brief  录入图像
/// @param  无
/// @return  模块返回确认字
/// @note  探测手指，探测到后录入指纹图像存于ImageBuffer。
u8 PS_GetImage(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
* @Function      ：u8 PS_GenChar(u8 BufferID)
* @Features      ：将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2
* @InportFunction：BufferID --> charBuffer1:0x01	charBuffer1:0x02           
* @ExportFunction：模块返回确认字
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：生成特征 PS_GenChar
**********************************************************************/
u8 PS_GenChar(u8 BufferID)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
* @Function      ：u8 PS_Match(void)
* @Features      ：精确比对CharBuffer1 与CharBuffer2 中的特征文件
* @InportFunction：BufferID --> charBuffer1:0x01	charBuffer1:0x02           
* @ExportFunction：模块返回确认字
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：精确比对两枚指纹特征 PS_Match
**********************************************************************/
u8 PS_Match(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
* @Function      ：u8 PS_Search(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
* @Features      ：以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库.若搜索到，则返回页码。
* @InportFunction：BufferID @ref CharBuffer1	CharBuffer2           
* @ExportFunction：模块返回确认字，页码（相配指纹模板）
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：搜索指纹 PS_Searchh
**********************************************************************/
u8 PS_Search(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
* @Function      ：u8 PS_RegModel(void)
* @Features      ：将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2
* @InportFunction：No        
* @ExportFunction：模块返回确认字
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：合并特征（生成模板）PS_RegModel
**********************************************************************/
u8 PS_RegModel(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
* @Function      ：u8 PS_StoreChar(u8 BufferID, u16 PageID)
* @Features      ：将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2
* @InportFunction：BufferID @ref charBuffer1:0x01	
                   charBuffer1:0x02
                   PageID（指纹库位置号）     
* @ExportFunction：模块返回确认字
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：储存模板 PS_StoreChar
**********************************************************************/
u8 PS_StoreChar(u8 BufferID, u16 PageID)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
* @Function      ：u8 PS_DeletChar(u16 PageID, u16 N)
* @Features      ：删除flash数据库中指定ID号开始的N个指纹模板
* @InportFunction：PageID(指纹库模板号)，N删除的模板个数。     
* @ExportFunction：模块返回确认字
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：删除模板 PS_DeletCharr
**********************************************************************/
u8 PS_DeletChar(u16 PageID, u16 N)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
* @Function      ：u8 PS_Empty(void)
* @Features      ：删除flash数据库中所有指纹模板
* @InportFunction：No     
* @ExportFunction：模块返回确认字
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：清空指纹库 PS_Empty
**********************************************************************/
u8 PS_Empty(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
* @Function      ：u8 PS_WriteReg(u8 RegNum, u8 DATA)
* @Features      ：写模块寄存器
* @InportFunction：寄存器序号RegNum:4\5\6
* @ExportFunction：模块返回确认字
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：写系统寄存器 PS_WriteReg
**********************************************************************/
u8 PS_WriteReg(u8 RegNum, u8 DATA)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
    printf("\r\n设置参数成功！");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

/**********************************************************************
* @Function      ：u8 PS_ReadSysPara(SysPara *p)
* @Features      ：读取模块的基本参数（波特率，包大小等)
* @InportFunction：No
* @ExportFunction：模块返回确认字 + 基本参数（16bytes）
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：读系统基本参数 PS_ReadSysPara
**********************************************************************/
u8 PS_ReadSysPara(SysPara *p)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
    printf("\r\n模块最大指纹容量=%d", p->PS_max);
    printf("\r\n对比等级=%d", p->PS_level);
    printf("\r\n地址=%x", p->PS_addr);
    printf("\r\n波特率=%d", p->PS_N * 9600);
  }
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

/**********************************************************************
* @Function      ：u8 PS_SetAddr(u32 PS_addr)
* @Features      ：设置模块地址
* @InportFunction：PS_addr
* @ExportFunction：模块返回确认字
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：设置模块地址 PS_SetAddr
**********************************************************************/
u8 PS_SetAddr(u32 PS_addr)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
  AS608Addr = PS_addr; //发送完指令，更换地址
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  AS608Addr = PS_addr;
  if(ensure == 0x00)
    printf("\r\n设置地址成功！");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

/**********************************************************************
* @Function      ：u8 PS_WriteNotepad(u8 NotePageNum, u8 *Byte32)
* @Features      ：模块内部为用户开辟了256bytes的FLASH空间用于存用户记事本,
            	     该记事本逻辑上被分成 16 个页。
* @InportFunction：NotePageNum(0~15),Byte32(要写入内容，32个字节)
* @ExportFunction：模块返回确认字
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：No
**********************************************************************/
u8 PS_WriteNotepad(u8 NotePageNum, u8 *Byte32)
{
  u16 temp;
  u8  ensure, i;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
* @Function      ：u8 PS_ReadNotepad(u8 NotePageNum, u8 *Byte32)
* @Features      ：读取FLASH用户区的128bytes数据
* @InportFunction：NotePageNum(0~15)
* @ExportFunction：模块返回确认字+用户信息
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：读记事PS_ReadNotepad
**********************************************************************/
u8 PS_ReadNotepad(u8 NotePageNum, u8 *Byte32)
{
  u16 temp;
  u8  ensure, i;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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

/// @brief  高速搜索指纹 
/// @param BufferID  缓冲区ID
/// @param StartPage  起始页
/// @param PageNum  页数
/// @param p  搜索结果指针
/// @return 模块返回确认字+页码（相配指纹模板）
/// @note 若搜索到，则返回页码,该指令对于的确存在于指纹库中 ，且登录时质量很好的指纹，会很快给出搜索结果。
u8 PS_HighSpeedSearch(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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

/// @brief  读有效模板个数
/// @param ValidN  有效模板个数指针
/// @return  模块返回确认字
u8 PS_ValidTempleteNum(u16 *ValidN)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
    printf("\r\n有效指纹个数=%d", (data[10] << 8) + data[11]);
  }
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

/// @brief  与AS608握手
/// @param PS_Addr  AS608地址指针
/// @return  模块返回新地址（正确地址）
u8 PS_HandShake(u32 *PS_Addr)
{
  SendHead();
  SendAddr();
  MYUSART_SendData(0X01);
  MYUSART_SendData(0X00);
  MYUSART_SendData(0X00);
  delay_ms(200);
  if(USART3_RX_STA & 0X8000) //接收到数据
  {
    if(//判断是不是模块返回的应答包
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
* @Function      ：const char *EnsureMessage(u8 ensure)
* @Features      ：解析确认码错误信息返回信息
* @InportFunction：ensure
* @ExportFunction：返回解析到的数据对应值
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：模块应答包确认码信息解析
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
    p = " 数据包接收错误 ";
    break;
  case  0x02:
    p = "传感器上没有手指";
    break;
  case  0x03:
    p = "录入指纹图像失败";
    break;
  case  0x04:
    p = " 指纹太干或太淡 ";
    break;
  case  0x05:
    p = " 指纹太湿或太糊 ";
    break;
  case  0x06:
    p = "  指纹图像太乱  ";
    break;
  case  0x07:
    p = " 指纹特征点太少 ";
    break;
  case  0x08:
    p = "  指纹不匹配    ";
    break;
  case  0x09:
    p = " 没有搜索到指纹 ";
    break;
  case  0x0a:
    p = "   特征合并失败 ";
    break;
  case  0x0b:
    p = "地址序号超出范围";
  case  0x10:
    p = "  删除模板失败  ";
    break;
  case  0x11:
    p = " 清空指纹库失败 ";
    break;
  case  0x15:
    p = "缓冲区内无有效图";
    break;
  case  0x18:
    p = " 读写FLASH出错  ";
    break;
  case  0x19:
    p = "   未定义错误   ";
    break;
  case  0x1a:
    p = "  无效寄存器号  ";
    break;
  case  0x1b:
    p = " 寄存器内容错误 ";
    break;
  case  0x1c:
    p = " 记事本页码错误 ";
    break;
  case  0x1f:
    p = "    指纹库满    ";
    break;
  case  0x20:
    p = "    地址错误    ";
    break;
  default :
    p = " 返回确认码有误 ";
    break;
  }
  return p;
}

//显示确认码错误信息
/**********************************************************************
* @Function      ：void ShowErrMessage(u8 ensure)
* @Features      ：显示确认码错误信息
* @InportFunction：ensure
* @ExportFunction：No
* @Author        ：TXD
* @ModifiedDate  ：2020.12.29
* @Remarks       ：错误显示
**********************************************************************/
void ShowErrMessage(u8 ensure)
{
	printf("Error \r\n");
	printf("ensure = %d \r\n",ensure);
}


// /**********************************************************************
// * @Function      ：void Add_FR(void)
// * @Features      ：添加指纹
// * @InportFunction：No                
// * @ExportFunction：No
// * @Author        ：TXD
// * @ModifiedDate  ：2020.12.29
// * @Remarks       ：No
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
// 		  Gui_DrawFont_GBK16(35,15,RED,WHITE,"请按手指");
// 		  showimage(gImage_Finger65,33,40,97,104,65,65);    //指纹图片显示示例
// 		    //OLED_ShowString(1,1,"Press");
// 		    OLED_ShowCharChinese(1,1,17);OLED_ShowCharChinese(1,3,19);
// 		    OLED_ShowCharChinese(1,5,21);OLED_ShowCharChinese(1,7,5);
// /* ----------------------------------------------------------------------------------------------------------------- */		
//       ensure = PS_GetImage();
//       if(ensure == 0x00)
//       {
//         ensure = PS_GenChar(CharBuffer1); //生成特征
//         if(ensure == 0x00)
//         {
// 					Gui_DrawFont_GBK16(35,15,GREEN,WHITE,"指纹正常");
// 					  //OLED_ShowString(1,1,"normal");
// 					  OLED_ShowCharChinese(1,1,5);OLED_ShowCharChinese(1,3,7);
// 					  OLED_ShowCharChinese(1,5,23);OLED_ShowCharChinese(1,7,25);
// /* ----------------------------------------------------------------------------------------------------------------- */					
// 					printf("指纹正常 \r\n");
// 					delay_ms(1500);
//           i = 0;
//           processnum = 1;                 //跳到第二步
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
// 		  printf("请再按一次 \r\n");
// 		  Gui_DrawFont_GBK16(24,15,RED,WHITE,"请再按一次");
// 		    //OLED_ShowString(1,1,"Press");
// 		    OLED_ShowCharChinese(1,1,17);OLED_ShowCharChinese(1,3,27);
// 				OLED_ShowCharChinese(1,5,19);OLED_ShowCharChinese(1,7,21);OLED_ShowCharChinese(1,9,5);
// /* ----------------------------------------------------------------------------------------------------------------- */		
// 		  delay_ms(1500);
// 		  delay_ms(1500);
//       ensure = PS_GetImage();
//       if(ensure == 0x00)
//       {
//         ensure = PS_GenChar(CharBuffer2);    //生成特征
//         if(ensure == 0x00)
//         {
// 					printf("指纹正常 \r\n");
// 					  //OLED_ShowString(1,1,"normal");
// 					  OLED_ShowCharChinese(1,1,5);OLED_ShowCharChinese(1,3,7);
// 					  OLED_ShowCharChinese(1,5,23);OLED_ShowCharChinese(1,7,25);OLED_ShowString(1,9,"  ");					
// /* ----------------------------------------------------------------------------------------------------------------- */					
// 					delay_ms(1500);
//           i = 0;
//           processnum = 2;                    //跳到第三步
//         }
//         else ShowErrMessage(ensure);
//       }
//       else ShowErrMessage(ensure);
//       break;

//     case 2:
// 			printf("对比两次指纹 \r\n");
// 		    //OLED_ShowString(1,1,"matching");
// 		    OLED_ShowCharChinese(1,1,29);OLED_ShowCharChinese(1,3,31);
// 	    	OLED_ShowCharChinese(1,5,33);OLED_ShowCharChinese(1,7,35);
// 		    OLED_ShowCharChinese(1,9,5);OLED_ShowCharChinese(1,11,7);
// /* ----------------------------------------------------------------------------------------------------------------- */		
//       ensure = PS_Match();
//       if(ensure == 0x00)
//       {
// 				Gui_DrawFont_GBK16(16,15,BLUE,WHITE,"对比指纹成功");
// 				  //OLED_ShowString(1,1,"matching OK");
// 				  OLED_ShowCharChinese(1,1,29);OLED_ShowCharChinese(1,3,31);
// 				  OLED_ShowCharChinese(1,5,5);OLED_ShowCharChinese(1,7,7);
// 				  OLED_ShowCharChinese(1,9,37);OLED_ShowCharChinese(1,11,39);
// /* ----------------------------------------------------------------------------------------------------------------- */				
// 				delay_ms(1500);
// 				printf("对比成功 \r\n");
//         processnum = 3; //跳到第四步
//       }
//       else
//       {
// 				printf("对比失败 \r\n");
// 				Gui_DrawFont_GBK16(16,15,BLUE,WHITE,"对比指纹失败");
// 				delay_ms(1500);
// 				Gui_DrawFont_GBK16(16,15,BLUE,WHITE,"           ");
//         ShowErrMessage(ensure);
//         i = 0;
//         processnum = 0; //跳回第一步
//       }
//       delay_ms(500);
//       break;

//     case 3:
// 			printf("生成指纹模板 \r\n");
// 		  Gui_DrawFont_GBK16(16,15,BLUE,WHITE,"正在生成指纹");
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
// 				showimage(gImage_finish65,33,40,97,104,65,65);    //指纹图片显示示例
// 				Gui_DrawFont_GBK16(16,15,GREEN,WHITE,"指纹生成成功");
// 					OLED_Clear();
// 				  //OLED_ShowString(1,1,"succeed");
// 				  OLED_ShowCharChinese(1,1,5);OLED_ShowCharChinese(1,3,7);
// 				  OLED_ShowCharChinese(1,5,41);OLED_ShowCharChinese(1,7,37);
// 				  OLED_ShowCharChinese(1,9,37);OLED_ShowCharChinese(1,11,39);
// /* ----------------------------------------------------------------------------------------------------------------- */				
// 				printf("生成指纹模板成功 \r\n");	
// 				delay_ms(1500);
//         processnum = 4; //跳到第五步
//       }
//       else
//       {
//         processnum = 0;
//         ShowErrMessage(ensure);
//       }
//       delay_ms(1500);
//       break;

//     case 4:
// 			printf("按K4加,按K2减 \r\n");
// 		    OLED_ShowString(2,1,"K4+ID;K2-ID");
// /* ----------------------------------------------------------------------------------------------------------------- */		
// 		  printf("按K3保存 \r\n");
// 			  OLED_ShowString(3,1,"K3:");
// 		    OLED_ShowCharChinese(3,4,51);OLED_ShowCharChinese(3,6,53);
// /* ----------------------------------------------------------------------------------------------------------------- */		
// 		  printf("0=< ID <=99  \r\n");
// 		  Gui_DrawFont_GBK16(16,15,GREEN,WHITE,"              ");
// 		  Gui_DrawFont_GBK16(16,8,RED,WHITE,"K2:ID+,K4:ID-");
// 		  Gui_DrawFont_GBK16(16,28,RED,WHITE,"请按 K3 保存");
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
// 			 sprintf(str, " 指纹ID: %d ",ID_NUM);	
// 			 Gui_DrawFont_GBK16(16,105,RED,WHITE,(u8*)str);
// 				OLED_ShowString(4,1,"ID:");
// 				OLED_ShowNum(4,4,ID_NUM,3);
// /* ----------------------------------------------------------------------------------------------------------------- */				
//       }
//       key_num = 0;
// 			printf("储存模板 \r\n");
// 			printf("ID = %d \r\n",ID_NUM);
//       ensure = PS_StoreChar(CharBuffer2, ID_NUM); //储存模板
//       if(ensure == 0x00)
//       {				
// 				Lcd_Clear(WHITE);
// 				Gui_DrawFont_GBK16(16,10,GREEN,WHITE,"录入指纹成功");
// 					OLED_Clear();
// 				  //OLED_ShowString(1,1,"OK!!!");
// 	        OLED_ShowCharChinese(1,1,1);OLED_ShowCharChinese(1,3,3);
// 	        OLED_ShowCharChinese(1,5,5);OLED_ShowCharChinese(1,7,7);
// 				  OLED_ShowCharChinese(1,9,37);OLED_ShowCharChinese(1,11,39);
// 					OLED_ShowString(2,1,"K4:");
// 					OLED_ShowCharChinese(2,4,47);OLED_ShowCharChinese(2,6,49);				 
// /* ----------------------------------------------------------------------------------------------------------------- */				
// 				Gui_DrawFont_GBK16(32,30,GREEN,WHITE,"感谢使用");
// 				Gui_DrawFont_GBK16(16,50,GREEN,WHITE,"指纹识别系统");
// 				printf("录入指纹成功 \r\n");
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
//     if(i == 10)                            //超过5次没有按手指则退出
//     {
// 			Gui_DrawFont_GBK16(35,15,RED,WHITE,"验证超时");
// 			delay_ms(1500);
// 			Redraw_Mainmenu();
//       break;
//     }
//   }
// }

// SysPara AS608Para;//指纹模块AS608参数
// /**********************************************************************
// * @Function      ：void press_FR(void)
// * @Features      ：刷指纹
// * @InportFunction：No                
// * @ExportFunction：No
// * @Author        ：TXD
// * @ModifiedDate  ：2020.12.29
// * @Remarks       ：No
// **********************************************************************/
// void press_FR(void)
// {
//   SearchResult seach;
//   u8 ensure;
//   char str[20];
// 	Gui_DrawFont_GBK16(35,15,RED,WHITE,"请按手指");
// 	showimage(gImage_Finger65,33,40,97,104,65,65);    //指纹图片显示示例
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
// 			printf("获取图像成功 \r\n");
//       ensure = PS_GenChar(CharBuffer1);
// 				OLED_Clear();
// 	      //OLED_ShowString(1,1,"gain success");
// 			  OLED_ShowCharChinese(1,1,55);OLED_ShowCharChinese(1,3,57);
// 			  OLED_ShowCharChinese(1,5,59);OLED_ShowCharChinese(1,7,61);
// 			  OLED_ShowCharChinese(1,9,37);OLED_ShowCharChinese(1,11,39);
// /* ----------------------------------------------------------------------------------------------------------------- */			
//       if(ensure == 0x00) 
//       {
// 				printf("生成特征成功 \r\n");
//         ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);				
// 				  //OLED_ShowString(2,1,"matching--");
// 				  OLED_ShowCharChinese(2,1,41);OLED_ShowCharChinese(2,3,37);
// 			    OLED_ShowCharChinese(2,5,63);OLED_ShowCharChinese(2,7,65);
// 			    OLED_ShowCharChinese(2,9,37);OLED_ShowCharChinese(2,11,39);			
// /* ----------------------------------------------------------------------------------------------------------------- */				
//         if(ensure == 0x00) 
//         {
// 					printf("验证成功 \r\n");
// 					Lcd_Clear(WHITE);
//           Gui_DrawFont_GBK16(16,15,GREEN,WHITE,"指纹验证成功");
// 			    OLED_ShowCharChinese(3,1,13);OLED_ShowCharChinese(3,3,15);
// 			    OLED_ShowCharChinese(3,5,37);OLED_ShowCharChinese(3,7,39);							
// 					 OLED_ShowString(3,10,"ID:");
// /* ----------------------------------------------------------------------------------------------------------------- */					
//           sprintf(str, " 指纹ID:%d  ", seach.pageID);				
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
// 					Gui_DrawFont_GBK16(16,15,RED,WHITE,"指纹验证失败");
// 					Gui_DrawFont_GBK16(24,105,RED,WHITE,"查无此指纹");
// 					printf("验证失败 \r\n");
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
// 	 Redraw_Mainmenu();//绘制主菜单(部分内容由于分辨率超出物理值可能无法显示)	
	
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
// 	Gui_DrawFont_GBK16(16,11,RED,WHITE,"K1:返回");
// 	  OLED_Clear();
// 	  OLED_ShowString(1,1,"K1:");
// 	  OLED_ShowCharChinese(1,4,47);OLED_ShowCharChinese(1,6,49);
// /* ----------------------------------------------------------------------------------------------------------------- */	
	
// 	DisplayButtonUp(15,32,115,52); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,33,RED,WHITE,"K2:上一个ID ");
// 	  OLED_ShowString(2,1,"K4:ID+");	
// /* ----------------------------------------------------------------------------------------------------------------- */	
	
// 	DisplayButtonUp(15,54,115,74); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,55,RED,WHITE,"K3:确认");
// 	  OLED_ShowString(3,1,"K3:");	
// 	  OLED_ShowCharChinese(3,4,67);OLED_ShowCharChinese(3,6,69);
// /* ----------------------------------------------------------------------------------------------------------------- */	
	
// 	DisplayButtonUp(15,76,115,96); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,77,RED,WHITE,"K4:下一个ID");
// 	  OLED_ShowString(2,8,"K2:ID-");
// /* ----------------------------------------------------------------------------------------------------------------- */		
	
// 	DisplayButtonUp(15,98,115,118); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,99,RED,WHITE,"K5:清空指纹库");
// 	  OLED_ShowString(4,1,"K5:");
// 	  OLED_ShowCharChinese(4,4,71);OLED_ShowCharChinese(4,6,73);
// 		OLED_ShowCharChinese(4,8,5);OLED_ShowCharChinese(4,10,7);
// 		OLED_ShowCharChinese(4,12,75);
// /* ----------------------------------------------------------------------------------------------------------------- */		
// 	delay_ms(1500);
	
// 	Lcd_Clear(WHITE);
// 	DisplayButtonUp(15,10,115,30); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,11,RED,WHITE,"K2:上一个ID");
	
// 	DisplayButtonUp(15,32,115,52); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,33,RED,WHITE,"K4:下一个ID ");
	
// 	DisplayButtonUp(15,76,115,96); //x1,y1,x2,y2
// 	Gui_DrawFont_GBK16(16,77,RED,WHITE,"K3:确认");
	
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
// 					goto MENU ;             //返回主页面
// 				if(key_num == 5) 
// 				{
// 					key_num = 0;
// 					ensure = PS_Empty();   //清空指纹库
// 					if(ensure == 0)
// 					{					
// 						Lcd_Clear(WHITE);
// 						Gui_DrawFont_GBK16(10,15,GREEN,WHITE,"清空指纹库成功");
// 						  		OLED_Clear();
// 	                OLED_ShowString(1,1,"succeed");
// /* ----------------------------------------------------------------------------------------------------------------- */						
// 						printf("清空指纹库成功 \r\n");
// 					}
// 					else
// 						ShowErrMessage(ensure);
// 						delay_ms(1500);
// 					goto MENU ;             //返回主页面
// 				}

// 				sprintf(DelStr, "指纹ID: %d ",ID_NUM);	
// 				DisplayButtonUp(15,54,115,74); //x1,y1,x2,y2
// 	      Gui_DrawFont_GBK16(16,55,RED,WHITE,(u8*)DelStr);
// 				printf("ID_NUM = %d \r\n",ID_NUM);
				  
// 				  OLED_ShowString(3,10,"ID:");
// 				  OLED_ShowNum(3,13,ID_NUM,3);
// /* ----------------------------------------------------------------------------------------------------------------- */				
// 			}

	
//   ensure = PS_DeletChar(ID_NUM, 1); //删除单个指纹
//   if(ensure == 0)
//   {
// 		Lcd_Clear(WHITE);
// 		sprintf(DelStr, "删除指纹ID: %d ",ID_NUM);	
// 		Gui_DrawFont_GBK16(16,55,RED,WHITE,(u8*)DelStr);
// 		Gui_DrawFont_GBK16(32,75,GREEN,WHITE,"删除成功");
// 		printf("删除成功 \r\n");
// 		  OLED_Clear();
// 		  OLED_ShowString(1,1,"succeed OK");
// /* ----------------------------------------------------------------------------------------------------------------- */		
//   }
//   else
//     ShowErrMessage(ensure);
//     delay_ms(1500);
// MENU:
//    Lcd_Clear(WHITE);
// 	 Redraw_Mainmenu();//绘制主菜单(部分内容由于分辨率超出物理值可能无法显示)	
// 	 MAIN_OLED();
//    key_num = 0;
// }
