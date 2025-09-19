// mag_calibration.c
#include "mag_calibration.h"
#include "qmc5883l.h"           // 用于读取磁力计
#include <string.h>
#include <stdio.h>

// 全局校准参数
Mag_Calibration_t mag_calib = {0};

// 临时存储校准期间的原始数据
static mag_raw_t calib_samples[CALIBRATION_SAMPLES];
static uint16_t sample_count = 0;

// 滤波系数（必须与 QMC5883L_Read 中一致）
#define CALIB_FILTER_ALPHA 0.3f

/**
 * @brief 收集磁力计原始数据（带滤波）
 */
static void collect_magnetometer_data(void) {
    int16_t last_x = 0, last_y = 0, last_z = 0;
    sample_count = 0;

   // printf(" 开始磁力计校准，请绕 '8' 字形缓慢旋转设备...\r\n");

    while (sample_count < CALIBRATION_SAMPLES) {
        int16_t x_raw, y_raw, z_raw;
        Multiple_Read_QMC5883L(&x_raw, &y_raw, &z_raw);

        // 无效数据过滤（可选）
        if (x_raw == 0 && y_raw == 0 && z_raw == 0) {
            HAL_Delay(10);
            continue;
        }

        // 一阶低通滤波（与 QMC5883L_Read 保持一致）
        int16_t x_filt = (int16_t)(last_x * (1.0f - CALIB_FILTER_ALPHA) + x_raw * CALIB_FILTER_ALPHA);
        int16_t y_filt = (int16_t)(last_y * (1.0f - CALIB_FILTER_ALPHA) + y_raw * CALIB_FILTER_ALPHA);
        int16_t z_filt = (int16_t)(last_z * (1.0f - CALIB_FILTER_ALPHA) + z_raw * CALIB_FILTER_ALPHA);

        last_x = x_filt;
        last_y = y_filt;
        last_z = z_filt;

        // 存储滤波后数据用于校准
        calib_samples[sample_count].x = x_filt;
        calib_samples[sample_count].y = y_filt;
        calib_samples[sample_count].z = z_filt;
        sample_count++;

        printf("Sample %d: X=%d, Y=%d, Z=%d\r\n", sample_count, x_filt, y_filt, z_filt);
        HAL_Delay(50); // 采集间隔 ~20Hz，避免过快
    }

    //printf(" 数据采集完成，共 %d 个样本。\r\n", sample_count);
}

/**
 * @brief 计算校准参数（硬铁 + 软铁）
 */
static void calculate_calibration_params(void) {
    float max_x = -32768.0f, max_y = -32768.0f, max_z = -32768.0f;
    float min_x = 32767.0f, min_y = 32767.0f, min_z = 32767.0f;

    for (int i = 0; i < sample_count; i++) {
        float x = calib_samples[i].x;
        float y = calib_samples[i].y;
        float z = calib_samples[i].z;

        if (x > max_x) max_x = x; if (x < min_x) min_x = x;
        if (y > max_y) max_y = y; if (y < min_y) min_y = y;
        if (z > max_z) max_z = z; if (z < min_z) min_z = z;
    }

    // 检查数据有效性
    if (max_x <= min_x || max_y <= min_y || max_z <= min_z) {
        //printf(" 校准失败：数据范围无效！\r\n");
        memset(&mag_calib, 0, sizeof(Mag_Calibration_t));
        return;
    }

    // 硬铁偏移（中心点）
    mag_calib.bias_x = (max_x + min_x) / 2.0f;
    mag_calib.bias_y = (max_y + min_y) / 2.0f;
    mag_calib.bias_z = (max_z + min_z) / 2.0f;

    // 软铁缩放（归一化到 [-1,1]）
    float range_x = max_x - min_x;
    float range_y = max_y - min_y;
    float range_z = max_z - min_z;

    mag_calib.scale_x = (range_x > 1.0f) ? (2.0f / range_x) : 1.0f;
    mag_calib.scale_y = (range_y > 1.0f) ? (2.0f / range_y) : 1.0f;
    mag_calib.scale_z = (range_z > 1.0f) ? (2.0f / range_z) : 1.0f;

    mag_calib.valid_flag = CALIB_VALID_FLAG;

    printf(" 校准成功！\r\n");
    // Print_Calibration_Params();  // 可选：打印参数
}

