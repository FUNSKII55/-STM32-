///*
// * oled.c
// *
// *  Created on: 2022��7��24��
// *      Author: Unicorn_Li
// */
//#include "oled.h"
//#include "FreeRTOS.h"
//#include "task.h"
///**********************************************************
// * 初始化,OLED屏初始化,详细说明见注释
// ***********************************************************/
//uint8_t CMD_Data[]={
//0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,0xA1, 0xC8, 0xDA,

//0x12, 0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6,0x8D, 0x14,

//0xAF};



///**
// * @function: void OLED_Init(void)
// * @description: OLED初始化
// * @return {*}
// */
//void OLED_Init(void)
//{
//	vTaskDelay(200);

//	uint8_t i = 0;
//	for(i=0; i<23; i++)
//	{
//		OLED_WR_CMD(CMD_Data[i]);
//	}
//	
//}

///**
// * @function: void OLED_WR_CMD(uint8_t cmd)
// * @description: 向设备写入命令
// * @param {uint8_t} cmd 芯片手册规定的命令
// * @return {*}
// */
//void OLED_WR_CMD(uint8_t cmd)
//{
//	HAL_I2C_Mem_Write(&hi2c2 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,0x100);
//}

///**
// * @function: void OLED_WR_DATA(uint8_t data)
// * @description: 向设备写入数据
// * @param {uint8_t} data 数据
// * @return {*}
// */
//void OLED_WR_DATA(uint8_t data)
//{
//	HAL_I2C_Mem_Write(&hi2c2 ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&data,1,0x100);
//}

///**
// * @function: void OLED_On(void)
// * @description: 全屏显示

// * @return {*}
// */
//void OLED_On(void)
//{
//	uint8_t i,n;
//	for(i=0;i<8;i++)
//	{
//		OLED_WR_CMD(0xb0+i);    //设置页地址（0~7页）
//		OLED_WR_CMD(0x00);      //设置显示位置—列低地址
//		OLED_WR_CMD(0x10);      //设置显示位置—列高地址
//		for(n=0;n<128;n++)
//			OLED_WR_DATA(1);
//	}
//}


///**
// * @function: OLED_Clear(void)
// * @description: 清屏,清完屏,整个屏幕是黑色的!和没初始化一样!!!
// * @return {*}
// */
//void OLED_Clear(void)
//{
//	uint8_t i,n;
//	for(i=0;i<8;i++)
//	{
//		OLED_WR_CMD(0xb0+i);    //设置页地址（0~7页）
//		OLED_WR_CMD(0x00);      //设置显示位置—列低地址
//		OLED_WR_CMD(0x10);      //设置显示位置—列高地址
//		for(n=0;n<128;n++)
//			OLED_WR_DATA(0);
//	}
//}

///**
// * @function: void OLED_Display_On(void)
// * @description: 开启OLED显示
// * @return {*}
// */
//void OLED_Display_On(void)
//{
//	OLED_WR_CMD(0X8D);  //SET DCDC命令
//	OLED_WR_CMD(0X14);  //DCDC ON
//	OLED_WR_CMD(0XAF);  //DISPLAY ON,开启显示
//}


///**
// * @function: void OLED_Display_Off(void)
// * @description: 关闭OLED显示
// * @return {*}
// */
//void OLED_Display_Off(void)
//{
//	OLED_WR_CMD(0X8D);  //SET DCDC命令
//	OLED_WR_CMD(0X10);  //DCDC OFF
//	OLED_WR_CMD(0XAE);  //DISPLAY OFF，关闭显示
//}

///**
// * @function: void OLED_Set_Pos(uint8_t x, uint8_t y)
// * @description: 设置起始点坐标
// * @param {uint8_t} x,y
// * @return {*}
// */
//void OLED_Set_Pos(uint8_t x, uint8_t y)
//{
//	OLED_WR_CMD(0xb0+y);	//设置页地址（0~7页）
//	OLED_WR_CMD(((x&0xf0)>>4)|0x10); //设置显示位置—列高地址
//	OLED_WR_CMD(x&0x0f);	//设置显示位置—列低地址
//}


///**
// * @function: unsigned int oled_pow(uint8_t m,uint8_t n)
// * @description: m^n计算
// * @param {uint8_t} m,n
// * @return {unsigned int} result
// */
//unsigned int oled_pow(uint8_t m,uint8_t n)
//{
//	unsigned int result=1;
//	while(n--)result*=m;
//	return result;
//}

