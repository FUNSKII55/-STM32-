#include "mpuiic.h"
#include "generic.h"

#define MPUiicDelay 0   //��ʱʱ��

void MPU_IIC_Start(void)
{
	MPU_SDA_OUT();     //sda�����
	MPU_IIC_SDA=1;	  	  
	MPU_IIC_SCL=1;
	Delay_us(MPUiicDelay);
 	MPU_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}

void MPU_IIC_Stop(void)
{
	MPU_SDA_OUT();//sda�����
	MPU_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=1; 
	MPU_IIC_SDA=1;//����I2C���߽����ź�
	Delay_us(MPUiicDelay);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t MPU_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	MPU_SDA_IN();      //SDA����Ϊ����
	MPU_IIC_SDA=1;
	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=1;
	Delay_us(MPUiicDelay);
	while(MPU_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250){
			MPU_IIC_Stop();
			return 1;
		}
	}
	MPU_IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 

void MPU_WriteAck(uint8_t AckBit){
	MPU_SDA_OUT();
	MPU_IIC_SDA=AckBit;
	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=1;
	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void MPU_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	MPU_SDA_OUT(); 	    
    //MPU_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        MPU_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		    MPU_IIC_SCL=1;
		    Delay_us(MPUiicDelay); 
		    MPU_IIC_SCL=0;	
		    Delay_us(MPUiicDelay);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t MPU_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MPU_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        Delay_us(MPUiicDelay);
		MPU_IIC_SCL=1;
        receive<<=1;
        if(MPU_READ_SDA)receive++;   
		Delay_us(MPUiicDelay); 
		MPU_IIC_SCL=0; 
    }					 
    if (!ack)
        MPU_WriteAck(1);//����nACK
    else
        MPU_WriteAck(0); //����ACK   
    return receive;
}

//IIC����д
//addr:������ַ
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i; 
    MPU_IIC_Start(); 
	MPU_IIC_Send_Byte(addr<<1);//����������ַ+д����	
	if(MPU_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		printf("MPU_Write_Len�е�ַ���ִ���\n");
		MPU_IIC_Stop();		 
		return 1;
	}
    MPU_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();		//�ȴ�Ӧ��
	for(i=0;i<len;i++)
	{
		MPU_IIC_Send_Byte(buf[i]);	//��������
		if(MPU_IIC_Wait_Ack())		//�ȴ�ACK
		{
			printf("MPU_Write_Len�����ݷ���ʧ��\n");
			MPU_IIC_Stop();	 
			return 1;		 
		}
	}    
    MPU_IIC_Stop();
	return 0;	
} 
//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
 	MPU_IIC_Start(); 
	MPU_IIC_Send_Byte(addr<<1);//����������ַ+д����	
	if(MPU_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		MPU_IIC_Stop();
		printf("MPU_Read_Len�е�ַ���ִ���\n");
		return 1;
	}
    MPU_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();		//�ȴ�Ӧ��
    MPU_IIC_Start();
	MPU_IIC_Send_Byte((addr<<1)|1);//����������ַ+������	
    MPU_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	while(len)
	{
		if(len==1)*buf=MPU_IIC_Read_Byte(0);//������,����nACK 
		else *buf=MPU_IIC_Read_Byte(1);		//������,����ACK  
		len--;
		buf++; 
	}    
    MPU_IIC_Stop();	//����һ��ֹͣ���� 
	return 0;	
}
