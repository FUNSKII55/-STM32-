#include "filter.h"
#include "math.h"

#define M_PI	3.1415926535f
// ��ʼ�����׵�ͨ�˲���
/*filter �ṹ��
 * cutoffHz ����Ƶ��(С���˻�10~50Hz)
 * sampleRateHz ������
 * */
void pt2FilterInit(pt2Filter_t *filter, float cutoffHz, float sampleRateHz) {
    filter->cutoffHz = cutoffHz;
    filter->sampleRateHz = sampleRateHz;
    filter->x1 = 0.0f;
    filter->x2 = 0.0f;
    filter->y1 = 0.0f;
    filter->y2 = 0.0f;
    filter->initialized = 0;

    // �����˲���ϵ��
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

// Ӧ�ö��׵�ͨ�˲���
float pt2FilterApply(pt2Filter_t *filter, float input) {
    if (!filter->initialized) {
        // �״�ʹ�ã���ʼ����ʷֵ
        filter->y2 = filter->y1 = filter->x1 = filter->x2 = input;
        filter->initialized = 1;
        return input;
    }

    // �����ֹƵ��Ϊ0���������˲�
    if (filter->cutoffHz <= 0.0f) {
        return input;
    }

    // �����˲�����ʽ: y0 = b0*x0 + b1*x1 + b2*x2 - a1*y1 - a2*y2
    const float y0 = filter->b0 * input + filter->b1 * filter->x1 + filter->b2 * filter->x2
                   - filter->a1 * filter->y1 - filter->a2 * filter->y2;

    // ������ʷֵ
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
// �������˲����º���
float KalmanFilter_Update(KalmanFilter *kf, float measurement) {
    // Ԥ�ⲽ��
    kf->p = kf->p + kf->q;
    // ���㿨��������
    kf->k = kf->p / (kf->p + kf->r);
    // ���¹���ֵ
    kf->x = kf->x + kf->k * (measurement - kf->x);
    // ���¹������Э����
    kf->p = (1 - kf->k) * kf->p;
    return kf->x;
}
