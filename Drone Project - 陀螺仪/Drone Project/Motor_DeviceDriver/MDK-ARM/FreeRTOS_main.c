#include "FreeRTOS_main.h"
//#include "oled.h"
#include "motor_control.h"
#include "mpu6050.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"
#include "key.h"
#include "UI.h"
#include "UI_PID.h"
#include "menu.h"
#include "filter.h"
//FREERTOS RELATED INCLUDE 
#include "FreeRTOS.h"
#include "task.h"
#include "u8g2.h"

/*
* @description:句柄放置处
*
*
*/

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern u8g2_t u8g2;           // 由 main.c 顶部全局定义：u8g2_t u8g2;


/*
* @description:宏定义放置处
*
*
*/
//START TASK'S CONFIG
#define START_TASK_STACK 256
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;
void start_task(void * pvParameters);


//TASK 1 'S CONFIG
#define TASK1_STACK 256
#define TASK1_PRIORITY 1
TaskHandle_t task1_handle;
void task1(void * pvParameters);


//TASK 2 'S CONFIG
#define TASK2_STACK 256
#define TASK2_PRIORITY 1
TaskHandle_t task2_handle;
void task2(void * pvParameters);

//TASK 3 'S CONFIG
#define TASK3_STACK 512
#define TASK3_PRIORITY 1
TaskHandle_t task3_handle;
void task3(void * pvParameters);

//TASK 4 'S CONFIG
#define TASK4_STACK 128
#define TASK4_PRIORITY 1
TaskHandle_t task4_handle;
void task4(void * pvParameters);

/*
* @description:结构体放置处
*
*
*/

	uint32_t key_ts = 0;	


	//ROLL  横滚
	PID_t roll_pid = {
			.Kp = 1.8f*60.0,
			.Ki = 0.01*60.0f,
			.Kd = 0.03*60.0f,
			.setpoint = 0.0f,
			.integral = 0.0f,
			.last_error = 0.0f,
			.output = 0.0f,
			.integral_max = 500.0f,
			.output_max = 4000.0f
	};

	//YAW
	PID_t yaw_pid = {
			.Kp = 1.8f*60.0,
			.Ki = 0.01*60.0f,
			.Kd = 0.03*60.0f,
			.setpoint = 0.0f,
			.integral = 0.0f,
			.last_error = 0.0f,
			.output = 0.0f,
			.integral_max = 500.0f,
			.output_max = 4000.0f
	};

	//PITCH  俯仰
	PID_t pitch_pid = {
			.Kp = 1.8*60.0f,
			.Ki = 0.01*60.0f,
			.Kd = 0.03*60.0f,
			.setpoint = 0.0f,
			.integral = 0.0f,
			.last_error = 0.0f,
			.output = 0.0f,
			.integral_max = 500.0f,
			.output_max = 4000.0f
};
	
// PID计算
	float PID_SingleLoop(PID_t* pid, float measured, float dt) {
    float error = pid->setpoint - measured;
    pid->integral += error * dt;
    if(pid->integral > pid->integral_max) pid->integral = pid->integral_max;
    if(pid->integral < -pid->integral_max) pid->integral = -pid->integral_max;

    float derivative = (error - pid->last_error) / dt;
    pid->output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;

    if(pid->output > pid->output_max) pid->output = pid->output_max;
    if(pid->output < -pid->output_max) pid->output = -pid->output_max;

    pid->last_error = error;
    return pid->output;
}

// 电机初始化
void Motor_Init(void) {
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 0);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
}

