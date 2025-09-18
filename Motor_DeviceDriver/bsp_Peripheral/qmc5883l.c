/*
Author : Sercan DEMIRHAN + ChatGPT适配校准存储
Created : 21.11.2018, Modified: 2024.08
*/

#include "qmc5883l.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//========================= 校准结构体与全局变量 =========================

QMC5883L_Calib_t g_qmc5883l_calib; // <<<<< 仅此一行与原来不同
volatile bool mag_calibrating = false;
volatile uint32_t mag_calib_tick = 0;
#define QMC5883L_FLASH_ADDR ((uint32_t)0x0800FC00) // 128K最后一页(实际根据MCU容量调整)

uint8_t QMC5883L_Read_Reg(uint8_t reg)
{
	uint8_t Buffer[1];
	HAL_I2C_Mem_Read(QMC5883L_I2C_PORT, QMC5883L_ADDRESS, reg, 1, Buffer, 1, 10);
	return Buffer[0];
}

void QMC5883L_Write_Reg(uint8_t reg, uint8_t data)
{
	uint8_t Buffer[2] = {reg, data};
	HAL_I2C_Master_Transmit(QMC5883L_I2C_PORT, QMC5883L_ADDRESS, Buffer, 2, 10);
}

void QMC5883L_Read_Data(int16_t *MagX, int16_t *MagY, int16_t *MagZ)
{
	*MagX = ((int16_t)QMC5883L_Read_Reg(QMC5883L_DATA_READ_X_LSB) | ((int16_t)QMC5883L_Read_Reg(QMC5883L_DATA_READ_X_MSB) << 8));
	*MagY = ((int16_t)QMC5883L_Read_Reg(QMC5883L_DATA_READ_Y_LSB) | ((int16_t)QMC5883L_Read_Reg(QMC5883L_DATA_READ_Y_MSB) << 8));
	*MagZ = ((int16_t)QMC5883L_Read_Reg(QMC5883L_DATA_READ_Z_LSB) | ((int16_t)QMC5883L_Read_Reg(QMC5883L_DATA_READ_Z_MSB) << 8));
}

int16_t QMC5883L_Read_Temperature()
{
	return (((int16_t)QMC5883L_Read_Reg(QMC5883L_TEMP_READ_LSB)) | (((int16_t)QMC5883L_Read_Reg(QMC5883L_TEMP_READ_MSB)) << 8)) / 100;
}

void QMC5883L_Initialize(_qmc5883l_MODE MODE, _qmc5883l_ODR ODR, _qmc5883l_RNG RNG, _qmc5883l_OSR OSR)
{
	QMC5883L_Write_Reg(QMC5883L_CONFIG_3, 0x01);
	QMC5883L_Write_Reg(QMC5883L_CONFIG_1, MODE | ODR | RNG | OSR);
}

void QMC5883L_Reset()
{
	QMC5883L_Write_Reg(QMC5883L_CONFIG_2, 0x81);
}

void QMC5883L_InterruptConfig(_qmc5883l_INT INT)
{
	if (INT == INTERRUPT_ENABLE) { QMC5883L_Write_Reg(QMC5883L_CONFIG_2, 0x00); }
	else { QMC5883L_Write_Reg(QMC5883L_CONFIG_2, 0x01); }
}

_qmc5883l_status QMC5883L_DataIsReady()
{
	uint8_t Buffer = QMC5883L_Read_Reg(QMC5883L_STATUS);
	if ((Buffer & 0x00) == 0x00) { return NO_NEW_DATA; }
	else if ((Buffer & 0x01) == 0X01) { return NEW_DATA_IS_READY; }
	return NORMAL;
}

_qmc5883l_status QMC5883L_DataIsSkipped()
{
	uint8_t Buffer = QMC5883L_Read_Reg(QMC5883L_STATUS);
	if ((Buffer & 0x00) == 0X00) { return NORMAL; }
	else if ((Buffer & 0x04) == 0X04) { return DATA_SKIPPED_FOR_READING; }
	return NORMAL;
}

_qmc5883l_status QMC5883L_DataIsOverflow()
{
	uint8_t Buffer = QMC5883L_Read_Reg(QMC5883L_STATUS);
	if ((Buffer & 0x00) == 0X00) { return NORMAL; }
	else if ((Buffer & 0x02) == 0X02) { return DATA_OVERFLOW; }
	return NORMAL;
}

//========================= 校准&Flash部分 =========================

