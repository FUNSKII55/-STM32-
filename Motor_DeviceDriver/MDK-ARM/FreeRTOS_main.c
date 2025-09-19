#include "FreeRTOS_main.h"
#include "oled.h"
#include "motor_control.h"
#include "mpu6050.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "qmc5883l.h"
//FREERTOS RELATED INCLUDE 
#include "FreeRTOS.h"
#include "task.h"

/*
* @description:句柄放置处
*
*
*/

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

/*
* @description:宏定义放置处
*
*
*/

extern UART_HandleTypeDef huart1;
uint8_t rx1_data;
void StartUart1Receive(void) { HAL_UART_Receive_IT(&huart1, &rx1_data, 1); }

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        if(rx1_data == '1') QMC5883L_StartCalibration();
        HAL_UART_Receive_IT(&huart1, &rx1_data, 1);
    }
		
		if(rx1_data == '2') {
    char msg[128];
    sprintf(msg, "From Flash:\r\nXmin:%d Xmax:%d\r\nYmin:%d Ymax:%d\r\nZmin:%d Zmax:%d\r\n",
           g_qmc5883l_calib.Xmin, g_qmc5883l_calib.Xmax,
           g_qmc5883l_calib.Ymin, g_qmc5883l_calib.Ymax,
           g_qmc5883l_calib.Zmin, g_qmc5883l_calib.Zmax);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 200);
}

}

//START TASK'S CONFIG
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;
void start_task(void * pvParameters);


//TASK 1 'S CONFIG
#define TASK1_STACK 128
#define TASK1_PRIORITY 1
TaskHandle_t task1_handle;
void task1(void * pvParameters);


//TASK 2 'S CONFIG
#define TASK2_STACK 128
#define TASK2_PRIORITY 1
TaskHandle_t task2_handle;
void task2(void * pvParameters);

//TASK 3 'S CONFIG
#define TASK3_STACK 128
#define TASK3_PRIORITY 1
TaskHandle_t task3_handle;
void task3(void * pvParameters);

/*
* @description:结构体放置处
*
*
*/

	//ROLL
	PID_t roll_pid = {
			.Kp = 0.8f,
			.Ki = 0.01f,
			.Kd = 0.03f,
			.setpoint = 0.0f,
			.integral = 0.0f,
			.last_error = 0.0f,
			.output = 0.0f,
			.integral_max = 500.0f,
			.output_max = 1000.0f
	};

	//YAW
	PID_t yaw_pid = {
			.Kp = 0.8f,
			.Ki = 0.01f,
			.Kd = 0.03f,
			.setpoint = 0.0f,
			.integral = 0.0f,
			.last_error = 0.0f,
			.output = 0.0f,
			.integral_max = 500.0f,
			.output_max = 1000.0f
	};

	//PITCH
	PID_t pitch_pid = {
			.Kp = 0.8f,
			.Ki = 0.01f,
			.Kd = 0.03f,
			.setpoint = 0.0f,
			.integral = 0.0f,
			.last_error = 0.0f,
			.output = 0.0f,
			.integral_max = 500.0f,
			.output_max = 1000.0f
};
	//高度环
	PID_t alt_pid = {
			.Kp = 2.0f,
			.Ki = 0.8f, 
			.Kd = 0.4f,
			.setpoint = 0.0f,
			.integral = 0.0f,
			.last_error = 0.0f,
			.output = 0.0f, 
			.integral_max = 500.0f, 
			.output_max = 600.0f
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

extern double Real_roll , Real_pitch , Real_yaw ;

static float Yaw_fusion = 0;    // 融合后的Yaw角（单位°）
static float Yaw_gyro = 0;      // 陀螺仪积分Yaw（单位°）
static float dt = 0.01f;        // 控制周期，和你的vTaskDelay一致，这里10ms
#define ALPHA 0.98f             // 互补滤波权重

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
    QMC5883L_Initialize(MODE_CONTROL_CONTINUOUS, OUTPUT_DATA_RATE_50HZ, FULL_SCALE_2G, OVER_SAMPLE_RATIO_128);

    while(1)
    {
        MPU6050 mpu_data;
        int16_t mag_x, mag_y, mag_z;
//        float Yaw_mag_rad, Yaw_mag_deg;

        // 1. 获取陀螺仪姿态角、角速度
        MPU6050_Get_Angle_Plus(&mpu_data);

//        // 2. 获取磁力计数据
//        QMC5883L_Read_Data(&mag_x, &mag_y, &mag_z);

//        // 3. 计算磁力计航向角（地磁北，弧度/度）
//        Yaw_mag_rad = atan2((float)mag_y, (float)mag_x); // 弧度，北为0
//        if(Yaw_mag_rad < 0) Yaw_mag_rad += 2 * M_PI;
//        Yaw_mag_deg = Yaw_mag_rad * 180.0f / M_PI;

        // 4. 陀螺仪积分得到Yaw（这里用角速度积分，如果你MPU6050已经输出Yaw可以省略）
        // Yaw_gyro += mpu_data.gyro_z * dt; // gyro_z单位为°/s
        // 这里如果mpu_data.yaw是积分后的Yaw，直接用
        Yaw_gyro = mpu_data.yaw * 57.3f; // 这里单位转成°，保持一致

        // 5. 互补滤波融合Yaw角
//        Yaw_fusion = ALPHA * (Yaw_gyro) + (1-ALPHA) * Yaw_mag_deg;

        // 6. 输出结果
        Real_yaw   = Yaw_fusion;
        Real_pitch = -mpu_data.roll * 57.3f;
        Real_roll  =  mpu_data.pitch * 57.3f;

        char uart_msg[100];
        sprintf(uart_msg, "%.3f,%.3f,%.3f\n", Real_yaw, Real_pitch, Real_roll);
        HAL_UART_Transmit(&huart1, (uint8_t*)uart_msg, strlen(uart_msg), 10);

        vTaskDelay(10); // 10ms
    }
}