// 电机PWM输出
void Motor_Speed_Set(int pwm1, int pwm2, int pwm3, int pwm4, int pwm5, int pwm6) {
    if(pwm1 > MOTOR_PWM_MAX) pwm1 = MOTOR_PWM_MAX;
    if(pwm1 < MOTOR_PWM_MIN) pwm1 = MOTOR_PWM_MIN;
    if(pwm2 > MOTOR_PWM_MAX) pwm2 = MOTOR_PWM_MAX;
    if(pwm2 < MOTOR_PWM_MIN) pwm2 = MOTOR_PWM_MIN;
    if(pwm3 > MOTOR_PWM_MAX) pwm3 = MOTOR_PWM_MAX;
    if(pwm3 < MOTOR_PWM_MIN) pwm3 = MOTOR_PWM_MIN;
    if(pwm4 > MOTOR_PWM_MAX) pwm4 = MOTOR_PWM_MAX;
    if(pwm4 < MOTOR_PWM_MIN) pwm4 = MOTOR_PWM_MIN;
    if(pwm5 > MOTOR_PWM_MAX) pwm5 = MOTOR_PWM_MAX;
    if(pwm5 < MOTOR_PWM_MIN) pwm5 = MOTOR_PWM_MIN;
    if(pwm6 > MOTOR_PWM_MAX) pwm6 = MOTOR_PWM_MAX;
    if(pwm6 < MOTOR_PWM_MIN) pwm6 = MOTOR_PWM_MIN;

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm1);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwm2);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwm3);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwm4);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm5);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm6);
}

/*
* @description:变量放置处
*
*
*/
volatile float roll_output = 0.0f;
volatile float yaw_output  = 0.0f;
volatile float pitch_output= 0.0f;

volatile float Real_roll = 0.0f, Real_pitch = 0.0f, Real_yaw = 0.0f;
int base_pwm = 3000; 
volatile unsigned int Departure_Flag = 0; 


	/*
* @description:创建任务函数
*
*
*/


void FreeRTOS_Start(void)
{
//1. creat the lunching task
	xTaskCreate( (TaskFunction_t) start_task,
                            (char * )"caonima",
                            (configSTACK_DEPTH_TYPE)START_TASK_STACK,
                            NULL,
                            (UBaseType_t)START_TASK_PRIORITY,
                            &start_task_handle );
														
														
//2.start the scheduler(creat freeTsak )
	vTaskStartScheduler();
}


void start_task(void * pvParameters)
{

		xTaskCreate( (TaskFunction_t) task1,
                            (char * )"GET_MPU6050_DATA_AND_TREANSMIT_2_UARST",
                            (configSTACK_DEPTH_TYPE)TASK1_STACK,
                            NULL,
                            (UBaseType_t)TASK1_PRIORITY,
                            &task1_handle );
														
		xTaskCreate( (TaskFunction_t) task2,
                            (char * )"MOTOR_DRIVER_AND_PID",
                            (configSTACK_DEPTH_TYPE)TASK2_STACK,
                            NULL,
                            (UBaseType_t)TASK2_PRIORITY,
                            &task2_handle );
														
		xTaskCreate( (TaskFunction_t) task3,
                            (char * )"task3",
                            (configSTACK_DEPTH_TYPE)TASK3_STACK,
                            NULL,
                            (UBaseType_t)TASK3_PRIORITY,
                            &task3_handle );
														
		xTaskCreate( (TaskFunction_t) task4,
                            (char * )"task4",
                            (configSTACK_DEPTH_TYPE)TASK4_STACK,
                            NULL,
                            (UBaseType_t)TASK4_PRIORITY,
                            &task4_handle );

		vTaskDelete(NULL);
														
}

/*
* @description:任务函数
*
*
*/
void task1(void * pvParameters)
{
	
	MPU6050_init(&hi2c2);
	while(1)	
	{
	
		MPU6050 mpu_data;  // 姿态变量
		MPU6050_Get_Angle_Plus(&mpu_data);

    Real_yaw   =  mpu_data.yaw;
    Real_pitch = -mpu_data.roll;																				
    Real_roll   =  mpu_data.pitch;
		
		char uart_msg[100];
    sprintf(uart_msg, "%.3f,%.3f,%.3f,\n", 
    Real_yaw, Real_pitch,Real_roll);
	  HAL_UART_Transmit(&huart1, (uint8_t*)uart_msg, strlen(uart_msg), 10);
		
		
	
		vTaskDelay(5);
	}
}

