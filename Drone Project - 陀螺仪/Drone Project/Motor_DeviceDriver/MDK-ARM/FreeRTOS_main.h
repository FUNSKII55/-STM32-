#ifndef __FreeRTOS_H
#define __FreeRTOS_H

void FreeRTOS_Start(void);
// PID控制器结构体（适用于单轴的角度或速度环）
// 用于存储和管理PID算法中的所有变量和参数
typedef struct {
    float Kp;          // 比例系数（Proportional）：误差越大，输出越大，决定反应速度
    float Ki;          // 积分系数（Integral）：累积历史误差，消除稳态误差
    float Kd;          // 微分系数（Derivative）：误差变化率，抑制超调和震荡
    float setpoint;    // 目标值（期望值），比如0°水平
    float integral;    // 积分项的当前累加值（历史误差的累和）
    float last_error;  // 上一次的误差值（用于计算微分项）
    float output;      // PID输出（本次计算的控制量，直接用来调电机/执行器）
    float integral_max;// 积分最大限幅（防止积分项过大导致积分饱和/失控）
    float output_max;  // 输出最大限幅（防止PID输出过大损坏设备/抖动）
} PID_t;
extern volatile float Real_roll , Real_pitch , Real_yaw ;


// 宏定义
#define MOTOR_PWM_MAX 7200
#define MOTOR_PWM_MIN 0
#endif
