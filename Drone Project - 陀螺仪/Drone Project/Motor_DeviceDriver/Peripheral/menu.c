#include "menu.h"
#include "u8g2.h"
#include "main.h"
#include "FreeRTOS_main.h"

// 只保留必须的头文件即可，其他如 i2c.h / Flash.h 若本文件不需要可去掉
// #include "i2c.h"
// #include "Flash.h"
// #include "Voltage Sensing.h"

extern u8g2_t u8g2;           // 由 main.c 顶部全局定义：u8g2_t u8g2;

unsigned char menu_index = 0; // 当前索引 —— 真正定义在这里
unsigned int Power_Grids = 0;


/* ========= 各菜单节点的显示/动作 ========= */

void op_main(void)
{
	



		char buf[24];		char buff[24];		char bufff[24]; char buffff[24];

		snprintf(buf,   sizeof(buf),   "Real_yaw: %.2f",   Real_yaw);
		snprintf(buff,  sizeof(buff),  "Real_pitch: %.2f", Real_pitch);
		snprintf(bufff, sizeof(bufff), "Real_roll: %.2f",  Real_roll);
	  u8g2_ClearBuffer(&u8g2);

		u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);

		u8g2_DrawStr(&u8g2, 0, 10, buf);
		u8g2_DrawStr(&u8g2, 0, 20, buff);
		u8g2_DrawStr(&u8g2, 0, 30, bufff);
	  u8g2_SendBuffer(&u8g2);

//		draw_squashed_rframe(&u8g2, 0, cur_y, (int)cur_w, 18, 3, 2);
		
		for(int i = 0;i < Power_Grids;i++)
			{
				int j = (i)*5+3;
				u8g2_DrawBox(&u8g2,j,15,4,12);
				j+=5;
			}
		
//		char Vol[16];
//		sprintf(Vol, "Vol: %.2f", voltage);     
//    u8g2_DrawStr(&u8g2, 70, 15, Vol);
		
	

		char Vol2[16];
//		sprintf(Vol2, "%d%%", soc);     
    u8g2_DrawStr(&u8g2, 46, 27, Vol2);
    u8g2_SendBuffer(&u8g2);    

}


void op_settings(void) {

  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, u8g2_font_6x12_tf);
	
  u8g2_SendBuffer(&u8g2);
	
	
}

void op_power(void)
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x12_tf);
    u8g2_DrawStr(&u8g2, 0, 15, "2");
    u8g2_SendBuffer(&u8g2);
}

void test1(void)
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x12_tf);
    u8g2_DrawStr(&u8g2, 0, 15, "5");
    u8g2_SendBuffer(&u8g2);
}

void test2(void)
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x12_tf);
    u8g2_DrawStr(&u8g2, 0, 15, "6");
    u8g2_SendBuffer(&u8g2);
}

/* ========= 查表（只读） ========= */

const key_table menu_table[] = {
	
    // current,11.left, 22.right, 33.long_left, 44.long_right, 55.two_s, 66.two_l     function

    {0, 1, 2, 3, 4, 3, 4 ,op_main},
    {1, 0, 0, 0, 0, 0, 0 ,op_settings},
    {2, 0, 0, 0, 0, 0, 0 ,op_power},
    {3, 0, 0, 0, 0, 0, 0 ,test1},
    {4, 0, 0, 0, 0, 0, 0 ,test2},
		
};


//Array out of bounds, returning to the main menu.

void menu_clamp(void)
#define MENU_COUNT  (sizeof(menu_table) / sizeof(menu_table[0]))

{
    if (menu_index >= MENU_COUNT)
        menu_index = 0;
}
