#include "stm32f10x.h"
#include "OLED_I2C.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

//ensure your OLED size
uint8_t OLED_DisplayBuf[8][128];

struct OLED_I2C
{
	I2C_TypeDef* I2Cx;
	GPIO_TypeDef* GPIOx;
	uint16_t SCl;
	uint16_t SDA;
}OLED_I2C_BUS;
uint8_t EN_HI2C = 0;

void OLED_W_SCL(uint8_t BitValue){
	//GPIO_WriteBit(OLED_I2C_BUS.GPIOx, OLED_I2C_BUS.SCl, (BitAction)BitValue);//set your SCL pin
	 if (BitValue != Bit_RESET) OLED_I2C_BUS.GPIOx->BSRR = OLED_I2C_BUS.SCl;
	 else OLED_I2C_BUS.GPIOx->BRR = OLED_I2C_BUS.SCl; 	
	//if your MCU speed is too fast ,you can delay for a while
}
void OLED_W_SDA(uint8_t BitValue){
	//GPIO_WriteBit(OLED_I2C_BUS.GPIOx, OLED_I2C_BUS.SDA, (BitAction)BitValue);//set your SDA pin
	if (BitValue != Bit_RESET) OLED_I2C_BUS.GPIOx->BSRR = OLED_I2C_BUS.SDA;
	else OLED_I2C_BUS.GPIOx->BRR = OLED_I2C_BUS.SDA;
	//if your MCU speed is too fast you can delay for a while
}

void OLED_GPIO_Init(GPIO_TypeDef* GPIOx,uint16_t SCl,uint16_t SDA){//set your SCL and SDA pin
	OLED_I2C_BUS.GPIOx = GPIOx;
	OLED_I2C_BUS.SCl = SCl;
	OLED_I2C_BUS.SDA = SDA;

	uint32_t i, j;
	for (i = 0; i < 1000; i ++)
	{
		for (j = 0; j < 1000; j ++);
	}
    if(GPIOx==GPIOA)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	else if(GPIOx==GPIOB)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	else if(GPIOx==GPIOC)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	else if(GPIOx==GPIOD)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	else if(GPIOx==GPIOE)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	else if(GPIOx==GPIOF)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	else if(GPIOx==GPIOG)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = SCl|SDA;
 	GPIO_Init(GPIOx, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

void OLED_I2C_Start(void){
	OLED_W_SDA(1);		//释放SDA，确保SDA为高电平
	OLED_W_SCL(1);		//释放SCL，确保SCL为高电平
	OLED_W_SDA(0);		//在SCL高电平期间，拉低SDA，产生起始信�?
	OLED_W_SCL(0);		//起�?�后把SCL也拉低，即为了占用总线，也为了方便总线时序的拼�?
}

void OLED_I2C_Stop(void){
	OLED_W_SDA(0);		//拉低SDA，确保SDA为低电平
	OLED_W_SCL(1);		//释放SCL，使SCL呈现高电�?
	OLED_W_SDA(1);		//在SCL高电平期间，释放SDA，产生终�?信号
}

void OLED_I2C_SendByte(uint8_t Byte){
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		//OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);	//释放SCL，从机在SCL高电平期间�?�取SDA
		OLED_W_SCL(0);	//拉低SCL，主机开始发送下一位数�?
	}
	
	OLED_W_SCL(1);		//额�?�的一�?时钟，不处理应答信号
	OLED_W_SCL(0);
}

void OLED_WriteCommand(uint8_t Command)
{
	if(EN_HI2C == 0){
		OLED_I2C_Start();				//I2C起�??
		OLED_I2C_SendByte(0x78);		//发送OLED的I2C从机地址
		OLED_I2C_SendByte(0x00);		//控制字节，给0x00，表示即将写命令
		OLED_I2C_SendByte(Command);		//写入指定的命�?
		OLED_I2C_Stop();				//I2C终�??
	}
	else{
		
	}
}

void OLED_WriteData(uint8_t *Data, uint8_t Count)
{
	if(EN_HI2C == 0){
		uint8_t i;
		
		OLED_I2C_Start();				//I2C起�??
		OLED_I2C_SendByte(0x78);		//发送OLED的I2C从机地址
		OLED_I2C_SendByte(0x40);		//控制字节，给0x40，表示即将写数据
		/*�?环Count次，进�?�连�?的数�?写入*/
		for (i = 0; i < Count; i ++)
		{
			OLED_I2C_SendByte(Data[i]);	//依�?�发送Data的每一�?数据
		}
		OLED_I2C_Stop();				//I2C终�??
	}else{
		
	}
}
/*����:OLED_Init(GPIOB,GPIO_Pin_10,GPIO_Pin_11);
GPIOx:ѡ��GPIO
SCl:ѡ��GPIO
SDA:ѡ��GPIO
*/
void OLED_Init(GPIO_TypeDef* GPIOx,uint16_t SCl,uint16_t SDA)
{
	OLED_GPIO_Init(GPIOx,SCl,SDA);
	EN_HI2C = 0;
	
	OLED_WriteCommand(0xAE);	//设置显示开�?/关闭�?0xAE关闭�?0xAF开�?
	OLED_WriteCommand(0xD5);	//设置显示时钟分�?�比/�?荡器频率
	OLED_WriteCommand(0x80);	//0x00~0xFF
	OLED_WriteCommand(0xA8);	//设置多路复用�?
	OLED_WriteCommand(0x3F);	//0x0E~0x3F
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);	//0x00~0x7F
	OLED_WriteCommand(0x40);	//设置显示开始�?�，0x40~0x7F
	OLED_WriteCommand(0xA1);	//设置左右方向�?0xA1正常�?0xA0左右反置
	OLED_WriteCommand(0xC8);	//设置上下方向�?0xC8正常�?0xC0上下反置
	OLED_WriteCommand(0xDA);	//设置COM引脚�?件配�?
	OLED_WriteCommand(0x12);	
	OLED_WriteCommand(0x81);	//设置对比�?
	OLED_WriteCommand(0xCF);	//0x00~0xFF
	OLED_WriteCommand(0xD9);	//设置预充电周�?
	OLED_WriteCommand(0xF1);
	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);
	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭
	OLED_WriteCommand(0xA6);	//设置正常/反色显示�?0xA6正常�?0xA7反色
	OLED_WriteCommand(0x8D);	//设置充电�?
	OLED_WriteCommand(0x14);
	OLED_WriteCommand(0xAF);	//开�?显示
	
	OLED_Clear();				//清空显存数组
	OLED_Update();				//更新显示，清屏，防�?�初始化后未显示内�?�时花屏
}

