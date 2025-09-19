#ifndef __FILTER_H
#define __FILTER_H
#include "stdint.h"

//////////////////////PT2滤波器(二阶低通滤波器)////////////////////////////
// 二阶低通滤波器结构体
typedef struct {
    float cutoffHz;          // 截止频率
    float sampleRateHz;      // 采样频率
    float a1;                // 滤波器系数
    float a2;                // 滤波器系数
    float b0;                // 滤波器系数
    float b1;                // 滤波器系数
    float b2;                // 滤波器系数
    float x1;                // 上一次输入
    float x2;                // 上上次输入
    float y1;                // 上一次输出
    float y2;                // 上上次输出
    uint8_t initialized;        // 初始化标志
} pt2Filter_t;
void pt2FilterInit(pt2Filter_t *filter, float cutoffHz, float sampleRateHz);
float pt2FilterApply(pt2Filter_t *filter, float input);

/////////////////////卡尔曼滤波器/////////////////////////
typedef struct {
    float q; // 过程噪声协方差
    float r; // 测量噪声协方差
    float x; // 状态估计值
    float p; // 估计误差协方差
    float k; // 卡尔曼增益
} KalmanFilter;
void KalmanFilter_Init(KalmanFilter *kf, float q,float r,float x,float p);
float KalmanFilter_Update(KalmanFilter *kf, float measurement);

#endif
