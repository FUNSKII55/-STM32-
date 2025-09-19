#include "E49.h"

#define M0_Pin  GPIO_PIN_6
#define M0_Port GPIOA
#define M1_Pin  GPIO_PIN_7
#define M1_Port GPIOA

extern UART_HandleTypeDef huart1;

void E49_Mode_Set(uint8_t Mode)
{
	if(Mode==0)//0 0 传输模式
	{
		HAL_GPIO_WritePin(M0_Port,M0_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(M1_Port,M1_Pin,GPIO_PIN_RESET);
	}	
	if(Mode==1)//1 0 RSSI模式
	{
		HAL_GPIO_WritePin(M0_Port,M0_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(M1_Port,M1_Pin,GPIO_PIN_RESET);
	}
	if(Mode==2)//0 1 设置模式 M0 0 M1 1
	{
		HAL_GPIO_WritePin(M0_Port,M0_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(M1_Port,M1_Pin,GPIO_PIN_SET);
	}
	if(Mode==3)//1 1 休眠模式
	{
		HAL_GPIO_WritePin(M0_Port,M0_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(M1_Port,M1_Pin,GPIO_PIN_SET);
	}
}
void E49_Init()
{
	//进入设置模式
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);//M0
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);//M1
	vTaskDelay(50);
	uint8_t SendData[6]={0xC0,0x00,0x00,0x19,0x2E,0x00};
	HAL_UART_Transmit(&huart1,SendData,6,100);
	vTaskDelay(200);
	//进入传输模式
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);//M0
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);//M1
	vTaskDelay(50);
}
