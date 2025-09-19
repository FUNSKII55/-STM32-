#ifndef __UI_PID_H__
#define __UI_PID_H__

#include "stdint.h"

extern float Kp_y;
extern float Ki_y;
extern float Kd_y;
extern float Kp_w;
extern float Ki_w;
extern float Kd_w;
extern int aim_y;
extern int cur_y;
extern float vel_y;
extern int aim_w;   // 动态更新：由字符串宽度计算
extern float cur_w;	// 7char = 48  5char = 37
extern float vel_w;


void UI_Move(void);
float PID_Update(float aim, float cur, float *integral, float *last_err,float Kp, float Ki, float Kd);

#endif
