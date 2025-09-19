#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* ====== 配置 ====== */
#define SCAN_PERIOD_MS   10u    // 扫描周期（务必每10ms调用一次 key_scan）
#define LONG_MS          700u   // 长按时长
#define LONG_TICKS       (LONG_MS / SCAN_PERIOD_MS)

/**
 * @brief 按键结构体
 */
typedef struct {
    uint8_t sta;       // 状态机状态：0等待，1消抖，2按下保持
    uint8_t ic_key;    // 当前采样电平
    uint8_t state;     // 0无事件，1短按，2长按
    uint16_t key_time; // 持续时间 (SCAN_PERIOD_MS 单位)
} keys_t;

/**
 * @brief 全局按键数组
 */
extern keys_t key[2];

/**
 * @brief 按键扫描函数（需每 SCAN_PERIOD_MS 调用一次）
 */
void key_scan(void);

/**
 * @brief 按键事件处理（用户功能写在这里）
 */
void key_pd(void);

#endif
