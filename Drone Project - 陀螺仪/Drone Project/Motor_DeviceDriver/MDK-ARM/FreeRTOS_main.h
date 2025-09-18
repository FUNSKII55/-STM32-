#ifndef __FreeRTOS_H
#define __FreeRTOS_H

void FreeRTOS_Start(void);
// PID�������ṹ�壨�����ڵ���ĽǶȻ��ٶȻ���
// ���ڴ洢�͹���PID�㷨�е����б����Ͳ���
typedef struct {
    float Kp;          // ����ϵ����Proportional�������Խ�����Խ�󣬾�����Ӧ�ٶ�
    float Ki;          // ����ϵ����Integral�����ۻ���ʷ��������̬���
    float Kd;          // ΢��ϵ����Derivative�������仯�ʣ����Ƴ�������
    float setpoint;    // Ŀ��ֵ������ֵ��������0��ˮƽ
    float integral;    // ������ĵ�ǰ�ۼ�ֵ����ʷ�����ۺͣ�
    float last_error;  // ��һ�ε����ֵ�����ڼ���΢���
    float output;      // PID��������μ���Ŀ�������ֱ�����������/ִ������
    float integral_max;// ��������޷�����ֹ����������»��ֱ���/ʧ�أ�
    float output_max;  // �������޷�����ֹPID����������豸/������
} PID_t;
extern volatile float Real_roll , Real_pitch , Real_yaw ;


// �궨��
#define MOTOR_PWM_MAX 7200
#define MOTOR_PWM_MIN 0
#endif
