#include "mpu6050.h"
#include "i2c.h"
#include "math.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef HAL_I2C_MODULE_ENABLED
	#include "i2c.h"
	I2C_HandleTypeDef *MPU6050_I2C=NULL;


void MPU6050_Write_REG(uint8_t reg, uint8_t data){
	uint8_t tmpdata = data;
	//HAL_I2C_Master_Transmit(&hi2c1,reg,&tmpdata,1,1000);
	HAL_I2C_Mem_Write(MPU6050_I2C,0x68 << 1, reg, I2C_MEMADD_SIZE_8BIT, &tmpdata, 1, 500);
}

uint8_t MPU6050_Read_REG(uint8_t reg){
	uint8_t tmpdata = 0;
	//HAL_I2C_Master_Receive(&hi2c1,reg,&tmpdata,1,1000);
	HAL_I2C_Mem_Read(MPU6050_I2C,0x68<<1,reg,I2C_MEMADD_SIZE_8BIT,&tmpdata,1,500);
	return tmpdata;
}

#define MPU6050Wirte_Reg(reg,data) MPU6050_Write_REG(reg,data)//可以替换为你的写寄存器函数
#define MPU6050Read_Reg(reg)    MPU6050_Read_REG(reg)//可以替换为你的读寄存器函数

///////////////////////////下面是寄存器地址定义//////////////////////////////////
#define MPU6050_ADDRESS		      0x68//i2c address
//**************************** register below
#define	MPU6050_SMPLRT_DIV		  0x19
#define	MPU6050_CONFIG			  0x1A
#define	MPU6050_GYRO_CONFIG		  0x1B
#define	MPU6050_ACCEL_CONFIG	  0x1C
#define MPU6050_FIFO_EN           0x23

#define MPU6050_INTBP_CFG_REG	  0X37	//中断寄存器
#define MPU6050_INT_ENABLE        0x38

#define	MPU6050_ACCEL_XOUT_H	  0x3B
#define	MPU6050_ACCEL_XOUT_L	  0x3C
#define	MPU6050_ACCEL_YOUT_H	  0x3D
#define	MPU6050_ACCEL_YOUT_L	  0x3E
#define	MPU6050_ACCEL_ZOUT_H	  0x3F
#define	MPU6050_ACCEL_ZOUT_L	  0x40
#define	MPU6050_TEMP_OUT_H		  0x41
#define	MPU6050_TEMP_OUT_L		  0x42
#define	MPU6050_GYRO_XOUT_H		  0x43
#define	MPU6050_GYRO_XOUT_L		  0x44
#define	MPU6050_GYRO_YOUT_H		  0x45
#define	MPU6050_GYRO_YOUT_L		  0x46
#define	MPU6050_GYRO_ZOUT_H		  0x47
#define	MPU6050_GYRO_ZOUT_L		  0x48
#define MPU6050_SIGNAL_PATH_RESET 0x68
//校准地址不列出
#define MPU6050_USER_CTRL         0x6A
#define	MPU6050_PWR_MGMT_1		  0x6B
#define	MPU6050_WHO_AM_I		  0x75

//////////////////////////////////////////////////////////
typedef enum{//传感器的滤波带宽
    Band_256Hz = 0x00,
    Band_186Hz,
    Band_96Hz,
    Band_43Hz,
    Band_21Hz,
    Band_10Hz,
    Band_5Hz
}Filter_Typedef;

typedef enum{//传感器角速度测量范围
    gyro_250 = 0x00,
    gyro_500 = 0x08,
    gyro_1000 = 0x10,
    gyro_2000 = 0x18
}GYRO_CONFIG_Typedef;

typedef enum{//传感器加速度测量范围
    acc_2g = 0x00,
    acc_4g = 0x08,
    acc_8g = 0x10,
    acc_16g = 0x18
}ACCEL_CONFIG_Typedef;

typedef enum{
    FIFO_Disable,               //关闭FIFO
    Acc_OUT = 0x08,
    Gyro_zOUT = 0x10,
    Gyro_yOUT = 0x20,
    Gyro_xOUT = 0x40,
    Temp_OUT =0x80,
}FIFO_EN_Typedef;

