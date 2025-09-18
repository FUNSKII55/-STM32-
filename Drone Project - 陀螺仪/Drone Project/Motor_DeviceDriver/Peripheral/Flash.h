#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"
#include "stm32f1xx_hal_flash.h"

#define TYPEPROGRAM_CHAR                 0x00U  /*!<Program a char (8-bit) at a specified address.*/
#define TYPEPROGRAM_HALFWORD             0x01U  /*!<Program a half-word (16-bit) at a specified address.*/
#define TYPEPROGRAM_WORD                 0x02U  /*!<Program a word (32-bit) at a specified address.*/
#define TYPEPROGRAM_DOUBLEWORD           0x03U  /*!<Program a double word (64-bit) at a specified address*/
void Flash_Write(uint32_t TypeProgram, uint32_t Address, uint64_t (*Data), uint32_t len);
void Flash_Read(uint32_t TypeProgram, uint32_t Address, uint64_t (*Data), uint32_t len);

#endif
