#ifndef INTERRUPTPROCESS_H
#define INTERRUPTPROCESS_H
#include "E49.h"

// 声明全局变量
extern volatile uint8_t Motor_Ready;
extern volatile uint8_t Motor_Run;
extern volatile uint8_t Fly_Start;
extern UART_HandleTypeDef huart1;
extern volatile uint8_t USART1_Rx_Byte;  // 添加 volatile 确保中断中修改

// UART 接收回调函数声明
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif  // INTERRUPTPROCESS_H