// 启动自动校准
void QMC5883L_StartCalibration(void)
{
	g_qmc5883l_calib.Xmin = 32767; g_qmc5883l_calib.Xmax = -32768;
	g_qmc5883l_calib.Ymin = 32767; g_qmc5883l_calib.Ymax = -32768;
	g_qmc5883l_calib.Zmin = 32767; g_qmc5883l_calib.Zmax = -32768;
	g_qmc5883l_calib.isValid = 0;
	mag_calibrating = true;
	mag_calib_tick = HAL_GetTick();
}

// Flash存储
void QMC5883L_SaveCalibToFlash(void)
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct = {
		.TypeErase = FLASH_TYPEERASE_PAGES,
		.PageAddress = QMC5883L_FLASH_ADDR,
		.NbPages = 1
	};
	uint32_t PageError = 0;
	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

	uint32_t *pdata = (uint32_t*)&g_qmc5883l_calib;
	for (uint32_t i = 0; i < sizeof(QMC5883L_Calib_t) / 4; i++)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, QMC5883L_FLASH_ADDR + 4 * i, pdata[i]);

	HAL_FLASH_Lock();
}

void QMC5883L_LoadCalibFromFlash(void)
{
    const QMC5883L_Calib_t *p = (const QMC5883L_Calib_t*)QMC5883L_FLASH_ADDR;
    if (p->isValid == 0xA5)
        memcpy(&g_qmc5883l_calib, p, sizeof(QMC5883L_Calib_t));
    else
        memset(&g_qmc5883l_calib, 0, sizeof(QMC5883L_Calib_t));
}

// 归一化输出
void QMC5883L_Normalize(int16_t *mx, int16_t *my, int16_t *mz, float *X_cal, float *Y_cal, float *Z_cal)
{
	*X_cal = 2.0f * (*mx - g_qmc5883l_calib.Xmin) / (g_qmc5883l_calib.Xmax - g_qmc5883l_calib.Xmin) - 1.0f;
	*Y_cal = 2.0f * (*my - g_qmc5883l_calib.Ymin) / (g_qmc5883l_calib.Ymax - g_qmc5883l_calib.Ymin) - 1.0f;
	*Z_cal = 2.0f * (*mz - g_qmc5883l_calib.Zmin) / (g_qmc5883l_calib.Zmax - g_qmc5883l_calib.Zmin) - 1.0f;
}

//========================= 校准相关老接口 =========================

// 已弃用，不建议再用
void QMC5883L_ResetCalibration() { g_qmc5883l_calib.Xmin = g_qmc5883l_calib.Xmax = g_qmc5883l_calib.Ymin = g_qmc5883l_calib.Ymax = 0; }

// 航向角（建议只用作演示，实际请先归一化再用互补滤波）
float QMC5883L_Heading(int16_t Xraw, int16_t Yraw, int16_t Zraw)
{
	float X = Xraw, Y = Yraw, Z = Zraw;
	float Heading;

	// 使用校准值
	if (X < g_qmc5883l_calib.Xmin) { g_qmc5883l_calib.Xmin = X; }
	else if (X > g_qmc5883l_calib.Xmax) { g_qmc5883l_calib.Xmax = X; }

	if (Y < g_qmc5883l_calib.Ymin) { g_qmc5883l_calib.Ymin = Y; }
	else if (Y > g_qmc5883l_calib.Ymax) { g_qmc5883l_calib.Ymax = Y; }

	if (g_qmc5883l_calib.Xmin == g_qmc5883l_calib.Xmax || g_qmc5883l_calib.Ymin == g_qmc5883l_calib.Ymax) { return 0.0; }

	X -= (g_qmc5883l_calib.Xmax + g_qmc5883l_calib.Xmin) / 2;
	Y -= (g_qmc5883l_calib.Ymax + g_qmc5883l_calib.Ymin) / 2;

	X = X / (g_qmc5883l_calib.Xmax - g_qmc5883l_calib.Xmin);
	Y = Y / (g_qmc5883l_calib.Ymax - g_qmc5883l_calib.Ymin);

	Heading = atan2(Y, X);
	Heading += QMC5883L_DECLINATION_ANGLE;
	if (Heading < 0) { Heading += 2 * M_PI; }
	else if (Heading > 2 * M_PI) { Heading -= 2 * M_PI; }

	return Heading;
}

void QMC5883L_Scale(int16_t *X, int16_t *Y, int16_t *Z)
{
	*X *= QMC5883L_SCALE_FACTOR;
	*Y *= QMC5883L_SCALE_FACTOR;
	*Z *= QMC5883L_SCALE_FACTOR;
}
