/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"
#include "motor_control.h"
#include "mpu6050.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "FreeRTOS_main.h"
#include "u8g2.h"
#include "UI.h"
#include "UI_PID.h"
#include "menu.h"

u8g2_t u8g2; 
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t u8g2_stm32_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	u8g2_Setup_ssd1306_i2c_128x32_univision_f(&u8g2 ,U8G2_R0, u8x8_byte_hw_i2c,u8g2_stm32_delay);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_I2C2_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim1);
	
//	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
//	u8g2_SetPowerSave(&u8g2, 0); // wake up display
//	u8g2_ClearBuffer(&u8g2);
//	MPU6050_init(&hi2c2);
//	MPU6050 mpu_data;  // 姿态变量

//u8g2_InitDisplay(&u8g2);                  // 初始化控制器
//u8g2_SetPowerSave(&u8g2, 0);              // 关省电，打开面板

//u8g2_ClearBuffer(&u8g2);
//u8g2_SetFont(&u8g2, u8g2_font_6x12_tf);
//u8g2_DrawStr(&u8g2, 0, 12, "HELLO");
//u8g2_SendBuffer(&u8g2);   
FreeRTOS_Start();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
	{
//  {  MPU6050_Get_Angle_Plus(&mpu_data);

//			  double Real_yaw   =  mpu_data.yaw;
//				double Real_pitch =  -mpu_data.roll;
//				double Real_row   =  mpu_data.pitch;
//				
//        char uart_msg[100];
//        sprintf(uart_msg, "%.3f,%.3f,%.3f\n", 
//        Real_yaw, Real_pitch, Real_row);
//        HAL_UART_Transmit(&huart1, (uint8_t*)uart_msg, strlen(uart_msg), 10);
		
//	Main_Loop();


		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
uint8_t u8g2_stm32_delay(U8X8_UNUSED u8x8_t *u8x8,
                         U8X8_UNUSED uint8_t msg,
                         U8X8_UNUSED uint8_t arg_int,
                         U8X8_UNUSED void *arg_ptr)
{
    switch(msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        /* 如果有需要初始化的 GPIO，这里写；没有可以空着 */
        break;

    case U8X8_MSG_DELAY_MILLI:
        if (arg_int == 0) {
            taskYIELD();   // 让出 CPU
        } else {
            vTaskDelay(pdMS_TO_TICKS(arg_int)); // RTOS 延时，单位 ms
        }
        break;

    case U8X8_MSG_DELAY_10MICRO:
        /* 简单忙等，不依赖硬件 */
        for (volatile uint32_t n = 0; n < arg_int * 20; n++) {
            __NOP();
        }
        break;

    case U8X8_MSG_DELAY_100NANO:
        __NOP(); // 太短了，用 NOP 占位
        break;

    default:
        return 0;  // 未处理的消息
    }
    return 1;      // 成功处理
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
