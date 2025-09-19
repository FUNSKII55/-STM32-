#include "key.h"
#include "menu.h"
#include "OLED.h"  // �������Ҫ��ʾ��ȥ��
#include "Flash.h"
#include "stdint.h"
#include "main.h"
#include "u8g2.h"
#include "UI.h"
#include "UI_PID.h"
#include "FreeRTOS.h"
#include "task.h"

#define CHORD_WIN_MS 80    // ��ϼ�ʱ�䴰������Ҫ�� 50~120ms


extern u8g2_t u8g2;           // �� main.c ����ȫ�ֶ��壺u8g2_t u8g2;
keys_t key[2] = {0};

void key_scan(void)
{
    /* ��ȡ�������ŵ�ƽ */
    key[0].ic_key = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12);
    key[1].ic_key = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);


    /* ����ÿ��������״̬�� */
    for (uint8_t i = 0; i < 2; i++)
    {
        switch (key[i].sta)
        {
        case 0: // �ȴ�����
            if (key[i].ic_key == GPIO_PIN_RESET) {
                key[i].sta = 1;
                key[i].key_time = 0;
            }
            break;

        case 1: // �����׶�
            if (key[i].ic_key == GPIO_PIN_RESET) {
                key[i].sta = 2;
            } else {
                key[i].sta = 0;
            }
            break;

        case 2: // ���±���
            if (key[i].ic_key == GPIO_PIN_SET && key[i].key_time < LONG_TICKS) {
                key[i].sta = 0;
                key[i].state = 1; // �̰�
            }
            else if (key[i].ic_key == GPIO_PIN_SET && key[i].key_time >= LONG_TICKS) {
                key[i].state = 2; // ����
                key[i].sta = 0;
            }
            key[i].key_time++;
            break;
        }
    }
}

void key_pd(void)
{
    /* ���� ͬʱ���µ���ϼ������ж��� return ���� */

    // �� ������ͬʱ������ �� ���ؼ�����Ҳ���Ըĳɻ��ϼ���menu_index = menu_table[menu_index].left��
    if (key[0].state == 2 && key[1].state == 2) {
        // TODO: ���ؼ�����
        menu_index = menu_table[menu_index].two_long;            // ���������˵������� left �ֶοɸĳ� menu_table[menu_index].left��
        menu_clamp();

        key[0].state = 0;
        key[1].state = 0;
        return;
    }

    // �� ������ͬʱ�̰��� �� ѡ���м����
    if (key[0].state == 1 && key[1].state == 1) {
        // TODO: ѡ�������
        menu_index = menu_table[menu_index].two_short;   // ����� ��right�� ��������/ѡ��
        menu_clamp();

        key[0].state = 0;
        key[1].state = 0;
        return;
    }

    /* ���� ��������ԭ���ĵ������� ���� */

    /* �̰����� */
    if (key[0].state == 1) {
        menu_index = menu_table[menu_index].left;   // �Ҽ��̰�
        menu_clamp();
        key[0].state = 0;
    }
		
    else if (key[1].state == 1) {
        menu_index = menu_table[menu_index].right;   // �ϼ��̰�����ԭ������ͬ��������
        menu_clamp();
        key[1].state = 0;
    }

    /* �������� */
    if (key[0].state == 2) {
			
			  menu_index = menu_table[menu_index].lon_left;   // �Ҽ��̰�
        menu_clamp();
			
        uint8_t x = MENU_CURRENT;
        if (x == 0) {
            cur_y = aim_y;
            HAL_Delay(1);
            aim_y -= 11;
        }
        key[0].state = 0;
    }
    else if (key[1].state == 2) {
			
			  menu_index = menu_table[menu_index].lon_right;   // �Ҽ��̰�
        menu_clamp();
			
        uint8_t x = MENU_CURRENT;
        if (x == 0) {
            cur_y = aim_y;
            HAL_Delay(1);
            aim_y += 11;
        }
        key[1].state = 0;
    }
}

