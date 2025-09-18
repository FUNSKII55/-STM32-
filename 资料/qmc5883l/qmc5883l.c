#include "qmc5883l.h"
#include "mag_calibration.h"  // 获取 mag_calib 结构体
#include <math.h>

extern I2C_HandleTypeDef hi2c2;

#define QMC5883L_ADDRESS 0x1A  // 7-bit 地址，左移一位后的地址

#define REG_XOUT_LSB 0x00  // 数据起始寄存器
#define REG_CONFIG_1 0x09  // 控制寄存器 1
#define REG_CONFIG_2 0x0A  // 控制寄存器 2
#define REG_RESET    0x0B  // 复位寄存器

// 缓冲区用于存储从传感器读取的数据
uint8_t QMC5883L_BUF[6] = {0};

// 单字节写入 QMC5883L
void Single_Write_QMC5883L(uint8_t reg, uint8_t data) {
    HAL_I2C_Mem_Write(&hi2c2, QMC5883L_ADDRESS << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

// 单字节读取 QMC5883L
uint8_t Single_Read_QMC5883L(uint8_t reg) {
    uint8_t data;
    HAL_I2C_Mem_Read(&hi2c2, QMC5883L_ADDRESS << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    return data;
}

/**
 * @brief 初始化 QMC5883L
 */
void Init_QMC5883L(void) {
    HAL_Delay(100);  // 上电延迟

    // 配置寄存器 0x09: [OSR:2][RNG:2][ODR:2][MODE:2]
    // 0x0D = 0001 1101 → OSR=512, RNG=8G, ODR=100Hz, MODE=Continuous
    Single_Write_QMC5883L(REG_CONFIG_1, 0x0D);

    // 配置寄存器 0x0A: [SOFT_RST:1][INT_ENA:1][OUT_POR:1][Reserved:5]
    // 0x01 = 启用连续测量模式
    Single_Write_QMC5883L(REG_CONFIG_2, 0x01);

    // 复位（可选）
    Single_Write_QMC5883L(REG_RESET, 0x01);

    HAL_Delay(10);
}

/**
 * @brief 从 QMC5883L 读取 X、Y、Z 轴磁场原始数据
 */
void Multiple_Read_QMC5883L(int16_t *x_mag, int16_t *y_mag, int16_t *z_mag) {
    HAL_I2C_Mem_Read(&hi2c2, QMC5883L_ADDRESS << 1, REG_XOUT_LSB, I2C_MEMADD_SIZE_8BIT, QMC5883L_BUF, 6, HAL_MAX_DELAY);

    *x_mag = (int16_t)((QMC5883L_BUF[1] << 8) | QMC5883L_BUF[0]);
    *y_mag = (int16_t)((QMC5883L_BUF[3] << 8) | QMC5883L_BUF[2]);
    *z_mag = (int16_t)((QMC5883L_BUF[5] << 8) | QMC5883L_BUF[4]);
}

/**
 * @brief 读取磁力计数据并转换为微特斯拉 (μT)，带一阶低通滤波
 */
void QMC5883L_Read(float *mx, float *my, float *mz) {
    static int16_t last_x = 0, last_y = 0, last_z = 0;
    const float alpha = 0.3f;  // 滤波系数（可调：0.1~0.5）

    int16_t x_raw, y_raw, z_raw;
    Multiple_Read_QMC5883L(&x_raw, &y_raw, &z_raw);

    // 一阶低通滤波
    int16_t x_filtered = (int16_t)(last_x * (1.0f - alpha) + x_raw * alpha);
    int16_t y_filtered = (int16_t)(last_y * (1.0f - alpha) + y_raw * alpha);
    int16_t z_filtered = (int16_t)(last_z * (1.0f - alpha) + z_raw * alpha);

    last_x = x_filtered;
    last_y = y_filtered;
    last_z = z_filtered;

    // 应用磁力计校准（去偏置、缩放、椭圆拟合等）
    // 注意：输入是 raw 或 filtered 数据，必须与你校准时的数据预处理方式一致！
    float mx_cal, my_cal, mz_cal;
    Mag_Apply_Calibration(&mx_cal, &my_cal, &mz_cal, x_filtered, y_filtered, z_filtered);

    // 转换为物理单位 μT
    // QMC5883L 默认 ±8 Gauss → 8G = 800 μT
    // 原始值范围：±32768 → 1 LSB = 800 / 32768 ≈ 0.02442 μT/LSB
    const float MAG_SCALE = 0.02442f;

    // 应用物理单位缩放（校准后值再转单位）
    *mx = -mx_cal * MAG_SCALE;  // 负号：匹配 MPU6050 坐标系（X前，Y左，Z上）
    *my = -my_cal * MAG_SCALE;
    *mz =  mz_cal * MAG_SCALE;
}