void OLED_SetCursor(uint8_t Page, uint8_t X)//(0~7,0~127)
{
	//X += 2;//1.3 inch OLED screen
	OLED_WriteCommand(0xB0 | Page);					//设置页位�?
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置�?4�?
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置�?4�?
}

uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//结果默�?�为1
	while (Y --)			//�?乘Y�?
	{
		Result *= X;		//每�?�把X�?乘到结果�?
	}
	return Result;
}

/**
  * �?    数：判断指定点是否在指定多边形内�?
  * �?    数：nvert 多边形的顶点�?
  * �?    数：vertx verty 包含多边形顶点的x和y坐标的数�?
  * �?    数：testx testy 测试点的X和y坐标
  * �? �? 值：指定点是否在指定多边形内�?�?1：在内部�?0：不在内�?
  */
uint8_t OLED_pnpoly(uint8_t nvert, int16_t *vertx, int16_t *verty, int16_t testx, int16_t testy)
{
	int16_t i, j, c = 0;
	
	for (i = 0, j = nvert - 1; i < nvert; j = i++)
	{
		if (((verty[i] > testy) != (verty[j] > testy)) &&
			(testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
		{
			c = !c;
		}
	}
	return c;
}

/**
  * �?    数：判断指定点是否在指定角度内部
  * �?    数：X Y 指定点的坐标
  * �?    数：StartAngle EndAngle 起�?��?�度和终止�?�度，范围：-180~180
  *           水平向右�?0度，水平向左�?180度或-180度，下方为�?�数，上方为负数，顺时针旋转
  * �? �? 值：指定点是否在指定角度内部�?1：在内部�?0：不在内�?
  */
uint8_t OLED_IsInAngle(int16_t X, int16_t Y, int16_t StartAngle, int16_t EndAngle)
{
	int16_t PointAngle;
	PointAngle = atan2(Y, X) / 3.14 * 180;	//计算指定点的弧度，并�?�?为�?�度表示
	if (StartAngle < EndAngle)	//起�?��?�度小于终�?��?�度的情�?
	{
		/*如果指定角度在起始终止�?�度之间，则判定指定点在指定角度*/
		if (PointAngle >= StartAngle && PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	else			//起�?��?�度大于于终止�?�度的情�?
	{
		/*如果指定角度大于起�?��?�度或者小于终止�?�度，则判定指定点在指定角度*/
		if (PointAngle >= StartAngle || PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	return 0;		//不满足以上条件，则判�?判定指定点不在指定�?�度
}

void OLED_Update(void)
{
	uint8_t j;
	/*遍历每一�?*/
	for (j = 0; j < 8; j ++)
	{
		/*设置光标位置为每一页的�?一�?*/
		OLED_SetCursor(j, 0);
		/*连续写入128�?数据，将显存数组的数�?写入到OLED�?�?*/
		OLED_WriteData(OLED_DisplayBuf[j], 128);
	}
}

void OLED_UpdateArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	int16_t j;
	int16_t Page, Page1;
	
	/*负数坐标在�?�算页地址时需要加一�?偏移*/
	/*(Y + Height - 1) / 8 + 1的目的是(Y + Height) / 8并向上取�?*/
	Page = Y / 8;
	Page1 = (Y + Height - 1) / 8 + 1;
	if (Y < 0)
	{
		Page -= 1;
		Page1 -= 1;
	}
	
	/*遍历指定区域涉及的相关页*/
	for (j = Page; j < Page1; j ++)
	{
		if (X >= 0 && X <= 127 && j >= 0 && j <= 7)		//超出屏幕的内容不显示
		{
			/*设置光标位置为相关页的指定列*/
			OLED_SetCursor(j, X);
			/*连续写入Width�?数据，将显存数组的数�?写入到OLED�?�?*/
			OLED_WriteData(&OLED_DisplayBuf[j][X], Width);
		}
	}
}

/**
  * �?    数：将OLED显存数组全部清零
  * �?    数：�?
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_Clear(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j ++)				//遍历8�?
	{
		for (i = 0; i < 128; i ++)			//遍历128�?
		{
			OLED_DisplayBuf[j][i] = 0x00;	//将显存数组数�?全部清零
		}
	}
}

/**
  * �?    数：将OLED显存数组部分清零
  * �?    数：X 指定区域左上角的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定区域左上角的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Width 指定区域的�?�度，范围：0~128
  * �?    数：Height 指定区域的高度，范围�?0~64
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	int16_t i, j;
	
	for (j = Y; j < Y + Height; j ++)		//遍历指定�?
	{
		for (i = X; i < X + Width; i ++)	//遍历指定�?
		{
			if (i >= 0 && i <= 127 && j >=0 && j <= 63)				//超出屏幕的内容不显示
			{
				OLED_DisplayBuf[j / 8][i] &= ~(0x01 << (j % 8));	//将显存数组指定数�?清零
			}
		}
	}
}

/**
  * �?    数：将OLED显存数组全部取反
  * �?    数：�?
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_Reverse(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j ++)				//遍历8�?
	{
		for (i = 0; i < 128; i ++)			//遍历128�?
		{
			OLED_DisplayBuf[j][i] ^= 0xFF;	//将显存数组数�?全部取反
		}
	}
}
	
/**
  * �?    数：将OLED显存数组部分取反
  * �?    数：X 指定区域左上角的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定区域左上角的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Width 指定区域的�?�度，范围：0~128
  * �?    数：Height 指定区域的高度，范围�?0~64
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	int16_t i, j;
	
	for (j = Y; j < Y + Height; j ++)		//遍历指定�?
	{
		for (i = X; i < X + Width; i ++)	//遍历指定�?
		{
			if (i >= 0 && i <= 127 && j >=0 && j <= 63)			//超出屏幕的内容不显示
			{
				OLED_DisplayBuf[j / 8][i] ^= 0x01 << (j % 8);	//将显存数组指定数�?取反
			}
		}
	}
}

/**
  * �?    数：OLED显示一�?字�??
  * �?    数：X 指定字�?�左上�?�的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定字�?�左上�?�的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Char 指定要显示的字�?�，范围：ASCII码可见字�?
  * �?    数：FontSize 指定字体大小
  *           范围：OLED_8X16		�?8像素，高16像素
  *                 OLED_6X8		�?6像素，高8像素
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize)
{
	if (FontSize == OLED_8X16)		//字体为�??8像素，高16像素
	{
		/*将ASCII字模库OLED_F8x16的指定数�?�?8*16的图像格式显�?*/
		OLED_ShowImage(X, Y, 8, 16, OLED_F8x16[Char - ' ']);
	}
	else if(FontSize == OLED_6X8)	//字体为�??6像素，高8像素
	{
		/*将ASCII字模库OLED_F6x8的指定数�?�?6*8的图像格式显�?*/
		OLED_ShowImage(X, Y, 6, 8, OLED_F6x8[Char - ' ']);
	}
}

/**
  * �?    数：OLED显示字�?�串
  * �?    数：X 指定字�?�串左上角的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定字�?�串左上角的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：String 指定要显示的字�?�串，范围：ASCII码可见字符组成的字�?�串
  * �?    数：FontSize 指定字体大小
  *           范围：OLED_8X16		�?8像素，高16像素
  *                 OLED_6X8		�?6像素，高8像素
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)		//遍历字�?�串的每�?字�??
		OLED_ShowChar(X + i * FontSize, Y, String[i], FontSize);
}

/**
  * �?    数：OLED显示数字（十进制，�?�整数）
  * �?    数：X 指定数字左上角的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定数字左上角的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Number 指定要显示的数字，范围：0~4294967295
  * �?    数：Length 指定数字的长度，范围�?0~10
  * �?    数：FontSize 指定字体大小
  *           范围：OLED_8X16		�?8像素，高16像素
  *                 OLED_6X8		�?6像素，高8像素
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//遍历数字的每一�?							
	{
		/*调用OLED_ShowChar函数，依次显示每�?数字*/
		/*Number / OLED_Pow(10, Length - i - 1) % 10 �?以十进制提取数字的每一�?*/
		/*+ '0' �?将数字转�?为字符格�?*/
		OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  * �?    数：OLED显示有�?�号数字（十进制，整数）
  * �?    数：X 指定数字左上角的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定数字左上角的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Number 指定要显示的数字，范围：-2147483648~2147483647
  * �?    数：Length 指定数字的长度，范围�?0~10
  * �?    数：FontSize 指定字体大小
  *           范围：OLED_8X16		�?8像素，高16像素
  *                 OLED_6X8		�?6像素，高8像素
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	uint32_t Number1;
	
	if (Number >= 0)						//数字大于等于0
	{
		OLED_ShowChar(X, Y, '+', FontSize);	//显示+�?
		Number1 = Number;					//Number1直接等于Number
	}
	else									//数字小于0
	{
		OLED_ShowChar(X, Y, '-', FontSize);	//显示-�?
		Number1 = -Number;					//Number1等于Number取负
	}
	
	for (i = 0; i < Length; i++)			//遍历数字的每一�?								
	{
		/*调用OLED_ShowChar函数，依次显示每�?数字*/
		/*Number1 / OLED_Pow(10, Length - i - 1) % 10 �?以十进制提取数字的每一�?*/
		/*+ '0' �?将数字转�?为字符格�?*/
		OLED_ShowChar(X + (i + 1) * FontSize, Y, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  * �?    数：OLED显示十六进制数字（十�?进制，�?�整数）
  * �?    数：X 指定数字左上角的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定数字左上角的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Number 指定要显示的数字，范围：0x00000000~0xFFFFFFFF
  * �?    数：Length 指定数字的长度，范围�?0~8
  * �?    数：FontSize 指定字体大小
  *           范围：OLED_8X16		�?8像素，高16像素
  *                 OLED_6X8		�?6像素，高8像素
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)		//遍历数字的每一�?
	{
		/*以十�?进制提取数字的每一�?*/
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		
		if (SingleNumber < 10)			//单个数字小于10
		{
			/*调用OLED_ShowChar函数，显示�?�数�?*/
			/*+ '0' �?将数字转�?为字符格�?*/
			OLED_ShowChar(X + i * FontSize, Y, SingleNumber + '0', FontSize);
		}
		else							//单个数字大于10
		{
			/*调用OLED_ShowChar函数，显示�?�数�?*/
			/*+ 'A' �?将数字转�?为从A开始的十六进制字�??*/
			OLED_ShowChar(X + i * FontSize, Y, SingleNumber - 10 + 'A', FontSize);
		}
	}
}

/**
  * �?    数：OLED显示二进制数字（二进制，正整数）
  * �?    数：X 指定数字左上角的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定数字左上角的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Number 指定要显示的数字，范围：0x00000000~0xFFFFFFFF
  * �?    数：Length 指定数字的长度，范围�?0~16
  * �?    数：FontSize 指定字体大小
  *           范围：OLED_8X16		�?8像素，高16像素
  *                 OLED_6X8		�?6像素，高8像素
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//遍历数字的每一�?	
	{
		/*调用OLED_ShowChar函数，依次显示每�?数字*/
		/*Number / OLED_Pow(2, Length - i - 1) % 2 �?以二进制提取数字的每一�?*/
		/*+ '0' �?将数字转�?为字符格�?*/
		OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(2, Length - i - 1) % 2 + '0', FontSize);
	}
}

