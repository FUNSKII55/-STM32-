// mag_calibration.h
#ifndef MAG_CALIBRATION_H
#define MAG_CALIBRATION_H

#include <stdint.h>

// 校准采样数量（建议 100~200）
#define CALIBRATION_SAMPLES   150

// Flash 存储地址（必须是页起始地址，如 STM32F1 第 63 页）
#define CALIB_FLASH_PAGE      0x0801F800  // STM32F103RCT6 最后一页

// 校准有效标志
#define CALIB_VALID_FLAG      0xA5A5A5A5UL

// 原始数据结构
typedef struct {
    int16_t x, y, z;
} mag_raw_t;

// 校准参数结构（必须 4 字节对齐，便于 Flash 写入）
typedef struct {
    float bias_x, bias_y, bias_z;     // 硬铁偏移
    float scale_x, scale_y, scale_z;  // 软铁缩放
    uint32_t valid_flag;              // 有效标志
} Mag_Calibration_t;

// 全局变量
extern Mag_Calibration_t mag_calib;

// 函数声明
void Mag_Calibrate(void);
void Load_Calibration_From_Flash(void);
void Save_Calibration_To_Flash(void);
void Mag_Apply_Calibration(float *cx, float *cy, float *cz, int16_t rx, int16_t ry, int16_t rz);
// void Print_Calibration_Params(void);  // 可选调试

#endif /* MAG_CALIBRATION_H */
