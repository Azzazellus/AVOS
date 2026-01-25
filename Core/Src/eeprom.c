#include "eeprom.h"
#include "stm32f4xx_hal.h"
#include <string.h>

#define EEPROM_ADDR  0x08020000U
#define EEPROM_SECTOR FLASH_SECTOR_5

static uint32_t crc32(const EEPROM_Data_t *d)
{
    const uint32_t *p = (const uint32_t *)d;
    uint32_t c = 0;
    for (uint32_t i = 0; i < (sizeof(EEPROM_Data_t)/4)-1; i++)
        c ^= p[i];
    return c;
}

void EEPROM_Init(void){}

void EEPROM_Load(EEPROM_Data_t *d)
{
    memcpy(d, (void*)EEPROM_ADDR, sizeof(*d));
    if (d->crc != crc32(d))
    {
        memset(d, 0, sizeof(*d));
        d->z_max_mm = 100.0f;
        d->spd_x_max = 90.0f;
        d->spd_z_max = 10.0f;
        d->axl_x = 180.0f;
        d->axl_z = 20.0f;
    }
}

void EEPROM_Save(const EEPROM_Data_t *src)
{
    EEPROM_Data_t tmp = *src;
    tmp.crc = crc32(&tmp);

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef e = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Sector = EEPROM_SECTOR,
        .NbSectors = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };
    uint32_t err;
    HAL_FLASHEx_Erase(&e, &err);

    uint32_t addr = EEPROM_ADDR;
    const uint64_t *p = (const uint64_t *)&tmp;
    for (uint32_t i = 0; i < sizeof(tmp)/8; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, p[i]);
        addr += 8;
    }

    HAL_FLASH_Lock();
}