/**
  * �?    数：OLED显示�?点数字（十进制，小数�?
  * �?    数：X 指定数字左上角的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定数字左上角的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Number 指定要显示的数字，范围：-4294967295.0~4294967295.0
  * �?    数：IntLength 指定数字的整数位长度，范围：0~10
  * �?    数：FraLength 指定数字的小数位长度，范围：0~9，小数进行四舍五入显�?
  * �?    数：FontSize 指定字体大小
  *           范围：OLED_8X16		�?8像素，高16像素
  *                 OLED_6X8		�?6像素，高8像素
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize)
{
	uint32_t PowNum, IntNum, FraNum;
	
	if (Number >= 0)						//数字大于等于0
	{
		OLED_ShowChar(X, Y, '+', FontSize);	//显示+�?
	}
	else									//数字小于0
	{
		OLED_ShowChar(X, Y, '-', FontSize);	//显示-�?
		Number = -Number;					//Number取负
	}
	
	/*提取整数部分和小数部�?*/
	IntNum = Number;						//直接赋值给整型变量，提取整�?
	Number -= IntNum;						//将Number的整数减掉，防�??之后将小数乘到整数时因数过大造成错�??
	PowNum = OLED_Pow(10, FraLength);		//根据指定小数的位数，�?定乘�?
	FraNum = round(Number * PowNum);		//将小数乘到整数，同时四舍五入，避免显示�??�?
	IntNum += FraNum / PowNum;				//若四舍五入造成了进位，则需要再加给整数
	
	OLED_ShowNum(X + FontSize, Y, IntNum, IntLength, FontSize);
	
	OLED_ShowChar(X + (IntLength + 1) * FontSize, Y, '.', FontSize);
	
	OLED_ShowNum(X + (IntLength + 2) * FontSize, Y, FraNum, FraLength, FontSize);
}
//(X_position , Y_position , Chinese)
void OLED_ShowChinese(int16_t X, int16_t Y, char *Chinese)
{
	uint8_t pChinese = 0;
	uint8_t pIndex;
	uint8_t i;
	char SingleChinese[OLED_CHN_CHAR_WIDTH + 1] = {0};
	
	for (i = 0; Chinese[i] != '\0'; i ++)		//遍历汉字�?
	{
		SingleChinese[pChinese] = Chinese[i];	//提取汉字串数�?到单�?汉字数组
		pChinese ++;							//计�?�自�?
		
		/*当提取�?�数到达OLED_CHN_CHAR_WIDTH时，即代表提取到了一�?完整的汉�?*/
		if (pChinese >= OLED_CHN_CHAR_WIDTH)
		{
			pChinese = 0;		//计�?�归�?
			
			/*遍历整个汉字字模库，寻找匹配的汉�?*/
			/*如果找到最后一�?汉字（定义为空字符串），则表示汉字未在字模库定义，停止�?�找*/
			for (pIndex = 0; strcmp(OLED_CF16x16[pIndex].Index, "") != 0; pIndex ++)
			{
				/*找到匹配的汉�?*/
				if (strcmp(OLED_CF16x16[pIndex].Index, SingleChinese) == 0)
				{
					break;		//跳出�?�?，�?�时pIndex的值为指定汉字的索�?
				}
			}
			
			/*将汉字字模库OLED_CF16x16的指定数�?�?16*16的图像格式显�?*/
			OLED_ShowImage(X + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * 16, Y, 16, 16, OLED_CF16x16[pIndex].Data);
		}
	}
}
//(X_posotion , Y_position , image_Width , image_Height , Image_array[])
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
	uint8_t i = 0, j = 0;
	int16_t Page, Shift;
	
	/*将图像所在区域清�?*/
	OLED_ClearArea(X, Y, Width, Height);
	
	/*遍历指定图像涉及的相关页*/
	/*(Height - 1) / 8 + 1的目的是Height / 8并向上取�?*/
	for (j = 0; j < (Height - 1) / 8 + 1; j ++)
	{
		/*遍历指定图像涉及的相关列*/
		for (i = 0; i < Width; i ++)
		{
			if (X + i >= 0 && X + i <= 127)		//超出屏幕的内容不显示
			{
				/*负数坐标在�?�算页地址和移位时需要加一�?偏移*/
				Page = Y / 8;
				Shift = Y % 8;
				if (Y < 0)
				{
					Page -= 1;
					Shift += 8;
				}
				
				if (Page + j >= 0 && Page + j <= 7)		//超出屏幕的内容不显示
				{
					/*显示图像在当前页的内�?*/
					OLED_DisplayBuf[Page + j][X + i] |= Image[j * Width + i] << (Shift);
				}
				
				if (Page + j + 1 >= 0 && Page + j + 1 <= 7)		//超出屏幕的内容不显示
				{					
					/*显示图像在下一页的内�??*/
					OLED_DisplayBuf[Page + j + 1][X + i] |= Image[j * Width + i] >> (8 - Shift);
				}
			}
		}
	}
}
/*
X position
Y pisition
FontSize OLED6X8,OLED8X16
type{%d(integer),%s(String),%f(float),%c(char),%o(8),%x(10)}
depend on your type
*/
void OLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...)
{
	char String[256];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	OLED_ShowString(X, Y, String, FontSize);
}

