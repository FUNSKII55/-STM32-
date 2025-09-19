#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* ====== ���� ====== */
#define SCAN_PERIOD_MS   10u    // ɨ�����ڣ����ÿ10ms����һ�� key_scan��
#define LONG_MS          700u   // ����ʱ��
#define LONG_TICKS       (LONG_MS / SCAN_PERIOD_MS)

/**
 * @brief �����ṹ��
 */
typedef struct {
    uint8_t sta;       // ״̬��״̬��0�ȴ���1������2���±���
    uint8_t ic_key;    // ��ǰ������ƽ
    uint8_t state;     // 0���¼���1�̰���2����
    uint16_t key_time; // ����ʱ�� (SCAN_PERIOD_MS ��λ)
} keys_t;

/**
 * @brief ȫ�ְ�������
 */
extern keys_t key[2];

/**
 * @brief ����ɨ�躯������ÿ SCAN_PERIOD_MS ����һ�Σ�
 */
void key_scan(void);

/**
 * @brief �����¼������û�����д�����
 */
void key_pd(void);

#endif
