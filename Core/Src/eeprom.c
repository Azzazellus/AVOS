#include "eeprom.h"
#include "stm32f4xx_hal.h"
#include <string.h>

#define EEPROM_ADDR  0x08020000U
#define EEPROM_SECTOR FLASH_SECTOR_5

static uint32_t crc32(const EEPROM_Data_t *d)
{
    const uint8_t *bytes = (const uint8_t *)d;
    uint32_t crc = 0;

    // Вычисляем CRC для всех байтов, кроме последних 4 (самого поля crc)
    for (size_t i = 0; i < sizeof(EEPROM_Data_t) - sizeof(uint32_t); i++) {
        crc ^= (bytes[i] << 24);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ 0x04C11DB7;
            else
                crc <<= 1;
        }
    }

    return crc;
}

void EEPROM_Init(void){}

void EEPROM_Load(EEPROM_Data_t *d)
{
    memcpy(d, (void*)EEPROM_ADDR, sizeof(*d));

    // Проверяем CRC
    uint32_t expected_crc = crc32(d);
    if (d->crc != expected_crc)
    {
        // Данные повреждены, устанавливаем значения по умолчанию
        memset(d, 0, sizeof(*d));
        d->z_max_mm = 100.0f;
        d->spd_x_max = 90.0f;
        d->spd_z_max = 10.0f;
        d->axl_x = 180.0f;
        d->axl_z = 20.0f;
        d->crc = crc32(d);
    }
}

void EEPROM_Save(const EEPROM_Data_t *src)
{
    EEPROM_Data_t tmp = *src;
    tmp.crc = crc32(&tmp);

    HAL_FLASH_Unlock();

    // Стираем сектор
    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Sector = EEPROM_SECTOR,
        .NbSectors = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };

    uint32_t sector_error = 0;
    HAL_StatusTypeDef erase_status = HAL_FLASHEx_Erase(&erase, &sector_error);

    if (erase_status != HAL_OK) {
        HAL_FLASH_Lock();
        return; // Ошибка стирания
    }

    // Записываем данные
    uint32_t addr = EEPROM_ADDR;
    const uint64_t *data_ptr = (const uint64_t *)&tmp;
    uint32_t doubleword_count = sizeof(tmp) / 8;

    // Записываем целыми двойными словами (8 байт)
    for (uint32_t i = 0; i < doubleword_count; i++) {
        HAL_StatusTypeDef write_status = HAL_FLASH_Program(
            FLASH_TYPEPROGRAM_DOUBLEWORD,
            addr,
            data_ptr[i]
        );

        if (write_status != HAL_OK) {
            HAL_FLASH_Lock();
            return; // Ошибка записи
        }

        addr += 8;
    }

    // Если есть остаток (меньше 8 байт), дописываем последнее двойное слово
    uint32_t remainder = sizeof(tmp) % 8;
    if (remainder > 0) {
        uint64_t last_word = 0;
        memcpy(&last_word, (uint8_t*)&tmp + doubleword_count * 8, remainder);

        HAL_StatusTypeDef write_status = HAL_FLASH_Program(
            FLASH_TYPEPROGRAM_DOUBLEWORD,
            addr,
            last_word
        );

        if (write_status != HAL_OK) {
            HAL_FLASH_Lock();
            return; // Ошибка записи
        }
    }

    HAL_FLASH_Lock();

    // Опционально: проверяем запись
    EEPROM_Data_t verify;
    EEPROM_Load(&verify);

    if (memcmp(&verify, &tmp, sizeof(EEPROM_Data_t)) != 0) {
        // Запись не прошла проверку
        // Можно добавить обработку ошибки
    }
}