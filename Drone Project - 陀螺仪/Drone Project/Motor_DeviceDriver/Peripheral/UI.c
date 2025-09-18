#include "UI.h"
#include "u8g2.h"
#include "UI_PID.h"
#include "menu.h"
#include "stdint.h"

extern u8g2_t u8g2;

// ===== UI.c ������ȫ�֣� =====
static int scroll_px = 0;           // ��ǰ�������Ƶ�����
static const int VIEW_H = 32;
static const int LINE_H = 13;
static const int TOTAL_H = 32;

// ��ѡ�п���ȫ�ɼ���ֻ���±߽���Ч���������پ����ƶ��٣�
static inline void ensure_visible_bottom(int box_y, int box_h)
{
    int bottom = box_y - scroll_px + box_h;   // ��ǰ����Ļ����ĵױ�
    if (bottom > VIEW_H) {
        int over = bottom - VIEW_H;          // ���� 34-32=2
        scroll_px += over;                   // ������������ over ����

        // �н�������Χ�����ⳬ�����հ�
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

// �ѳ�����Ļ�Ĳ��֡���ѹ���ڶ�����ʾ��Ҳ˳�㴦���±߽磩
void draw_squashed_rframe(u8g2_t *u8, int x, int y, int w, int h, int r, int thick)
{
    const int view_top = 0;      // ֻ�����ϱ߽�
    int draw_y = y;
    int draw_h = h;

    // �����ϱ߽缷ѹ����
    if (y < view_top) {
        int over = view_top - y;     // ������
        draw_y = view_top;
        draw_h = h - over;           // ѹ��
    }

    // ��С�߶ȱ��� + Բ�Ǳ���
    int min_h = thick * 2 + 1;
    if (draw_h < min_h) draw_h = min_h;
    if (draw_h > h)     draw_h = h;

    int r2 = r, maxr = draw_h / 2;
    if (r2 > maxr) r2 = maxr;

    u8g2_DrawRFrameThick(u8, x, draw_y, w, draw_h, r2, thick);
}

