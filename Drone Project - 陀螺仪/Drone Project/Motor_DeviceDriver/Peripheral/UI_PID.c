#include "u8g2.h"

extern u8g2_t u8g2;

// ------------------- PID参数组 -------------------
// Y轴 (位置)
float Kp_y = 0.40f;
float Ki_y = 0.005f;
float Kd_y = 0.08f;

// W轴 (宽度，smooth)
float Kp_w = 0.01f;
float Ki_w = 0.00003f;
float Kd_w = 0.05f;

// ------------------- 状态变量 -------------------
int aim_y = 0;
int cur_y = 0;
float vel_y = 0;

int aim_w = 49;   // 动态更新：由字符串宽度计算
float cur_w = 49;	// 7char = 48  5char = 37
float vel_w = 0;

float integral_y = 0, last_err_y = 0;
float integral_w = 0, last_err_w = 0;

// ------------------- PID 更新函数 -------------------
float PID_Update(float aim, float cur, float *integral, float *last_err,
                 float Kp, float Ki, float Kd)
{
    float err = aim - cur;
    *integral += err;
    float der = err - *last_err;
    *last_err = err;
    return Kp * err + Ki * (*integral) + Kd * der;
}

// ------------------- UI 更新 -------------------
void UI_Move(void)
{
    // Y 方向 PID
    float out_y = PID_Update(aim_y, cur_y, &integral_y, &last_err_y,
                             Kp_y, Ki_y, Kd_y);
    vel_y += out_y;
    vel_y *= 0.9f;
    cur_y += vel_y;

    // 宽度 PID
    float out_w = PID_Update(aim_w, cur_w, &integral_w, &last_err_w,
                             Kp_w, Ki_w, Kd_w);
    vel_w += out_w;
    vel_w *= 0.9f;
    cur_w += vel_w;

    // 限制
//    if(cur_y < 0) cur_y = 0;
    if(cur_y > 108) cur_y = 108;
    if(cur_w < 20) cur_w = 20;   // 最小宽度保护
    if(cur_w > 120) cur_w = 120; // 最大宽度保护

    // 绘制
    draw_squashed_rframe(&u8g2, 0, (int)cur_y, (int)cur_w, 13, 3, 2);
}