///**
// * @function: void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size,uint8_t Color_Turn)
// * @description: 从OLED12864的指定位置开始显示一个字符
// * @param {uint8_t} x字符开始显示的列坐标
// * @param {uint8_t} y字符开始显示的行坐标
// * @param {uint8_t} chr要显示的字符
// * @param {uint8_t} Char_Size字符显示的大小,选择 16/12
// * @param {uint8_t} Color_Turn是否反显(1为反显,0为正显)
// * @return {*}
// */
//void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size,uint8_t Color_Turn)
//{
//	unsigned char c=0,i=0;
//		c=chr-' ';//获取偏移后的字符值
//		if(x>128-1){x=0;y=y+2;}
//		if(Char_Size ==16)
//		{
//			OLED_Set_Pos(x,y);
//			for(i=0;i<8;i++)
//				{
//				  if(Color_Turn)
//					  OLED_WR_DATA(~F8X16[c*16+i]);
//				  else
//					  OLED_WR_DATA(F8X16[c*16+i]);
//				}
//			OLED_Set_Pos(x,y+1);
//			for(i=0;i<8;i++)
//			    {
//				  if(Color_Turn)
//					  OLED_WR_DATA(~F8X16[c*16+i+8]);
//				  else
//					  OLED_WR_DATA(F8X16[c*16+i+8]);
//			    }

//			}
//	     else
//	     {
//				OLED_Set_Pos(x,y);
//				for(i=0;i<6;i++)
//			    {
//				  if(Color_Turn)
//					  OLED_WR_DATA(~F6x8[c][i]);
//				  else
//					  OLED_WR_DATA(F6x8[c][i]);
//			    }
//		  }
//}

///**
// * @function: void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_tChar_Size, uint8_t Color_Turn)
// * @description: 从OLED12864的指定位置开始显示字符串
// * @param {uint8_t} x字符串开始显示的列坐标
// * @param {uint8_t} y字符串开始显示的行坐标
// * @param {uint8_t} *chr要显示的字符串
// * @param {uint8_t} Char_Size字符串显示的大小,选择 16/12,16为8X16,12为6x8
// * @param {uint8_t} Color_Turn是否反显(1为反显,0为正显)
// * @return {*}
// */
//void OLED_ShowString(uint8_t x,uint8_t y,char*chr,uint8_t Char_Size, uint8_t Color_Turn)
//{
//	uint8_t  j=0;
//	while (chr[j]!='\0')
//	{		OLED_ShowChar(x,y,chr[j],Char_Size, Color_Turn);
//			if (Char_Size == 12) //6X8字体中间显示6个字符
//				x += 6;
//			else  //8X16字体中间显示8个字符
//				x += 8;

//			if (x > 122 && Char_Size==12) //TextSize6x8最后一行显示完，下一行开始
//			{
//				x = 0;
//				y++;
//			}
//			if (x > 120 && Char_Size== 16) //TextSize8x16最后一行显示完，下一行开始
//			{
//				x = 0;
//				y++;
//			}
//			j++;
//	}
//}

///**
// * @function: void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2, Color_Turn)
// * @description: 显示数字
// * @param {uint8_t} x数字开始显示的列坐标
// * @param {uint8_t} y数字开始显示的行坐标
// * @param {unsigned int} num:要显示的数字
// * @param {uint8_t } len:数字的位数
// * @param {uint8_t} size2:数字显示的数据大小,选择 16/12,16为8X16,12为6x8
// * @param {uint8_t} Color_Turn是否反显(1为反显,0为正显)
// * @return {*}
// */
//void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2, uint8_t Color_Turn)
//{
//	uint8_t t,temp;
//	uint8_t enshow=0;
//	for(t=0;t<len;t++)
//	{
//		temp=(num/oled_pow(10,len-t-1))%10;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				OLED_ShowChar(x+(size2/2)*t,y,' ',size2, Color_Turn);
//				continue;
//			}else enshow=1;

//		}
//	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2, Color_Turn);
//	}
//}


