#include "key.h"
#include "menu.h"
#include "OLED.h"  // 如果不需要显示可去掉
#include "Flash.h"
#include "stdint.h"
#include "main.h"
#include "u8g2.h"
#include "UI.h"
#include "UI_PID.h"
#include "FreeRTOS.h"
#include "task.h"

#define CHORD_WIN_MS 80    // 组合键时间窗，按需要调 50~120ms


extern u8g2_t u8g2;           // 由 main.c 顶部全局定义：u8g2_t u8g2;
keys_t key[2] = {0};

void key_scan(void)
{
    /* 读取按键引脚电平 */
    key[0].ic_key = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12);
    key[1].ic_key = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);


    /* 遍历每个按键的状态机 */
    for (uint8_t i = 0; i < 2; i++)
    {
        switch (key[i].sta)
        {
        case 0: // 等待按下
            if (key[i].ic_key == GPIO_PIN_RESET) {
                key[i].sta = 1;
                key[i].key_time = 0;
            }
            break;

        case 1: // 消抖阶段
            if (key[i].ic_key == GPIO_PIN_RESET) {
                key[i].sta = 2;
            } else {
                key[i].sta = 0;
            }
            break;

        case 2: // 按下保持
            if (key[i].ic_key == GPIO_PIN_SET && key[i].key_time < LONG_TICKS) {
                key[i].sta = 0;
                key[i].state = 1; // 短按
            }
            else if (key[i].ic_key == GPIO_PIN_SET && key[i].key_time >= LONG_TICKS) {
                key[i].state = 2; // 长按
                key[i].sta = 0;
            }
            key[i].key_time++;
            break;
        }
    }
}

void key_pd(void)
{
    /* —— 同时按下的组合键：先判定并 return —— */

    // ① 两键“同时长按” → 返回键（你也可以改成回上级：menu_index = menu_table[menu_index].left）
    if (key[0].state == 2 && key[1].state == 2) {
        // TODO: 返回键动作
        menu_index = menu_table[menu_index].two_long;            // 例：回主菜单（若有 left 字段可改成 menu_table[menu_index].left）
        menu_clamp();

        key[0].state = 0;
        key[1].state = 0;
        return;
    }

    // ② 两键“同时短按” → 选择（中间键）
    if (key[0].state == 1 && key[1].state == 1) {
        // TODO: 选择键动作
        menu_index = menu_table[menu_index].two_short;   // 这里把 “right” 当作进入/选择
        menu_clamp();

        key[0].state = 0;
        key[1].state = 0;
        return;
    }

    /* —— 以下是你原来的单键处理 —— */

    /* 短按处理 */
    if (key[0].state == 1) {
        menu_index = menu_table[menu_index].left;   // 右键短按
        menu_clamp();
        key[0].state = 0;
    }
		
    else if (key[1].state == 1) {
        menu_index = menu_table[menu_index].right;   // 上键短按（你原本就是同样动作）
        menu_clamp();
        key[1].state = 0;
    }

    /* 长按处理 */
    if (key[0].state == 2) {
			
			  menu_index = menu_table[menu_index].lon_left;   // 右键短按
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
			
			  menu_index = menu_table[menu_index].lon_right;   // 右键短按
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