typedef enum{
    interrupt_Disable,          //中断使能
    Data_Ready_EN = 0x01,       
    I2C_Master_EN = 0x08,       //IIC主机模式
    FIFO_overFolow_EN = 0x10,   //FIFO覆盖
    Motion_EN = 0x40,       
}INT_EN_Typedef;
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct MPU6050_InitTypeDef
{
    uint16_t SMPLRT_Rate;              //采样率Hz
    Filter_Typedef Filter;             //滤波器
    GYRO_CONFIG_Typedef gyro_range;    //陀螺仪测量范围
    ACCEL_CONFIG_Typedef acc_range;    //加速度计测量范围
    FIFO_EN_Typedef FIFO_EN;           //FIFO缓冲区使能
    INT_EN_Typedef INT;                //中断使能
}MPU6050_InitTypeDef;
///////////////////////////////////////////////////////////////////////////////////////////////////
static void MPU6050_Register_init(MPU6050_InitTypeDef* this){
    MPU6050Wirte_Reg(MPU6050_PWR_MGMT_1,0x80);//复位
    vTaskDelay(100);
    MPU6050Wirte_Reg(MPU6050_PWR_MGMT_1,0x00);//唤醒
   	uint8_t SMPLRT_DIV;
	if(this->SMPLRT_Rate>=1000)this->SMPLRT_Rate=1000;
	else if(this->SMPLRT_Rate<4)this->SMPLRT_Rate=4;
	SMPLRT_DIV=1000.0f/this->SMPLRT_Rate-1;//由计算公式得
    MPU6050Wirte_Reg(MPU6050_SMPLRT_DIV,SMPLRT_DIV);

    MPU6050Wirte_Reg(MPU6050_INT_ENABLE,this->INT);
    MPU6050Wirte_Reg(MPU6050_CONFIG,this->Filter);
    MPU6050Wirte_Reg(MPU6050_GYRO_CONFIG,this->gyro_range);
    MPU6050Wirte_Reg(MPU6050_ACCEL_CONFIG,this->acc_range);
    MPU6050Wirte_Reg(MPU6050_FIFO_EN,this->FIFO_EN);
    uint8_t temp = 0x00;
    if(this->FIFO_EN!=0x00)//如果打开了FIFO
        temp = 0x40;
    if((this->INT & 0x01)==0)//如果打开了中断
        temp |= 0x08;
    MPU6050Wirte_Reg(MPU6050_USER_CTRL,temp);
    MPU6050Wirte_Reg(MPU6050_PWR_MGMT_1,0x01);//X轴为参考
}

// 传感器校准函数,减小零点漂移
static float gyro_zero_z = 0.0f;
static void MPU6050_SoftCalibrate_Z() {
    uint16_t calibration_samples = 800;//采样次数
    float gz_sum = 0.0f;
    int16_t GyroZ;

    for (uint16_t i = 0; i < calibration_samples; i++) {
        ///////////根据Z轴的变换规律进行修正/////////
        // 读取Z轴数据
        GyroZ = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_ZOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_ZOUT_L);
        gz_sum += (float)GyroZ;
        vTaskDelay(10);//要和dt同步
    }
    gyro_zero_z = gz_sum / calibration_samples;
}

// 卡尔曼滤波器结构体
typedef struct {
    float q; // 过程噪声协方差
    float r; // 测量噪声协方差
    float x; // 状态估计值
    float p; // 估计误差协方差
    float k; // 卡尔曼增益
} MPU6050_KalmanFilter;
// 卡尔曼滤波更新函数
static float KalmanFilter_Update(MPU6050_KalmanFilter *kf, float measurement) {
    // 预测步骤
    kf->p = kf->p + kf->q;
    // 计算卡尔曼增益
    kf->k = kf->p / (kf->p + kf->r);
    // 更新估计值
    kf->x = kf->x + kf->k * (measurement - kf->x);
    // 更新估计误差协方差
    kf->p = (1 - kf->k) * kf->p;
    return kf->x;
}
///////////////////////////////////////////以下为用户可以访问的代码/////////////////////////////////////////////////////

