#include "Flash.h"
#include "stdint.h"
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Not FUNSKII
 * 2023-12-07     Windows       the first version
 */

extern void    FLASH_PageErase(uint32_t PageAddress);
extern HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);
//#define TYPEPROGRAM_CHAR                 0x00U  /*!<Program a char (8-bit) at a specified address.*/
//#define TYPEPROGRAM_HALFWORD             0x01U  /*!<Program a half-word (16-bit) at a specified address.*/
//#define TYPEPROGRAM_WORD                 0x02U  /*!<Program a word (32-bit) at a specified address.*/
//#define TYPEPROGRAM_DOUBLEWORD           0x03U  /*!<Program a double word (64-bit) at a specified address*/
void Flash_Write(uint32_t TypeProgram, uint32_t Address, uint64_t (*Data), uint32_t len)
{

    HAL_FLASH_Unlock();

    FLASH_PageErase(Address);  //擦除这个扇区
    CLEAR_BIT(FLASH->CR, FLASH_CR_PER);                         //清除CR寄存器的PER位，此操作应该在FLASH_PageErase()中完成！
                                                                            //但是HAL库里面并没有做，应该是HAL库bug！
    if (TypeProgram==TYPEPROGRAM_CHAR) {
        for (int var = 0; var < len/2; var++) {
            uint8_t data1=(uint8_t)*Data;
            uint8_t data2=(uint8_t)*(Data+1);
            uint64_t datar = data2<<8|data1;
            HAL_FLASH_Program(TYPEPROGRAM_HALFWORD, Address,datar);
            Address+=2;
            Data+=2;
        }
    } else {
        for (int var = 0; var < len; var++) {
            HAL_FLASH_Program(TypeProgram, Address, *Data);
            Address+=(1<<TypeProgram);
            Data+=1;
        }
    }
    HAL_FLASH_Lock();
}

void Flash_Read(uint32_t TypeProgram, uint32_t Address, uint64_t (*Data), uint32_t len)
{
    int32_t i=0;
    if (TypeProgram==TYPEPROGRAM_CHAR) {
       for (i = 0; i < len; i++, Address+=1, Data++)
       {
           *Data = *(uint8_t *) Address;
       }
    }else if (TypeProgram==TYPEPROGRAM_HALFWORD) {
       for (i = 0; i < len; i++, Address+=2, Data++)
       {
           *Data = *(uint16_t *) Address;
       }
    }else if (TypeProgram==TYPEPROGRAM_WORD) {
      for (i = 0; i < len; i++, Address+=4, Data++)
      {
          *Data = *(uint32_t *) Address;
      }
    }else if (TypeProgram==TYPEPROGRAM_DOUBLEWORD) {
      for (i = 0; i < len; i++, Address+=8, Data++)
      {
          *Data = *(uint64_t *) Address;
      }
    }
}