/**
  * �?    数：OLED在指定位�?画一�?�?
  * �?    数：X 指定点的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定点的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_DrawPoint(int16_t X, int16_t Y)
{
	if (X >= 0 && X <= 127 && Y >=0 && Y <= 63)		//超出屏幕的内容不显示
	{
		/*将显存数组指定位�?的一个Bit数据�?1*/
		OLED_DisplayBuf[Y / 8][X] |= 0x01 << (Y % 8);
	}
}

/**
  * �?    数：OLED获取指定位置点的�?
  * �?    数：X 指定点的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定点的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �? �? 值：指定位置点是否�?�于点亮状态，1：点�?�?0：熄�?
  */
uint8_t OLED_GetPoint(int16_t X, int16_t Y)
{
	if (X >= 0 && X <= 127 && Y >=0 && Y <= 63)		//超出屏幕的内容不读取
	{
		/*判断指定位置的数�?*/
		if (OLED_DisplayBuf[Y / 8][X] & 0x01 << (Y % 8))
		{
			return 1;	//�?1，返�?1
		}
	}
	
	return 0;		//否则，返�?0
}

void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1)
{
	int16_t x, y, dx, dy, d, incrE, incrNE, temp;
	int16_t x0 = X0, y0 = Y0, x1 = X1, y1 = Y1;
	uint8_t yflag = 0, xyflag = 0;
	
	if (y0 == y1)		//�?线单�?处理
	{
		/*0号点X坐标大于1号点X坐标，则交换两点X坐标*/
		if (x0 > x1) {temp = x0; x0 = x1; x1 = temp;}
		
		/*遍历X坐标*/
		for (x = x0; x <= x1; x ++)
		{
			OLED_DrawPoint(x, y0);	//依�?�画�?
		}
	}
	else if (x0 == x1)	//竖线单独处理
	{
		/*0号点Y坐标大于1号点Y坐标，则交换两点Y坐标*/
		if (y0 > y1) {temp = y0; y0 = y1; y1 = temp;}
		
		/*遍历Y坐标*/
		for (y = y0; y <= y1; y ++)
		{
			OLED_DrawPoint(x0, y);	//依�?�画�?
		}
	}
	else				//斜线
	{
		/*使用Bresenham算法画直线，�?以避免耗时的浮点运算，效率更高*/
		/*参考文档：https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
		/*参考教程：https://www.bilibili.com/video/BV1364y1d7Lo*/
		
		if (x0 > x1)	//0号点X坐标大于1号点X坐标
		{
			/*交换两点坐标*/
			/*交换后不影响画线，但�?画线方向由�??一、二、三、四象限变为�?一、四象限*/
			temp = x0; x0 = x1; x1 = temp;
			temp = y0; y0 = y1; y1 = temp;
		}
		
		if (y0 > y1)	//0号点Y坐标大于1号点Y坐标
		{
			/*将Y坐标取负*/
			/*取负后影响画线，但是画线方向由�??一、四象限变为�?一象限*/
			y0 = -y0;
			y1 = -y1;
			
			/*�?标志位yflag，�?�住当前变换，在后续实际画线时，再将坐标换回�?*/
			yflag = 1;
		}
		
		if (y1 - y0 > x1 - x0)	//画线斜率大于1
		{
			/*将X坐标与Y坐标互换*/
			/*互换后影响画线，但是画线方向由�??一象限0~90度范围变为�??一象限0~45度范�?*/
			temp = x0; x0 = y0; y0 = temp;
			temp = x1; x1 = y1; y1 = temp;
			
			/*�?标志位xyflag，�?�住当前变换，在后续实际画线时，再将坐标换回�?*/
			xyflag = 1;
		}
		
		/*以下为Bresenham算法画直�?*/
		/*算法要求，画线方向必须为�?一象限0~45度范�?*/
		dx = x1 - x0;
		dy = y1 - y0;
		incrE = 2 * dy;
		incrNE = 2 * (dy - dx);
		d = 2 * dy - dx;
		x = x0;
		y = y0;
		
		/*画起始点，同时判�?标志位，将坐标换回来*/
		if (yflag && xyflag){OLED_DrawPoint(y, -x);}
		else if (yflag)		{OLED_DrawPoint(x, -y);}
		else if (xyflag)	{OLED_DrawPoint(y, x);}
		else				{OLED_DrawPoint(x, y);}
		
		while (x < x1)		//遍历X轴的每个�?
		{
			x ++;
			if (d < 0)		//下一�?点在当前点东�?
			{
				d += incrE;
			}
			else			//下一�?点在当前点东北方
			{
				y ++;
				d += incrNE;
			}
			
			/*画每一�?点，同时判断标志位，将坐标换回来*/
			if (yflag && xyflag){OLED_DrawPoint(y, -x);}
			else if (yflag)		{OLED_DrawPoint(x, -y);}
			else if (xyflag)	{OLED_DrawPoint(y, x);}
			else				{OLED_DrawPoint(x, y);}
		}	
	}
}

