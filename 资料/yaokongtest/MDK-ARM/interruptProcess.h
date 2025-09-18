#ifndef INTERRUPTPROCESS_H
#define INTERRUPTPROCESS_H
#include "E49.h"

// ����ȫ�ֱ���
extern volatile uint8_t Motor_Ready;
extern volatile uint8_t Motor_Run;
extern volatile uint8_t Fly_Start;
extern UART_HandleTypeDef huart1;
extern volatile uint8_t USART1_Rx_Byte;  // ��� volatile ȷ���ж����޸�

// UART ���ջص���������
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif  // INTERRUPTPROCESS_H
