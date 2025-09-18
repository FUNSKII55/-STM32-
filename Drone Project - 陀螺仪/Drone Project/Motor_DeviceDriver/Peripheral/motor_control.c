//#include "motor_control.h"
//#include "main.h"
//#include "mpu6050.h"
//#include "oled.h"
//#include "usart.h"
//#include "FreeRTOS.h"
//#include "task.h"

//extern TIM_HandleTypeDef htim3;
//extern TIM_HandleTypeDef htim4;

//extern double Real_yaw, Real_pitch, Real_roll;


//	// PID角度环结构体

//	//ROLL
//	PID_t roll_pid = {
//			.Kp = 0.8f,
//			.Ki = 0.01f,
//			.Kd = 0.03f,
//			.setpoint = 0.0f,
//			.integral = 0.0f,
//			.last_error = 0.0f,
//			.output = 0.0f,
//			.integral_max = 500.0f,
//			.output_max = 3000.0f
//	};

//	//YAW
//	PID_t yaw_pid = {
//			.Kp = 0.8f,
//			.Ki = 0.01f,
//			.Kd = 0.03f,
//			.setpoint = 0.0f,
//			.integral = 0.0f,
//			.last_error = 0.0f,
//			.output = 0.0f,
//			.integral_max = 500.0f,
//			.output_max = 3000.0f
//	};

//	//PITCH
//	PID_t pitch_pid = {
//			.Kp = 0.8f,
//			.Ki = 0.01f,
//			.Kd = 0.03f,
//			.setpoint = 0.0f,
//			.integral = 0.0f,
//			.last_error = 0.0f,
//			.output = 0.0f,
//			.integral_max = 500.0f,
//			.output_max = 3000.0f
//};

//// PID计算
//	float PID_SingleLoop(PID_t* pid, float measured, float dt) {
//    float error = pid->setpoint - measured;
//    pid->integral += error * dt;
//    if(pid->integral > pid->integral_max) pid->integral = pid->integral_max;
//    if(pid->integral < -pid->integral_max) pid->integral = -pid->integral_max;

//    float derivative = (error - pid->last_error) / dt;
//    pid->output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;

//    if(pid->output > pid->output_max) pid->output = pid->output_max;
//    if(pid->output < -pid->output_max) pid->output = -pid->output_max;

//    pid->last_error = error;
//    return pid->output;
//}

//// 电机初始化
//void Motor_Init(void) {
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
//    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
//    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 0);

//    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
//    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
//    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
//    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
//    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
//    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
//}

//// 电机PWM输出
//void Motor_Speed_Set(int pwm1, int pwm2, int pwm3, int pwm4, int pwm5, int pwm6) {
//    if(pwm1 > MOTOR_PWM_MAX) pwm1 = MOTOR_PWM_MAX;
//    if(pwm1 < MOTOR_PWM_MIN) pwm1 = MOTOR_PWM_MIN;
//    if(pwm2 > MOTOR_PWM_MAX) pwm2 = MOTOR_PWM_MAX;
//    if(pwm2 < MOTOR_PWM_MIN) pwm2 = MOTOR_PWM_MIN;
//    if(pwm3 > MOTOR_PWM_MAX) pwm3 = MOTOR_PWM_MAX;
//    if(pwm3 < MOTOR_PWM_MIN) pwm3 = MOTOR_PWM_MIN;
//    if(pwm4 > MOTOR_PWM_MAX) pwm4 = MOTOR_PWM_MAX;
//    if(pwm4 < MOTOR_PWM_MIN) pwm4 = MOTOR_PWM_MIN;
//    if(pwm5 > MOTOR_PWM_MAX) pwm5 = MOTOR_PWM_MAX;
//    if(pwm5 < MOTOR_PWM_MIN) pwm5 = MOTOR_PWM_MIN;
//    if(pwm6 > MOTOR_PWM_MAX) pwm6 = MOTOR_PWM_MAX;
//    if(pwm6 < MOTOR_PWM_MIN) pwm6 = MOTOR_PWM_MIN;

//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm1);
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwm2);
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwm3);
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwm4);
//    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm5);
//    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm6);
//}

//// ======= 主控循环 =======
//MPU6050 mpu_data;  // 姿态变量

//void Main_Loop(void) {
//    Motor_Init(); // 启动电机PWM
//    roll_pid.setpoint = 0; // 目标roll角为0°

//        // 1. 姿态采集
////        MPU6050_Get_Angle_Plus(&mpu_data);

//        // 2. 获取roll角（如果输出单位为弧度需*57.3，这里假设你的解算输出就是度）
////        float real_roll = mpu_data.pitch*57.3;
////        float real_yaw = mpu_data.yaw*57.3;
////				float real_pitch = mpu_data.roll*57.3;