/**
  * �?    数：OLED矩形
  * �?    数：X 指定矩形左上角的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定矩形左上角的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Width 指定矩形的�?�度，范围：0~128
  * �?    数：Height 指定矩形的高度，范围�?0~64
  * �?    数：IsFilled 指定矩形�?否填�?
  *           范围：OLED_UNFILLED		不填�?
  *                 OLED_FILLED			�?�?
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled)
{
	int16_t i, j;
	if (!IsFilled)		//指定矩形不填�?
	{
		/*遍历上下X坐标，画矩形上下两条�?*/
		for (i = X; i < X + Width; i ++)
		{
			OLED_DrawPoint(i, Y);
			OLED_DrawPoint(i, Y + Height - 1);
		}
		/*遍历左右Y坐标，画矩形左右两条�?*/
		for (i = Y; i < Y + Height; i ++)
		{
			OLED_DrawPoint(X, i);
			OLED_DrawPoint(X + Width - 1, i);
		}
	}
	else				//指定矩形�?�?
	{
		/*遍历X坐标*/
		for (i = X; i < X + Width; i ++)
		{
			/*遍历Y坐标*/
			for (j = Y; j < Y + Height; j ++)
			{
				/*在指定区域画点，�?充满矩形*/
				OLED_DrawPoint(i, j);
			}
		}
	}
}