/*例子:MPU6050_init(GPIOA,GPIO_PIN_1,GPIO_PIN_2);
GPIOx:选择你的GPIO
SCl:选择你的GPIO(choose your GPIO)
SDA:选择你的GPIO(choose your GPIO)
注意:SCL和SDA来自同一组GPIO口(notice:SCL and SDA come from a same GPIO port)*/
void MPU6050_init(I2C_HandleTypeDef *hi2c){
	MPU6050_I2C = hi2c;
    MPU6050_InitTypeDef MPU6050_init_Struct;
    MPU6050_init_Struct.SMPLRT_Rate = 200;            //采样率Hz
    MPU6050_init_Struct.Filter = Band_5Hz;            //滤波器带宽
    MPU6050_init_Struct.gyro_range = gyro_250;       //陀螺仪测量范围
    MPU6050_init_Struct.acc_range = acc_2g;          //加速度计测量范围
    MPU6050_init_Struct.FIFO_EN = FIFO_Disable;       //FIFO
    MPU6050_init_Struct.INT = interrupt_Disable;      //中断配置
    MPU6050_Register_init(&MPU6050_init_Struct);      //初始化寄存器
    MPU6050_SoftCalibrate_Z();//软件校准,减少yaw的零点漂移
}

void MPU6050_Get_Raw(MPU6050_raw* this){
    this->AccX = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_XOUT_H))<<8) | MPU6050Read_Reg(MPU6050_ACCEL_XOUT_L);
    this->AccY = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_YOUT_H))<<8) | MPU6050Read_Reg(MPU6050_ACCEL_YOUT_L);
    this->AccZ = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_H))<<8) | MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_L);
    this->GyroX = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_XOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_XOUT_L);
    this->GyroY = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_YOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_YOUT_L);
    this->GyroZ = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_ZOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_ZOUT_L);
    this->Temp = ((uint16_t)(MPU6050Read_Reg(MPU6050_TEMP_OUT_H))<<8) | MPU6050Read_Reg(MPU6050_TEMP_OUT_L);
}

float MPU6050_GetTemp(){//返回温度
	uint8_t temp = (((uint16_t)MPU6050Read_Reg(MPU6050_TEMP_OUT_H)) << 8) | MPU6050Read_Reg(MPU6050_TEMP_OUT_L);
	float temperature = (float)temp/340 + 36.53;
	return temperature;
}

float ACC_abs = 0;
void MPU6050_Get_Angle(MPU6050* this){
    int16_t temp=0;
	float Ax,Ay,Az;
	float Gx,Gy,Gz;
    const static float dt=0.01f;//积分时间10ms

    // 初始化卡尔曼滤波器
    static MPU6050_KalmanFilter kf_roll, kf_pitch;
    static int once_flag = 1;
    if (once_flag) {//只执行一次,修改卡尔曼滤波器:如果需要高速运动加大q，噪声大的时候增大r
        //roll的初始化
        kf_roll.q = 0.001f;
        kf_roll.r = 0.1f;
        kf_roll.x = 0;
        kf_roll.p = 1;
        //Pitch的初始化
        kf_pitch.q = 0.001f;
        kf_pitch.r = 0.1f;
        kf_pitch.x = 0;
        kf_pitch.p = 1;
        once_flag = 0;
    }
    
    /////////////////////////////Ax = temp * 加速度计量程/32768;//////////////////////////////////
	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_ACCEL_XOUT_H) << 8)+MPU6050Read_Reg(MPU6050_ACCEL_XOUT_L);
	Ax = temp * 2.0f/32768;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_ACCEL_YOUT_H) << 8)+MPU6050Read_Reg(MPU6050_ACCEL_YOUT_L);
	Ay = temp * 2.0f/32768;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_H) << 8)+MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_L);
	Az = temp * 2.0f/32768;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_GYRO_XOUT_H) << 8)+MPU6050Read_Reg(MPU6050_GYRO_XOUT_L);
	Gx = temp *dt*0.0174532f;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_GYRO_YOUT_H) << 8)+MPU6050Read_Reg(MPU6050_GYRO_YOUT_L);
	Gy = temp *dt*0.0174532f;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_GYRO_ZOUT_H) << 8) + MPU6050Read_Reg(MPU6050_GYRO_ZOUT_L) - (int16_t)gyro_zero_z;//软件校准减掉漂移值
	Gz = temp *dt*0.0174532f;

    // 计算加速度的绝对值
    float absAcc = sqrt(Ax * Ax + Ay * Ay + Az * Az);
    //ACC_abs = absAcc;
    // 动态调整权重
    float weight;
    if (absAcc > 1.2) {
        // 快速运动或剧烈振动状态，减小加速度计权重
        weight = 0.8f;
    } else {
        // 正常运动状态，强烈信任加速度计
        weight = 1.0f;
    }

    static float Gyroscope_roll=0.0f;
	static float Gyroscope_pitch=0.0f;
	Gyroscope_roll+=Gy;
	Gyroscope_pitch+=Gx;
	float raw_roll=weight * atan2(Ay,Az)/3.1415926f * 180.f + (1-weight) * Gyroscope_roll;
	float raw_pitch=-(weight * atan2(Ax,Az)/3.1415926f * 180.f + (1-weight) * Gyroscope_pitch);
    // 应用卡尔曼滤波
    this->roll = KalmanFilter_Update(&kf_roll, raw_roll);
    this->pitch = KalmanFilter_Update(&kf_pitch, raw_pitch);
    //减小零飘(如果零票严重，可以通过加减一个小数来解决,这是二次手动校准,值得注意的是，每次上电的时候gyro_zero_z的值都会不同
    //，并且VCC也会变换所以这个不是很好的解决办法，不想漂就加磁力计)
    if(fabsf(Gz)>=0.01)
    	this->yaw += Gz/2;
}

