#include "filter.h"
#include "math.h"

#define M_PI	3.1415926535f
// 初始化二阶低通滤波器
/*filter 结构体
 * cutoffHz 截至频率(小无人机10~50Hz)
 * sampleRateHz 采样率
 * */
void pt2FilterInit(pt2Filter_t *filter, float cutoffHz, float sampleRateHz) {
    filter->cutoffHz = cutoffHz;
    filter->sampleRateHz = sampleRateHz;
    filter->x1 = 0.0f;
    filter->x2 = 0.0f;
    filter->y1 = 0.0f;
    filter->y2 = 0.0f;
    filter->initialized = 0;

    // 计算滤波器系数
    if (cutoffHz > 0.0f && sampleRateHz > 0.0f) {
        const float fr = sampleRateHz / cutoffHz;
        const float ohm = tanf(M_PI / fr);
        const float c = 1.0f + 2.0f * cosf(M_PI / 4.0f) * ohm + ohm * ohm;

        filter->b0 = ohm * ohm / c;
        filter->b1 = 2.0f * filter->b0;
        filter->b2 = filter->b0;
        filter->a1 = 2.0f * (ohm * ohm - 1.0f) / c;
        filter->a2 = (1.0f - 2.0f * cosf(M_PI / 4.0f) * ohm + ohm * ohm) / c;
    }
}

// 应用二阶低通滤波器
float pt2FilterApply(pt2Filter_t *filter, float input) {
    if (!filter->initialized) {
        // 首次使用，初始化历史值
        filter->y2 = filter->y1 = filter->x1 = filter->x2 = input;
        filter->initialized = 1;
        return input;
    }

    // 如果截止频率为0，不进行滤波
    if (filter->cutoffHz <= 0.0f) {
        return input;
    }

    // 二阶滤波器公式: y0 = b0*x0 + b1*x1 + b2*x2 - a1*y1 - a2*y2
    const float y0 = filter->b0 * input + filter->b1 * filter->x1 + filter->b2 * filter->x2
                   - filter->a1 * filter->y1 - filter->a2 * filter->y2;

    // 更新历史值
    filter->x2 = filter->x1;
    filter->x1 = input;
    filter->y2 = filter->y1;
    filter->y1 = y0;

    return y0;
}

void KalmanFilter_Init(KalmanFilter *kf, float q,float r,float x,float p) {
	kf->q = q;
	kf->r = r;
	kf->x = x;
	kf->p = p;
}
// 卡尔曼滤波更新函数
float KalmanFilter_Update(KalmanFilter *kf, float measurement) {
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