/**
  * �?    数：OLED三�?�形
  * �?    数：X0 指定�?一�?�?点的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y0 指定�?一�?�?点的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：X1 指定�?二个�?点的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y1 指定�?二个�?点的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：X2 指定�?三个�?点的�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y2 指定�?三个�?点的纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：IsFilled 指定三�?�形�?否填�?
  *           范围：OLED_UNFILLED		不填�?
  *                 OLED_FILLED			�?�?
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled)
{
	int16_t minx = X0, miny = Y0, maxx = X0, maxy = Y0;
	int16_t i, j;
	int16_t vx[] = {X0, X1, X2};
	int16_t vy[] = {Y0, Y1, Y2};
	
	if (!IsFilled)			//指定三�?�形不填�?
	{
		/*调用画线函数，将三个点用直线连接*/
		OLED_DrawLine(X0, Y0, X1, Y1);
		OLED_DrawLine(X0, Y0, X2, Y2);
		OLED_DrawLine(X1, Y1, X2, Y2);
	}
	else					//指定三�?�形�?�?
	{
		/*找到三个点最小的X、Y坐标*/
		if (X1 < minx) {minx = X1;}
		if (X2 < minx) {minx = X2;}
		if (Y1 < miny) {miny = Y1;}
		if (Y2 < miny) {miny = Y2;}
		
		/*找到三个点最大的X、Y坐标*/
		if (X1 > maxx) {maxx = X1;}
		if (X2 > maxx) {maxx = X2;}
		if (Y1 > maxy) {maxy = Y1;}
		if (Y2 > maxy) {maxy = Y2;}
		
		/*最小最大坐标之间的矩形为可能需要填充的区域*/
		/*遍历此区域中所有的�?*/
		/*遍历X坐标*/		
		for (i = minx; i <= maxx; i ++)
		{
			/*遍历Y坐标*/	
			for (j = miny; j <= maxy; j ++)
			{
				/*调用OLED_pnpoly，判�?指定点是否在指定三�?�形之中*/
				/*如果�?，则画点，�?�果不在，则不做处理*/
				if (OLED_pnpoly(3, vx, vy, i, j)) {OLED_DrawPoint(i, j);}
			}
		}
	}
}