void task2(void * pvParameters)
{

		Motor_Init(); // 启动电机PWM
    roll_pid.setpoint = 0; // 目标roll角为0°
	
	while(1)		
	{


        // 3. PID输出
        float dt = 0.01f; // 10ms
        float roll_output = PID_SingleLoop(&roll_pid, Real_roll, dt);
        float yaw_output = PID_SingleLoop(&yaw_pid, Real_yaw, dt);
				float pitch_output = PID_SingleLoop(&pitch_pid, Real_pitch, dt);
				float hight_output = PID_SingleLoop(&pitch_pid, Real_pitch, dt);

			
        // 4. PWM分配（左右对称布局）
			
				//angle loop
        int base_pwm = 1000; // 基础油门
        int pwm1 = base_pwm - roll_output - yaw_output - pitch_output;
        int pwm2 = base_pwm - roll_output - yaw_output + pitch_output;
        int pwm3 = base_pwm + roll_output + yaw_output - pitch_output;
        int pwm4 = base_pwm + roll_output + yaw_output + pitch_output;
        int pwm5 = base_pwm - roll_output + yaw_output - 0;
        int pwm6 = base_pwm + roll_output - yaw_output - 0;

////				//roll
////        int base_pwm = 00; // 基础油门
////        int pwm1 = base_pwm - roll_output;
////        int pwm2 = base_pwm - roll_output;
////        int pwm3 = base_pwm + roll_output;
////        int pwm4 = base_pwm + roll_output;
////        int pwm5 = base_pwm - roll_output;
////        int pwm6 = base_pwm + roll_output;


	Motor_Speed_Set(pwm1, pwm2, pwm3, pwm4, pwm5, pwm6);
// 
		vTaskDelay(10);
	}
	
}

void task3(void *pvParameters)
{
    StartUart1Receive();
    QMC5883L_LoadCalibFromFlash();

    while(1)
    {
//        int16_t mx, my, mz;
//        QMC5883L_Read_Data(&mx, &my, &mz);

        // 自动采集最大最小
//        if(mag_calibrating)
//        {
//            if(mx < g_qmc5883l_calib.Xmin) g_qmc5883l_calib.Xmin = mx;
//            if(mx > g_qmc5883l_calib.Xmax) g_qmc5883l_calib.Xmax = mx;
//            if(my < g_qmc5883l_calib.Ymin) g_qmc5883l_calib.Ymin = my;
//            if(my > g_qmc5883l_calib.Ymax) g_qmc5883l_calib.Ymax = my;
//            if(mz < g_qmc5883l_calib.Zmin) g_qmc5883l_calib.Zmin = mz;
//            if(mz > g_qmc5883l_calib.Zmax) g_qmc5883l_calib.Zmax = mz;

//            if(HAL_GetTick() - mag_calib_tick > MAG_CALIB_DURATION_MS)
//            {
//                mag_calibrating = false;
//                g_qmc5883l_calib.isValid = 0xA5;
//                QMC5883L_SaveCalibToFlash();

//                char msg[128];
//                sprintf(msg, "QMC5883L Calibration Finish!\r\nXmin:%d Xmax:%d\r\nYmin:%d Ymax:%d\r\nZmin:%d Zmax:%d\r\n",
//                        g_qmc5883l_calib.Xmin, g_qmc5883l_calib.Xmax,
//                        g_qmc5883l_calib.Ymin, g_qmc5883l_calib.Ymax,
//                        g_qmc5883l_calib.Zmin, g_qmc5883l_calib.Zmax);
//                HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 200);
//            }
//        }
        vTaskDelay(10); // 100Hz
    }
}