///**
// * @function: void OLED_Showdecimal(uint8_t x,uint8_t y,float num,uint8_t z_len,uint8_t f_len,uint8_t size2, uint8_t Color_Turn)
// * @description: 显示浮点数
// * @param {uint8_t} x浮点数开始显示的列坐标
// * @param {uint8_t} y浮点数开始显示的行坐标
// * @param {float} num:要显示的浮点数
// * @param {uint8_t } z_ len:整数部分的位数
// * @param {uint8_t } f_len:小数部分的位数
// * @param {uint8_t} size2:浮点数显示的数据大小,选择 16/12,16为8X16,12为6x8
// * @param {uint8_t} Color_Turn是否反显(1为反显,0为正显)
// * @return {*}
// */
//void OLED_Showdecimal(uint8_t x,uint8_t y,float num,uint8_t z_len,uint8_t f_len,uint8_t size2, uint8_t Color_Turn)
//{
//	uint8_t t,temp,i=0;//i为小数点标志位
//	uint8_t enshow;
//	int z_temp,f_temp;
//	if(num<0)
//	{
//		z_len+=1;
//		i=1;
//		num=-num;
//	}
//	z_temp=(int)num;
//	//整数部分
//	for(t=0;t<z_len;t++)
//	{
//		temp=(z_temp/oled_pow(10,z_len-t-1))%10;
//		if(enshow==0 && t<(z_len-1))
//		{
//			if(temp==0)
//			{
//				OLED_ShowChar(x+(size2/2)*t,y,' ',size2, Color_Turn);
//				continue;
//			}
//			else
//			enshow=1;
//		}
//		OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2, Color_Turn);
//	}
//	//小数点
//	OLED_ShowChar(x+(size2/2)*(z_len),y,'.',size2, Color_Turn);

//	f_temp=(int)((num-z_temp)*(oled_pow(10,f_len)));
//  //小数部分
//	for(t=0;t<f_len;t++)
//	{
//		temp=(f_temp/oled_pow(10,f_len-t-1))%10;
//		OLED_ShowChar(x+(size2/2)*(t+z_len)+5,y,temp+'0',size2, Color_Turn);
//	}
//	if(i==1)//如果是负数，在前面加一个负号
//	{
//		OLED_ShowChar(x,y,'-',size2, Color_Turn);
//		i=0;
//	}
//}



///**
// * @function: void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no, uint8_t Color_Turn)
// * @description: 从OLED的指定位置开始显示16X16汉字
// * @param {uint8_t} x汉字开始显示的列坐标
// * @param {uint8_t} y汉字开始显示的行坐标
// * @param {uint8_t} no要显示的汉字序号
// * @param {uint8_t} Color_Turn是否反显(1为反显,0为正显)
// * @return {*}
// */
//void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no, uint8_t Color_Turn)
//{
//	uint8_t t=0;
//	OLED_Set_Pos(x,y);
//    for(t=0;t<16;t++)
//		{
//				if (Color_Turn)
//					OLED_WR_DATA(~Hzk[2*no][t]); //显示汉字的上半部分
//				else
//					OLED_WR_DATA(Hzk[2*no][t]); //显示汉字的上半部分
//        }

//		OLED_Set_Pos(x,y+1);
//    for(t=0;t<16;t++)
//		{
//				if (Color_Turn)
//					OLED_WR_DATA(~Hzk[2*no+1][t]); //显示汉字的下半部分
//				else
//					OLED_WR_DATA(Hzk[2*no+1][t]);//显示汉字的下半部分

//         }
//}

///**
// * @function: void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t *  BMP,uint8_t Color_Turn)
// * @description: 从OLED的指定位置开始显示BMP图片
// * @param {uint8_t} x0图片开始显示的列坐标  x0:0~127
// * @param {uint8_t} y0图片开始显示的行坐标  y0:0~7
// * @param {uint8_t} x1图片结束显示的列坐标  x1:1~128
// * @param {uint8_t} y1图片结束显示的行坐标  y1:1~8
// * @param {uint8_t} *BMP要显示的图片数据
// * @param {uint8_t} Color_Turn是否反显(1为反显,0为正显)
// * @return {*}
// */
//void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t *  BMP,uint8_t Color_Turn)
//{
//   uint32_t j = 0;
//   uint8_t x = 0, y = 0;

//  if(y1%8==0)
//		y = y1/8;
//  else
//		y = y1/8 + 1;
//	for(y=y0;y<y1;y++)
//	{
//		OLED_Set_Pos(x0,y);
//    for(x=x0;x<x1;x++)
//		{
//			if (Color_Turn)
//				OLED_WR_DATA(~BMP[j++]);//显示图片
//			else
//				OLED_WR_DATA(BMP[j++]);//显示图片

