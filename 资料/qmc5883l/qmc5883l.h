#ifndef QMC5883L_H
#define QMC5883L_H

#include "stm32f1xx_hal.h"  // 包含 HAL 库定义（如 I2C_HandleTypeDef）

#ifdef __cplusplus
extern "C" {
#endif

// ================== 函数声明 ==================

/**
 * @brief 初始化 QMC5883L 磁力计
 * 
 * 配置传感器为连续测量模式，量程 ±8 Gauss，输出速率 100Hz
 */
void Init_QMC5883L(void);

/**
 * @brief 从 QMC5883L 读取原始磁场数据（int16_t）
 * 
 * @param x_mag 指向存储 X 轴原始值的变量
 * @param y_mag 指向存储 Y 轴原始值的变量
 * @param z_mag 指向存储 Z 轴原始值的变量
 */
void Multiple_Read_QMC5883L(int16_t *x_mag, int16_t *y_mag, int16_t *z_mag);

/**
 * @brief 读取磁场数据并转换为微特斯拉 (μT)，带一阶低通滤波
 * 
 * @param mx 指向存储 X 轴磁场值（单位：μT）的变量
 * @param my 指向存储 Y 轴磁场值（单位：μT）的变量
 * @param mz 指向存储 Z 轴磁场值（单位：μT）的变量
 */
void QMC5883L_Read(float *mx, float *my, float *mz);

// ================== 可选：中断支持 ==================

/**
 * @brief 启用 QMC5883L 数据准备好中断（可选功能）
 */
void Enable_QMC5883L_Interrupt(void);

#ifdef __cplusplus
}
#endif

#endif // QMC5883L_H