//        // 3. PID输出
//        float dt = 0.01f; // 10ms
//        float roll_output = PID_SingleLoop(&roll_pid, Real_roll, dt);
//        float yaw_output = PID_SingleLoop(&yaw_pid, Real_yaw, dt);
//				float pitch_output = PID_SingleLoop(&pitch_pid, Real_pitch, dt);

//			
//        // 4. PWM分配（左右对称布局）
//			
//				//angle loop
////        int base_pwm = 500; // 基础油门
////        int pwm1 = base_pwm - roll_output - yaw_output - pitch_output;
////        int pwm2 = base_pwm - roll_output - yaw_output + pitch_output;
////        int pwm3 = base_pwm + roll_output + yaw_output - pitch_output;
////        int pwm4 = base_pwm + roll_output + yaw_output + pitch_output;
////        int pwm5 = base_pwm - roll_output + yaw_output - 0;
////        int pwm6 = base_pwm + roll_output - yaw_output - 0;

//				//roll
//        int base_pwm = 000; // 基础油门
//        int pwm1 = base_pwm - roll_output;
//        int pwm2 = base_pwm - roll_output;
//        int pwm3 = base_pwm + roll_output;
//        int pwm4 = base_pwm + roll_output;
//        int pwm5 = base_pwm - roll_output;
//        int pwm6 = base_pwm + roll_output;

////				//yaw偏航
////			  int base_pwm = 000; // 基础油门
////        int pwm1 = base_pwm - yaw_output;
////        int pwm2 = base_pwm - yaw_output;
////        int pwm3 = base_pwm + yaw_output;
////        int pwm4 = base_pwm + yaw_output;
////        int pwm5 = base_pwm + yaw_output;
////        int pwm6 = base_pwm - yaw_output;
//				
////				//pitch
////				int base_pwm = 000; // 基础油门
////        int pwm1 = base_pwm - pitch_output;
////        int pwm2 = base_pwm + pitch_output;
////        int pwm3 = base_pwm - pitch_output;
////        int pwm4 = base_pwm + pitch_output;
////        int pwm5 = base_pwm + 0;
////        int pwm6 = base_pwm + 0;

//        Motor_Speed_Set(pwm1, pwm2, pwm3, pwm4, pwm5, pwm6);
//			
////			char buf[128];
////			sprintf(buf, "setpoint=%.2f, real=%.2f, err=%.2f, out=%.2f\r\n",
////      roll_pid.setpoint, real_roll, roll_pid.setpoint - real_roll, roll_output);
////			HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 100);

//			
//			OLED_ShowNum(0,3,pwm1,6,8,0);
//			OLED_ShowNum(0,4,pwm2,6,8,0);
//			OLED_ShowNum(0,5,pwm3,6,8,0);
//			OLED_ShowNum(0,6,pwm4,6,8,0);
//			OLED_ShowNum(0,7,pwm5,6,8,0);
//			OLED_ShowNum(0,8,pwm6,6,8,0);

//				

//			
//        // 串口输出
////				
////			  double Real_yaw   =  mpu_data.yaw;
////				double Real_pitch =  -mpu_data.roll;
////				double Real_row   =  mpu_data.pitch;
////				
////        char uart_msg[100];
////        sprintf(uart_msg, "%.3f,%.3f,%.3f\n", 
////                Real_yaw, Real_pitch, Real_row);
////        HAL_UART_Transmit(&huart1, (uint8_t*)uart_msg, strlen(uart_msg), 10);
////			
////			OLED_Clear();
////			OLED_ShowNum(1,1,roll_output,5,8,0);
//			
////			OLED_ShowString(0,1,"YAW",8,0);
////			OLED_ShowString(0,1,"ROLL",8,0);
////			OLED_ShowString(0,1,"PITCH",8,0);
////			
////			
////			
////			char buf0[16];
////			sprintf(buf0, "%.3f", mpu_data.yaw);  // 保留两位小数
////			OLED_ShowString(50, 1, buf0, 8, 0);   // 显示字符串
////			char buf1[16];
////			sprintf(buf1, "%.3f", mpu_data.roll);  // 保留两位小数
////			OLED_ShowString(50, 2, buf1, 8, 0);   // 显示字符串
////			char buf2[16];
////			sprintf(buf2, "%.3f", mpu_data.pitch);  // 保留两位小数
////			OLED_ShowString(50, 3, buf2, 8, 0);   // 显示字符串

//        vTaskDelay(5); // 10ms周期
//    
//}