/**
 * @brief 保存校准参数到 Flash
 */

void Save_Calibration_To_Flash(void) {
    HAL_StatusTypeDef status = HAL_OK;
    FLASH_EraseInitTypeDef eraseInitStruct = {0};
    uint32_t pageError = 0;

    // === 所有变量定义移到最前面 ===
    uint32_t addr;
    uint32_t *data;
    uint32_t size;
    uint32_t i;

    if (sizeof(Mag_Calibration_t) % 4 != 0) {
        //printf("❌ 校准结构体大小不为 4 的倍数！\r\n");
        return;
    }

    HAL_FLASH_Unlock();

    eraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.PageAddress = CALIB_FLASH_PAGE;
    eraseInitStruct.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) != HAL_OK) {
        //printf("❌ Flash 擦除失败！\r\n");
        status = HAL_ERROR;
        goto exit;
    }

    // === 现在才赋值（不是定义）===
    addr = CALIB_FLASH_PAGE;
    data = (uint32_t*)&mag_calib;
    size = sizeof(Mag_Calibration_t) / 4;

    for (i = 0; i < size; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + i * 4, data[i]) != HAL_OK) {
           // printf("❌ Flash 写入失败 @ 0x%08lX\r\n", addr + i * 4);
            status = HAL_ERROR;
            goto exit;
        }
    }

    if (status == HAL_OK) {
        printf("💾 校准参数已保存到 Flash。\r\n");
    }

exit:
    HAL_FLASH_Lock();
}

/**
 * @brief 从 Flash 加载校准参数
 */
void Load_Calibration_From_Flash(void) {
    Mag_Calibration_t *flash_data = (Mag_Calibration_t*)CALIB_FLASH_PAGE;

    if (flash_data->valid_flag == CALIB_VALID_FLAG) {
        memcpy(&mag_calib, flash_data, sizeof(Mag_Calibration_t));
        //printf(" 从 Flash 加载校准参数。\r\n");
        // Print_Calibration_Params();
    } else {
        //printf(" Flash 中无有效校准数据。\r\n");
        memset(&mag_calib, 0, sizeof(Mag_Calibration_t));
    }
}

/**
 * @brief 执行磁力计校准流程
 */
void Mag_Calibrate(void) {
    collect_magnetometer_data();
    calculate_calibration_params();

    if (mag_calib.valid_flag == CALIB_VALID_FLAG) {
        Save_Calibration_To_Flash();
    } else {
        //printf("未保存校准数据（计算失败）。\r\n");
    }
}

/**
 * @brief 应用校准到原始数据
 * @param cx, cy, cz: 输出校准后的数据（归一化到 [-1,1]）
 * @param rx, ry, rz: 输入原始数据
 */
void Mag_Apply_Calibration(float *cx, float *cy, float *cz, int16_t rx, int16_t ry, int16_t rz) {
    // 应用滤波？❌ 不在这里！滤波应在读取时完成

    // 使用全局校准参数
    *cx = (rx - mag_calib.bias_x) * mag_calib.scale_x;
    *cy = (ry - mag_calib.bias_y) * mag_calib.scale_y;
    *cz = (rz - mag_calib.bias_z) * mag_calib.scale_z;
}

/**
 * @brief 打印当前校准参数（可选调试）
 */
/*
void Print_Calibration_Params(void) {
    if (mag_calib.valid_flag == CALIB_VALID_FLAG) {
        printf("🔧 硬铁偏移 (Bias):   X=%.2f, Y=%.2f, Z=%.2f\r\n",
               mag_calib.bias_x, mag_calib.bias_y, mag_calib.bias_z);
        printf("📐 软铁缩放 (Scale):  X=%.4f, Y=%.4f, Z=%.4f\r\n",
               mag_calib.scale_x, mag_calib.scale_y, mag_calib.scale_z);
    } else {
        printf("❌ 无有效校准数据。\r\n");
    }
}
*/
