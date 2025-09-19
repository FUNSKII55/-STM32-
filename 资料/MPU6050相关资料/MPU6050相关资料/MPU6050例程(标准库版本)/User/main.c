#include "stm32f10x.h"       
#include "generic.h"										//��Ҫ�洢Delay�ȳ��õ�
#include "OLED_I2C.h"
#include "MPU6050.h"
#include "Serial.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "Kalman_filter.h"
#include "Timer.h"

MPU6050 MM;	//����һ���ṹ����������ŷ����

void TIM3_IRQHandler(void){
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//clear flag
		//MPU6050_Get_Angle_Plus(&MM);//��ȡ�Ƕ�
		MPU6050_Get_Angle(&MM);
	}
}

int main(void)
{
	Serial_Init(USART1,115200);//���ڳ�ʼ��
	OLED_Init(GPIOB,GPIO_Pin_10,GPIO_Pin_11);//��ʼOLED��Ļ
	MPU6050_init(GPIOA,GPIO_Pin_0,GPIO_Pin_1);//��ʼ��MPU6050(SCL:A0,SDA:A1)
	TImer_IT_init_ms(TIM3,10);
	
	//mpu_dmp_init(GPIOA,GPIO_Pin_1,GPIO_Pin_0);//��ʼ��dmp��,ȡ��ע��2
	while (1)
	{
		OLED_Clear();
		//mpu_dmp_get_data(&MM.pitch,&MM.roll,&MM.yaw);ע��2
		
		printf("%.3f,%.3f,%.3f\n",MM.roll,MM.pitch,MM.yaw);//���ڷ�������
		OLED_Printf(0,0,6,"roll:%.2f",MM.roll);
		OLED_Printf(0,8,6,"pitch:%.2f",MM.pitch);
		OLED_Printf(0,16,6,"yaw:%.2f",MM.yaw);
		OLED_Update();
	}
}
