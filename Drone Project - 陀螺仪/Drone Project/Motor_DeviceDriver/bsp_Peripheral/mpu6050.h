#ifndef MPU6050_H
#define MPU6050_H
#include "main.h"
/*
bilibili 小努班 UID:437280309
@time时间: 2025.8.25
@version版本:V1_6
@Encoding :UTF-8
@attention:
1.注意杜邦线别用劣质的
2.如果打开INT要确保连接一定要好，否则mpu6050_Get_Angle_Plus会突然失控(这也是使用其的弊端)
3.注意如果ADD接了1，iic地址是0x69,接0为0x68
4.是否可以两个滤波器都打开?(额可以)
5.如果你使用杜邦线连接，如果线接触不良，MPU6050_Get_Angle_Plus可能会崩溃

@update:
1.修复mpu6050_Get_Angle_Plus中角度突变的问题
2.更新MPU6050_SoftCalibrate_Z函数确保校准正确
3.强效减小mpu6050_Get_Angle_Plus中零点漂移
4.合并MPU6050_raw与MPU6050结构体
5.取消内嵌卡尔曼滤波器
6.修复MPU6050_Get_Angle_Plus里的pitch角的万向锁问题。
7.新增PT2二阶低通滤波器

if you can't display Chinese correctly,please check your encoding mode(please set encoding mode to UTF-8)
*/

#define MPU6050_USE_Filter 1 // 是否使用滤波器(如果你没有我的filter.h那么你设置成1也没有用会报错)
#define MPU6050_ADDRESS           0x68 // i2c address
typedef struct MPU6050 {
    // 原始数据
    int16_t AccX;
    int16_t AccY;
    int16_t AccZ; // 加速度
    int16_t GyroX;
    int16_t GyroY;
    int16_t GyroZ;   // 角速度
    int16_t rawTemp; // 原始温度
    // 角度
    float yaw;
    float roll;
    float pitch;
    float Temp; // 实际物理温度
} MPU6050;

#ifdef HAL_I2C_MODULE_ENABLED//如果你没有打开硬件IIC,那么我们将禁止你使用
	/////////////////////////常用函数//////////////////////////
	void MPU6050_init(I2C_HandleTypeDef *hi2c);   						//初始化姿态传感器,使用硬件IIC
	void MPU6050_Get_Angle_Plus(MPU6050* this) ;                        //得到角度(Madgwick+自适应+无万向锁)
	void MPU6050_Get_Angle(MPU6050* this);                              //得到角度(正常方法+卡尔曼滤波+互补滤波)
	void MPU6050_Set_Angle0(MPU6050* this);//把角度设置为0(让上电的时候角度都从0开始,前提是让传感器采集数据稳定后)

	/////////////////////////扩展函数//////////////////////////
	uint8_t MPU6050_ID(void);             // 读取ID
	void MPU6050_GetTemp(MPU6050 *this); // 得到温度
#endif

#endif
