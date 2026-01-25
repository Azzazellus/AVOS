#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

typedef struct
{
    float z_max_mm;

    float spd_x_max;
    float spd_z_max;
    float axl_x;
    float axl_z;

    uint8_t led_r;
    uint8_t led_g;
    uint8_t led_b;
    uint8_t led_uv;
    uint8_t led_ir;

    uint32_t crc;
} EEPROM_Data_t;

void EEPROM_Init(void);
void EEPROM_Load(EEPROM_Data_t *d);
void EEPROM_Save(const EEPROM_Data_t *d);

#endif