#ifndef __FILTER_H
#define __FILTER_H
#include "stdint.h"

//////////////////////PT2�˲���(���׵�ͨ�˲���)////////////////////////////
// ���׵�ͨ�˲����ṹ��
typedef struct {
    float cutoffHz;          // ��ֹƵ��
    float sampleRateHz;      // ����Ƶ��
    float a1;                // �˲���ϵ��
    float a2;                // �˲���ϵ��
    float b0;                // �˲���ϵ��
    float b1;                // �˲���ϵ��
    float b2;                // �˲���ϵ��
    float x1;                // ��һ������
    float x2;                // ���ϴ�����
    float y1;                // ��һ�����
    float y2;                // ���ϴ����
    uint8_t initialized;        // ��ʼ����־
} pt2Filter_t;
void pt2FilterInit(pt2Filter_t *filter, float cutoffHz, float sampleRateHz);
float pt2FilterApply(pt2Filter_t *filter, float input);

/////////////////////�������˲���/////////////////////////
typedef struct {
    float q; // ��������Э����
    float r; // ��������Э����
    float x; // ״̬����ֵ
    float p; // �������Э����
    float k; // ����������
} KalmanFilter;
void KalmanFilter_Init(KalmanFilter *kf, float q,float r,float x,float p);
float KalmanFilter_Update(KalmanFilter *kf, float measurement);

#endif
