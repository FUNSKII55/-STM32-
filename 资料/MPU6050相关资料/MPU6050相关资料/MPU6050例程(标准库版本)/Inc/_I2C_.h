#ifndef EXTEND_HEADFIEL__I2C__H_
#define EXTEND_HEADFIEL__I2C__H_
#include "stm32f10x.h"
#include "generic.h"//��ҪװDelay.h

/*
bilibili:СŬ�� UID:437280309
@time:2025.4.12
@verson:V1_4
@updata: Fixed an issue when multiple I2C cannot be used.Using object oriented programming thinking,Hide a lot lof API function,softerware I2C have arrived 390K
�޸��˶��I2C�޷�ʹ�õ����⡣ʹ���������������˼ά.���ش���API�����ӿڡ����I2C����ٶ��Ѿ��ﵽ390Khz,��Ӳ��IIC����ģʽ400k����

ע��:���ú�����ʱ����Ϊʲô����û�в��Σ���Ϊ���ǿ�©ģʽ������Ҫ�������裬��Ȼ���ŵ�ƽ�޷������仯�����ҵ��Ǻܶ�ģ����Դ���������,4.7K�����ٶȱ�10K������������IIC*/
/*Ӳ��I2C��ʼ����Ӧ����for STM32F103C8
SCL1��PB6		SCL2��PB10
SDA1��PB7		SDA2��PB11
*/


/*//////////////////////////////////////////////////
ʹ��ʾ��:(���ڲ���8λ�Ĵ���)��ʽI2C(��Ķ�������)->��Ϊ(����)
I2C_BUS myI2C;
void main(){
	myI2C = Create_SI2C(GPIOA,GPIO_Pin_1,GPIO_Pin_2,0x40);//����һ�����ֽ�myI2C�Ķ���,7λ��ַ��0x40
	uint8_t add = 0;
	//I2C(INA266_I2C)->Mode16bit = 1;//����Ϊ16λ�Ĵ�����������(�����Ҫ��16λ�Ĵ�������)
	while(1){
		add = I2C(myI2C)->ScanAdress(); //ɨ��IIC��ַ
		print("add:%d",add);			//��ӡɨ�赽��IIC��ַ(����ֵ��10����)
	}
}
*///////////////////////////////////////////////////
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

	#define SI2C_delay_time 0																			//����Ӳ��I2C����ʱ�ٶ�
	#define I2C(obj) (Pthis_I2C = &obj)																	//Pthisȫ��ָ��궨��
	typedef struct I2C_Private I2C_Private;		
	typedef struct I2C_BUS
	{
		//�Ͻ�ʹ�ø�ָ��!(˽�б���) It is strictly forbidden to use this pointer
		I2C_Private* Private;

		///////////////////////���ú���//////////////////////////////////                   			 				//�û�API�����ӿ�
		void (*Write_Reg)(uint8_t RegAddress, uint16_t Data);																						//д�Ĵ�������(Ĭ��8bit����),write register by I2C bus
		uint16_t (*Read_Reg)(uint8_t RegAddress);																												//���Ĵ�������(Ĭ��8bit����),read register by I2C bus

		///////////////////////��չ����//////////////////////////////////  
		uint8_t (*ScanAdress)();																																				//ɨ��IIC��ַ������,Scan IIC address and return it
		uint8_t (*AckTest)();																																						//��Ӧ�ӿ�(1 success,0 failed),it can check that if our I2C bus is init succese
		uint8_t Mode16bit;																																							//��IIC����Ϊ16λ����(��1Ϊ����),boost reg operation to 16bit
		void (*Rest_Speed)(uint32_t Speed);																															//Ӳ��I2C���������ٶ�,you can reset your Hardware I2C Speed	
		uint8_t (*Write_Reg_continue)(uint8_t Device_Add,uint8_t RegAddress,uint8_t Count,uint8_t* Data);//����д�Ĵ�������,continue write register by I2C bus��Ϊ����Ӧ��ֲ
		uint8_t (*Read_Reg_continue)(uint8_t Device_Add,uint8_t RegAddress,uint8_t Count,uint8_t* Data); //�������Ĵ�������,continue read register by I2C bus��Ϊ����Ӧ��ֲ
	}I2C_BUS;
	extern I2C_BUS* Pthis_I2C;
		///////////////////////��ʼ������//////////////////////////////// 
	I2C_BUS Create_SI2C(GPIO_TypeDef* GPIOx,uint16_t SCL,uint16_t SDA,uint8_t Address);					//�������I2C����,create a softwere I2C
	I2C_BUS Create_HI2C(I2C_TypeDef* I2Cx,uint8_t Address);												//����Ӳ��I2C����,create a hardware I2C

#ifdef __cplusplus
extern "C"
}
#endif /* __cplusplus */

#endif /* EXTEND_HEADFIEL__I2C__H_ */