//		}
//	}
//}


///**
// * @function: void OLED_HorizontalShift(uint8_t direction)
// * @description: 屏幕水平全屏滚动
// * @param {uint8_t} direction			LEFT	   0x27     	RIGHT  0x26
// * @return {*}
// */
//void OLED_HorizontalShift(uint8_t direction)

//{
//	OLED_WR_CMD(0x2e);//停止滚动
//	OLED_WR_CMD(direction);//设置滚动方向
//	OLED_WR_CMD(0x00);//设置滚动起始页
//	OLED_WR_CMD(0x00);//设置滚动结束页
//	OLED_WR_CMD(0x07);//设置滚动速度
//    //  0x00-5页 0x01-64页 0x02-128页 0x03-256页 0x04-3页 0x05-4页 0x06-25页 0x07-2页
//	OLED_WR_CMD(0x07);//设置滚动结束页
//	OLED_WR_CMD(0x00);//设置滚动起始页
//	OLED_WR_CMD(0xff);//设置滚动结束页
//	OLED_WR_CMD(0x2f);//开启滚动-0x2f，关闭滚动-0x2e，需要重写
//}

///**
// * @function: void OLED_Some_HorizontalShift(uint8_t direction,uint8_t start,uint8_t end)
// * @description: 屏幕部分水平滚动
// * @param {uint8_t} direction			LEFT	   0x27     	RIGHT  0x26
// * @param {uint8_t} start 开始页  0x00~0x07
// * @param {uint8_t} end  结束页  0x01~0x07
// * @return {*}
// */
//void OLED_Some_HorizontalShift(uint8_t direction,uint8_t start,uint8_t end)
//{
//	OLED_WR_CMD(0x2e);//停止滚动
//	OLED_WR_CMD(direction);//设置滚动方向
//	OLED_WR_CMD(0x00);//设置滚动起始页
//	OLED_WR_CMD(start);//设置滚动结束页
//	OLED_WR_CMD(0x07);//设置滚动速度,0x07表示速度为2页
//	OLED_WR_CMD(end);//设置滚动结束页
//	OLED_WR_CMD(0x00);//设置滚动起始页
//	OLED_WR_CMD(0xff);//设置滚动结束页
//	OLED_WR_CMD(0x2f);//开启滚动-0x2f，关闭滚动-0x2e，需要重写

//}

///**
// * @function: void OLED_VerticalAndHorizontalShift(uint8_t direction)
// * @description: 屏幕垂直水平全屏滚动
// * @param {uint8_t} direction				垂直滚动	 0x29
// *                                                            垂直滚动   0x2A
// * @return {*}
// */
//void OLED_VerticalAndHorizontalShift(uint8_t direction)
//{
//	OLED_WR_CMD(0x2e);//停止滚动
//	OLED_WR_CMD(direction);//设置滚动方向
//	OLED_WR_CMD(0x01);//设置垂直偏移
//	OLED_WR_CMD(0x00);//设置滚动起始页
//	OLED_WR_CMD(0x07);//设置滚动速度
//	OLED_WR_CMD(0x07);//设置滚动结束页
//	OLED_WR_CMD(0x01);//设置垂直偏移
//	OLED_WR_CMD(0x00);//设置滚动起始页
//	OLED_WR_CMD(0xff);//设置滚动结束页
//	OLED_WR_CMD(0x2f);//开启滚动-0x2f，关闭滚动-0x2e，需要重写
//}

///**
// * @function: void OLED_DisplayMode(uint8_t mode)
// * @description: 屏幕显示模式
// * @param {uint8_t} direction			ON	0xA7  开启
// *                                                          OFF	0xA6	默认正常显示
// * @return {*}
// */
//void OLED_DisplayMode(uint8_t mode)
//{
//	OLED_WR_CMD(mode);
//}

///**
// * @function: void OLED_IntensityControl(uint8_t intensity)
// * @description: 屏幕亮度控制
// * @param  {uint8_t} intensity	0x00~0xFF,RESET=0x7F
// * @return {*}
// */
//void OLED_IntensityControl(uint8_t intensity)
//{
//	OLED_WR_CMD(0x81);
//	OLED_WR_CMD(intensity);
//}


