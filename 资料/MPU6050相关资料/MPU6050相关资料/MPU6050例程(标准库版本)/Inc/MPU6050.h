#ifndef MPU6050_H
#define MPU6050_H
#include "stm32f10x.h"
/*
bilibili 小努班 UID:437280309
@time时间: 2025.5.5
@version版本:V1_4
@Encoding :UTF-8
@attention : 注意杜邦线别用劣质的，还有如果零飘严重可以进入函数
1.强效减少Yaw角漂移

if you can't display Chinese correctly,please check your encoding mode(please set encoding mode to UTF-8)
*/

//陀螺仪原生数据结构体
typedef struct MPU6050_raw
{
    int16_t AccX;
    int16_t AccY;
    int16_t AccZ;
    int16_t GyroX;
    int16_t GyroY;
    int16_t GyroZ;
    uint16_t Temp;
}MPU6050_raw;
//陀螺仪角度结构体
typedef struct MPU6050
{
    float yaw;
    float roll;
    float pitch;
}MPU6050;

/////////////////////////常用函数//////////////////////////
void MPU6050_init(GPIO_TypeDef* GPIOx,uint16_t SCl,uint16_t SDA);   //初始化姿态传感器
void MPU6050_Get_Angle_Plus(MPU6050* this) ;                        //得到角度(四元素法+互补滤波)
void MPU6050_Get_Angle(MPU6050* this);                              //得到角度(正常方法+卡尔曼滤波+互补滤波)
float MPU6050_GetTemp(void);                                        //得到温度

/////////////////////////扩展函数//////////////////////////
uint8_t MPU6050_ID(void);                                           //读取ID
void MPU6050_Get_Raw(MPU6050_raw* this);                            //得到原生数据

#endif
