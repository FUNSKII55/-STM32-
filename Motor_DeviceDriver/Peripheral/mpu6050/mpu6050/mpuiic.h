#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "stm32f1xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32F103开发板
//MPU6050 IIC通信 驱动	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/7/28
//版本：V1.0
//版权所有，盗版必究
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
	   		   

#define u8 uint8_t
#define u16 uint16_t

//IO方向设置
#define MPU_SDA_IN()  {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=8<<12;}
#define MPU_SDA_OUT() {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=3<<12;}

//IO操作函数	 
#define MPU_IIC_SCL    PAout(2) 		//SCL
#define MPU_IIC_SDA    PAout(3) 		//SDA	 
#define MPU_READ_SDA   PAin(3) 		//读取SDA 

//IIC所有操作函数
void MPU_IIC_Delay(void);				//MPU IIC延时函数
void MPU_IIC_Init(void);                //初始化IIC的IO口				 
void MPU_IIC_Start(void);				//产生IIC起始信号
void MPU_IIC_Stop(void);	  			//产生IIC停止信号
void MPU_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 MPU_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 MPU_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void MPU_IIC_Ack(void);					//IIC发送ACK信号
void MPU_IIC_NAck(void);				//IIC不发送ACK信号

void IMPU_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