//四元素法+动态互补滤波
float Acc_cc = 0;
void MPU6050_Get_Angle_Plus(MPU6050* this) {
    // 四元素参数
    static float q_w = 1.0f;
    static float q_x = 0.0f;
    static float q_y = 0.0f;
    static float q_z = 0.0f;

    // yaw锁定相关变量（使用int8_t替代bool）
    static float locked_yaw = 0.0f;
    static int8_t yaw_locked = 0;  // 0:未锁定, 非0:已锁定
    static float last_gz = 0.0f;
    static uint8_t stable_count = 0;
    const float GYRO_THRESHOLD = 0.002f; // 角速度阈值，可调整
    const uint8_t STABLE_SAMPLES = 20;   // 稳定采样数，可调整

    // 控制器参数
    float _twoKp;
    float _twoKi;
    const float halfT = 0.01f;   // 采样时间10ms

    float recipNorm;
    float qDot1, qDot2, qDot3, qDot4;
    static float _integralFBx = 0.0f, _integralFBy = 0.0f, _integralFBz = 0.0f;

    // 从寄存器读取数据
    int16_t AccX = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_XOUT_H)) << 8) | MPU6050Read_Reg(MPU6050_ACCEL_XOUT_L);
    int16_t AccY = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_YOUT_H)) << 8) | MPU6050Read_Reg(MPU6050_ACCEL_YOUT_L);
    int16_t AccZ = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_H)) << 8) | MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_L);
    int16_t GyroX = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_XOUT_H)) << 8) | MPU6050Read_Reg(MPU6050_GYRO_XOUT_L);
    int16_t GyroY = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_YOUT_H)) << 8) | MPU6050Read_Reg(MPU6050_GYRO_YOUT_L);
    int16_t GyroZ = (((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_ZOUT_H)) << 8) | MPU6050Read_Reg(MPU6050_GYRO_ZOUT_L)) - (int16_t)gyro_zero_z;

    // 数据转换
    float ax = (float)AccX / 16384.0f;
    float ay = (float)AccY / 16384.0f;
    float az = (float)AccZ / 16384.0f;
    float gx = (float)GyroX * 0.000133f;
    float gy = (float)GyroY * 0.000133f;
    float gz = (float)GyroZ * 0.000133f;

    // 计算加速度的绝对值并调整参数
    float absAcc = sqrt(ax * ax + ay * ay + az * az);
    ACC_abs = absAcc;
    if (absAcc > 1.2) {
        _twoKp = 30.0f;
        _twoKi = 0.1f;
    } else {
        _twoKp = 20.0f;
        _twoKi = 0.05f;
    }

    // 检测z轴角速度是否稳定
    if (fabs(gz - last_gz) < GYRO_THRESHOLD) {
        stable_count++;
    } else {
        stable_count = 0;
        yaw_locked = 0;  // 解除锁定
    }
    last_gz = gz;

    // 当角速度连续稳定时锁定yaw
    if (stable_count >= STABLE_SAMPLES && yaw_locked == 0) {
        yaw_locked = 1;  // 设置锁定状态
        locked_yaw = atan2f(2 * (q_w * q_z + q_x * q_y), 1 - 2 * (q_y * q_y + q_z * q_z)) * 57.29578f;
    }

    // 计算四元数的导数
    qDot1 = 0.5f * (-q_x * gx - q_y * gy - q_z * gz);
    qDot2 = 0.5f * (q_w * gx + q_y * gz - q_z * gy);
    qDot3 = 0.5f * (q_w * gy - q_x * gz + q_z * gx);
    qDot4 = 0.5f * (q_w * gz + q_x * gy - q_y * gx);

    // 归一化加速度计测量值
    recipNorm = 1.0f / sqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;

    // 辅助变量来计算四元数的乘积
    float q0q0 = q_w * q_w;
    float q0q1 = q_w * q_x;
    float q0q2 = q_w * q_y;
    float q1q3 = q_x * q_z;
    float q2q3 = q_y * q_z;
    float q3q3 = q_z * q_z;

    // 参考方向的重力
    float halfvx = q1q3 - q0q2;
    float halfvy = q0q1 + q2q3;
    float halfvz = q0q0 - 0.5f + q3q3;

    // 误差是测量方向和参考方向之间的叉积
    float halfex = (ay * halfvz - az * halfvy);
    float halfey = (az * halfvx - ax * halfvz);
    float halfez = (ax * halfvy - ay * halfvx);

    // 积分误差比例积分增益
    if (_twoKi > 0.0f) {
        _integralFBx += _twoKi * halfex * halfT;
        _integralFBy += _twoKi * halfey * halfT;
        _integralFBz += _twoKi * halfez * halfT;
        gx += _integralFBx;
        gy += _integralFBy;
        gz += _integralFBz;
    } else {
        _integralFBx = 0.0f;
        _integralFBy = 0.0f;
        _integralFBz = 0.0f;
    }

    // 应用比例增益
    gx += _twoKp * halfex;
    gy += _twoKp * halfey;
    gz += _twoKp * halfez;

    // 更新四元数导数
    qDot1 -= q_x * halfex + q_y * halfey + q_z * halfez;
    qDot2 += q_w * halfex - q_z * halfey + q_y * halfez;
    qDot3 += q_z * halfex + q_w * halfey - q_x * halfez;
    qDot4 += -q_y * halfex + q_x * halfey + q_w * halfez;

    // 积分四元数导数
    q_w += qDot1 * halfT;
    q_x += qDot2 * halfT;
    q_y += qDot3 * halfT;
    q_z += qDot4 * halfT + 0.00003f; // 手动补偿yaw零点漂移

    // 归一化四元数
    recipNorm = 1.0f / sqrt(q_w * q_w + q_x * q_x + q_y * q_y + q_z * q_z);
    if (isnan(recipNorm) || isinf(recipNorm)) {
        // 处理异常情况，如四元数计算错误导致的无效值
        q_w = 1.0f;
        q_x = 0.0f;
        q_y = 0.0f;
        q_z = 0.0f;
        recipNorm = 1.0f;
    }
    q_w *= recipNorm;
    q_x *= recipNorm;
    q_y *= recipNorm;
    q_z *= recipNorm;

    // 将弧度转换为角度
    this->roll = atan2f(2 * (q_w * q_x + q_y * q_z), 1 - 2 * (q_x * q_x + q_y * q_y)) * 57.29578f - 2.3;
    this->pitch = asinf(2 * (q_w * q_y - q_z * q_x)) * 57.29578f + 5;

    // 使用锁定的yaw值或计算新的yaw值
    if (yaw_locked) {
        this->yaw = locked_yaw;
    } else {
        this->yaw = atan2f(2 * (q_w * q_z + q_x * q_y), 1 - 2 * (q_y * q_y + q_z * q_z)) * 57.29578f;
    }
}

uint8_t MPU6050_ID(){
    return MPU6050Read_Reg(MPU6050_WHO_AM_I);
}
#endif
