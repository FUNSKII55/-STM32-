#include "interruptProcess.h"

volatile uint8_t Motor_Run,Fly_Start;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        // 处理接收到的字节
        switch (USART1_Rx_Byte)
        {
        case '#':
            Motor_Run = 1;
            Fly_Start = 1;
            break;  // 解锁
        case '$':
            Motor_Run = 0;
            break;  // 上锁
        default:
            break;
        }

        // 重新启动串口接收中断
        USART1_Rx_Byte = 0;
        HAL_UART_Receive_IT(&huart1, &USART1_Rx_Byte, 1);  // 开始接收下一字节数据
    }
    UNUSED(huart);
}
