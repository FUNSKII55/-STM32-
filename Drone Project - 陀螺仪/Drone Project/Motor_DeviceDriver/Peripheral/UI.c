#include "UI.h"
#include "u8g2.h"
#include "UI_PID.h"
#include "menu.h"
#include "stdint.h"

extern u8g2_t u8g2;

// ===== UI.c 顶部（全局） =====
static int scroll_px = 0;           // 当前画面上移的像素
static const int VIEW_H = 32;
static const int LINE_H = 13;
static const int TOTAL_H = 32;

// 让选中框完全可见（只对下边界生效：超出多少就上移多少）
static inline void ensure_visible_bottom(int box_y, int box_h)
{
    int bottom = box_y - scroll_px + box_h;   // 当前在屏幕坐标的底边
    if (bottom > VIEW_H) {
        int over = bottom - VIEW_H;          // 例如 34-32=2
        scroll_px += over;                   // 整个画面上移 over 像素

        // 夹紧滚动范围，避免超滚到空白
        int max_scroll = (TOTAL_H > VIEW_H) ? (TOTAL_H - VIEW_H) : 0;
        if (scroll_px < 0) scroll_px = 0;
        if (scroll_px > max_scroll) scroll_px = max_scroll;
    }
}


void Cur_1_Ctrl(void)
{
	uint8_t x = MENU_CURRENT;
	
	if(x == 0)
		{

			aim_y += 11;
			
			cur_y = aim_y-11;

		}

}

// 把超出屏幕的部分“挤压”在顶部显示（也顺便处理下边界）
void draw_squashed_rframe(u8g2_t *u8, int x, int y, int w, int h, int r, int thick)
{
    const int view_top = 0;      // 只处理上边界
    int draw_y = y;
    int draw_h = h;

    // ——上边界挤压——
    if (y < view_top) {
        int over = view_top - y;     // 超出量
        draw_y = view_top;
        draw_h = h - over;           // 压扁
    }

    // 最小高度保护 + 圆角保护
    int min_h = thick * 2 + 1;
    if (draw_h < min_h) draw_h = min_h;
    if (draw_h > h)     draw_h = h;

    int r2 = r, maxr = draw_h / 2;
    if (r2 > maxr) r2 = maxr;

    u8g2_DrawRFrameThick(u8, x, draw_y, w, draw_h, r2, thick);
}