void task2(void * pvParameters)	
{

		Motor_Init(); // 启动电机PWM
    roll_pid.setpoint = 0; // 目标roll角为0°
	
	while(1)		
	{
		
				if(Departure_Flag == 0)
				{
					vTaskDelay(7000);
					base_pwm = 2500;
					Departure_Flag = 1;
		//			return;
				}

        // 3. PID输出
        float dt = 0.01f; // 10ms
        float roll_output = PID_SingleLoop(&roll_pid, Real_roll, dt);
        float yaw_output = PID_SingleLoop(&yaw_pid, Real_yaw, dt);
				float pitch_output = PID_SingleLoop(&pitch_pid, Real_pitch, dt);

			
        // 4. PWM分配（左右对称布局）
			
				//angle loop
		if(Departure_Flag == 1)
			{
					base_pwm = 1000; // 基础油门
			}
			
			else if(Departure_Flag == 0)
			{
				
				base_pwm = 100;
				
			}
				int pwm1 = base_pwm - roll_output - yaw_output + pitch_output;
        int pwm2 = base_pwm - roll_output - yaw_output - pitch_output;
        int pwm3 = base_pwm + roll_output + yaw_output + pitch_output;
        int pwm4 = base_pwm + roll_output + yaw_output - pitch_output;
        int pwm5 = base_pwm - roll_output + yaw_output - pitch_output;
        int pwm6 = base_pwm + roll_output - yaw_output - pitch_output;
			
//				char uart_msg[100];
//				sprintf(uart_msg, "%d,%d,%d,%d,%d,%d\n", 
//				pwm1,pwm2,pwm3,pwm4,pwm5,pwm6);
//				HAL_UART_Transmit(&huart1, (uint8_t*)uart_msg, strlen(uart_msg), 10);

//				int pwm1 = 200;
//        int pwm2 = 200;
//				int pwm3 = 200;
//        int pwm4 = 200;
//			  int pwm5 = 200;
//        int pwm6 = 200;

			
//				//roll
			
//        int pwm1 = base_pwm - roll_output;
//        int pwm2 = base_pwm - roll_output;
//        int pwm3 = base_pwm + roll_output;
//        int pwm4 = base_pwm + roll_output;
//        int pwm5 = base_pwm - roll_output;
//        int pwm6 = base_pwm + roll_output;


	Motor_Speed_Set(pwm1, pwm2, pwm3, pwm4, pwm5, pwm6);
 
		vTaskDelay(10);
	}
	
}

void task3(void * pvParameters)
{
	
	// Setup 之后立刻初始化并点亮
//	u8g2_InitDisplay(&u8g2);                  // 初始化控制器
//	u8g2_SetPowerSave(&u8g2, 0);              // 关省电，打开面板
    	while(1)	
	{

//		u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
		
		int counter = __HAL_TIM_GET_COUNTER(&htim1);
//		
//		char buf[24];		char buff[24];		char bufff[24]; char buffff[24];

//		snprintf(buf,   sizeof(buf),   "Real_yaw: %.2f",   Real_yaw);
//		snprintf(buff,  sizeof(buff),  "Real_pitch: %.2f", Real_pitch);
//		snprintf(bufff, sizeof(bufff), "Real_roll: %.2f",  Real_roll);
//		snprintf(buffff, sizeof(buffff), "counter: %d",  counter);

//		u8g2_DrawStr(&u8g2, 0, 10, buf);
//		u8g2_DrawStr(&u8g2, 0, 20, buff);
//		u8g2_DrawStr(&u8g2, 0, 30, bufff);
//		u8g2_DrawStr(&u8g2, 0, 10, buffff);

//		u8g2_DrawStr(&u8g2, 0, 12, "HELLO,SENIOR LiFANG");
//		u8g2_DrawStr(&u8g2, 0, 24, "THERE'S FUNSKII");
//		u8g2_DrawStr(&u8g2, 0, 24, "THERE IS FUNSKII'S DRONE");
//		u8g2_ClearBuffer(&u8g2);
//		u8g2_SendBuffer(&u8g2);          


    if (counter - key_ts >= 10) {
        key_ts += 10;
        key_scan();
        key_pd();          // 在 key_pd 里修改 menu_index
    }	
		
//		menu_table[menu_index].current_operation();
//		menu_clamp();
		
		vTaskDelay(200);
	}

}

void task4(void * pvParameters)
{

          
    	while(1)	
	{
		
		

		
		vTaskDelay(10);
	}

}
