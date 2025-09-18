#include "interruptProcess.h"

volatile uint8_t Motor_Run,Fly_Start;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        // ������յ����ֽ�
        switch (USART1_Rx_Byte)
        {
        case '#':
            Motor_Run = 1;
            Fly_Start = 1;
            break;  // ����
        case '$':
            Motor_Run = 0;
            break;  // ����
        default:
            break;
        }

        // �����������ڽ����ж�
        USART1_Rx_Byte = 0;
        HAL_UART_Receive_IT(&huart1, &USART1_Rx_Byte, 1);  // ��ʼ������һ�ֽ�����
    }
    UNUSED(huart);
}