/**
  * �?    数：OLED画圆
  * �?    数：X 指定圆的圆心�?坐标，范围：-32768~32767，屏幕区域：0~127
  * �?    数：Y 指定圆的圆心纵坐标，范围�?-32768~32767，屏幕区域：0~63
  * �?    数：Radius 指定圆的半径，范围：0~255
  * �?    数：IsFilled 指定圆是否填�?
  *           范围：OLED_UNFILLED		不填�?
  *                 OLED_FILLED			�?�?
  * �? �? 值：�?
  * �?    明：调用此函数后，�?�想真�?�地呈现在屏幕上，还需调用更新函数
  */
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled)
{
	int16_t x, y, d, j;
	
	/*使用Bresenham算法画圆，可以避免耗时的浮点运算，效率更高*/
	/*参考文档：https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
	/*参考教程：https://www.bilibili.com/video/BV1VM4y1u7wJ*/
	
	d = 1 - Radius;
	x = 0;
	y = Radius;
	
	/*画每�?�?分之一圆弧的起始点*/
	OLED_DrawPoint(X + x, Y + y);
	OLED_DrawPoint(X - x, Y - y);
	OLED_DrawPoint(X + y, Y + x);
	OLED_DrawPoint(X - y, Y - x);
	
	if (IsFilled)		//指定圆填�?
	{
		/*遍历起�?�点Y坐标*/
		for (j = -y; j < y; j ++)
		{
			/*在指定区域画点，�?充部分圆*/
			OLED_DrawPoint(X, Y + j);
		}
	}
	
	while (x < y)		//遍历X轴的每个�?
	{
		x ++;
		if (d < 0)		//下一�?点在当前点东�?
		{
			d += 2 * x + 1;
		}
		else			//下一�?点在当前点东南方
		{
			y --;
			d += 2 * (x - y) + 1;
		}
		
		/*画每�?�?分之一圆弧的点*/
		OLED_DrawPoint(X + x, Y + y);
		OLED_DrawPoint(X + y, Y + x);
		OLED_DrawPoint(X - x, Y - y);
		OLED_DrawPoint(X - y, Y - x);
		OLED_DrawPoint(X + x, Y - y);
		OLED_DrawPoint(X + y, Y - x);
		OLED_DrawPoint(X - x, Y + y);
		OLED_DrawPoint(X - y, Y + x);
		
		if (IsFilled)	//指定圆填�?
		{
			/*遍历�?间部�?*/
			for (j = -y; j < y; j ++)
			{
				/*在指定区域画点，�?充部分圆*/
				OLED_DrawPoint(X + x, Y + j);
				OLED_DrawPoint(X - x, Y + j);
			}
			
			/*遍历两侧部分*/
			for (j = -x; j < x; j ++)
			{
				/*在指定区域画点，�?充部分圆*/
				OLED_DrawPoint(X - y, Y + j);
				OLED_DrawPoint(X + y, Y + j);
			}
		}
	}
}


