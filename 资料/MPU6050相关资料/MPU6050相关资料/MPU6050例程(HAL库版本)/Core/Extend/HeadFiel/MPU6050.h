#ifndef MPU6050_H
#define MPU6050_H
#include "main.h"
/*
bilibili 小努班 UID:437280309
@time时间: 2025.5.18
@version版本:V1_5
@Encoding :UTF-8
@attention : 注意杜邦线别用劣质的，还有如果零飘严重可以进入函数
更新:强效减小零点漂移

if you can't display Chinese correctly,please check your encoding mode(please set encoding mode to UTF-8)
使用示例:
MPU6050 MM;
//10ms中断回调
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim_baseHandle)
{
  if (tim_baseHandle->Instance == TIM6)
	  MPU6050_Get_Angle_Plus(&MM);//获得角度s
}
void main(){
	MPU6050_init(&hi2c2);//初始化MPU6050
	while(1){
		printf("%.2f,%.2f,%.4f\n",MM.pitch,MM.roll,MM.yaw);//使用串口发送数据
	}
}
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

#ifdef HAL_I2C_MODULE_ENABLED//如果你没有打开硬件IIC,那么我们将禁止你使用
	/////////////////////////常用函数//////////////////////////
	void MPU6050_init(I2C_HandleTypeDef *hi2c);   						//初始化姿态传感器,使用硬件IIC
	void MPU6050_Get_Angle_Plus(MPU6050* this) ;                        //得到角度(四元素法+互补滤波)
	void MPU6050_Get_Angle(MPU6050* this);                              //得到角度(正常方法+卡尔曼滤波+互补滤波)
	float MPU6050_GetTemp(void);                                        //得到温度

	/////////////////////////扩展函数//////////////////////////
	uint8_t MPU6050_ID(void);                                           //读取ID(用来检测是否于芯片通信上)0:失败
	void MPU6050_Get_Raw(MPU6050_raw* this);                            //得到原生数据
#endif

#endif