void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled)
{
	int16_t x, y, j;
	int16_t a = A, b = B;
	float d1, d2;
	
	/*使用Bresenham算法画椭圆，�?以避免部分耗时的浮点运算，效率更高*/
	/*参考链接：https://blog.csdn.net/myf_666/article/details/128167392*/
	
	x = 0;
	y = b;
	d1 = b * b + a * a * (-b + 0.5);
	
	if (IsFilled)	//指定�?圆填�?
	{
		/*遍历起�?�点Y坐标*/
		for (j = -y; j < y; j ++)
		{
			/*在指定区域画点，�?充部分椭�?*/
			OLED_DrawPoint(X, Y + j);
			OLED_DrawPoint(X, Y + j);
		}
	}
	
	/*画椭圆弧的起始点*/
	OLED_DrawPoint(X + x, Y + y);
	OLED_DrawPoint(X - x, Y - y);
	OLED_DrawPoint(X - x, Y + y);
	OLED_DrawPoint(X + x, Y - y);
	
	/*画椭圆中间部�?*/
	while (b * b * (x + 1) < a * a * (y - 0.5))
	{
		if (d1 <= 0)		//下一�?点在当前点东�?
		{
			d1 += b * b * (2 * x + 3);
		}
		else				//下一�?点在当前点东南方
		{
			d1 += b * b * (2 * x + 3) + a * a * (-2 * y + 2);
			y --;
		}
		x ++;
		
		if (IsFilled)	//指定�?圆填�?
		{
			/*遍历�?间部�?*/
			for (j = -y; j < y; j ++)
			{
				/*在指定区域画点，�?充部分椭�?*/
				OLED_DrawPoint(X + x, Y + j);
				OLED_DrawPoint(X - x, Y + j);
			}
		}
		
		/*画椭圆中间部分圆�?*/
		OLED_DrawPoint(X + x, Y + y);
		OLED_DrawPoint(X - x, Y - y);
		OLED_DrawPoint(X - x, Y + y);
		OLED_DrawPoint(X + x, Y - y);
	}
	
	/*画椭圆两侧部�?*/
	d2 = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) - a * a * b * b;
	
	while (y > 0)
	{
		if (d2 <= 0)		//下一�?点在当前点东�?
		{
			d2 += b * b * (2 * x + 2) + a * a * (-2 * y + 3);
			x ++;
			
		}
		else				//下一�?点在当前点东南方
		{
			d2 += a * a * (-2 * y + 3);
		}
		y --;
		
		if (IsFilled)	//指定�?圆填�?
		{
			/*遍历两侧部分*/
			for (j = -y; j < y; j ++)
			{
				/*在指定区域画点，�?充部分椭�?*/
				OLED_DrawPoint(X + x, Y + j);
				OLED_DrawPoint(X - x, Y + j);
			}
		}
		
		/*画椭圆两侧部分圆�?*/
		OLED_DrawPoint(X + x, Y + y);
		OLED_DrawPoint(X - x, Y - y);
		OLED_DrawPoint(X - x, Y + y);
		OLED_DrawPoint(X + x, Y - y);
	}
}


void OLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled)
{
	int16_t x, y, d, j;
	
	d = 1 - Radius;
	x = 0;
	y = Radius;
	

	if (OLED_IsInAngle(x, y, StartAngle, EndAngle))	{OLED_DrawPoint(X + x, Y + y);}
	if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle)) {OLED_DrawPoint(X - x, Y - y);}
	if (OLED_IsInAngle(y, x, StartAngle, EndAngle)) {OLED_DrawPoint(X + y, Y + x);}
	if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle)) {OLED_DrawPoint(X - y, Y - x);}
	
	if (IsFilled)	//指定圆弧�?�?
	{
		/*遍历起�?�点Y坐标*/
		for (j = -y; j < y; j ++)
		{

			if (OLED_IsInAngle(0, j, StartAngle, EndAngle)) {OLED_DrawPoint(X, Y + j);}
		}
	}
	
	while (x < y)		//遍历X轴的每个�?
	{
		x ++;
		if (d < 0)		//下一�?点在当前点东�?
		{
			d += 2 * x + 1;
		}
		else			//下一�?点在当前点东南方
		{
			y --;
			d += 2 * (x - y) + 1;
		}
		
		/*在画圆的每个点时，判�?指定点是否在指定角度内，�?，则画点，不�?，则不做处理*/
		if (OLED_IsInAngle(x, y, StartAngle, EndAngle)) {OLED_DrawPoint(X + x, Y + y);}
		if (OLED_IsInAngle(y, x, StartAngle, EndAngle)) {OLED_DrawPoint(X + y, Y + x);}
		if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle)) {OLED_DrawPoint(X - x, Y - y);}
		if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle)) {OLED_DrawPoint(X - y, Y - x);}
		if (OLED_IsInAngle(x, -y, StartAngle, EndAngle)) {OLED_DrawPoint(X + x, Y - y);}
		if (OLED_IsInAngle(y, -x, StartAngle, EndAngle)) {OLED_DrawPoint(X + y, Y - x);}
		if (OLED_IsInAngle(-x, y, StartAngle, EndAngle)) {OLED_DrawPoint(X - x, Y + y);}
		if (OLED_IsInAngle(-y, x, StartAngle, EndAngle)) {OLED_DrawPoint(X - y, Y + x);}
		
		if (IsFilled)	//指定圆弧�?�?
		{
			/*遍历�?间部�?*/
			for (j = -y; j < y; j ++)
			{
				/*在填充圆的每�?点时，判�?指定点是否在指定角度内，�?，则画点，不�?，则不做处理*/
				if (OLED_IsInAngle(x, j, StartAngle, EndAngle)) {OLED_DrawPoint(X + x, Y + j);}
				if (OLED_IsInAngle(-x, j, StartAngle, EndAngle)) {OLED_DrawPoint(X - x, Y + j);}
			}
			
			/*遍历两侧部分*/
			for (j = -x; j < x; j ++)
			{
				/*在填充圆的每�?点时，判�?指定点是否在指定角度内，�?，则画点，不�?，则不做处理*/
				if (OLED_IsInAngle(-y, j, StartAngle, EndAngle)) {OLED_DrawPoint(X - y, Y + j);}
				if (OLED_IsInAngle(y, j, StartAngle, EndAngle)) {OLED_DrawPoint(X + y, Y + j);}
			}
		}
	}
}
