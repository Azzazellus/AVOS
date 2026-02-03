#ifndef __EEPROM_H
#define __EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    typedef struct {
        float z_max_mm;     // Максимальное перемещение Z (мм)
        float spd_z_max;    // Макс. скорость Z (мм/с)
        float spd_x_max;    // Макс. скорость X (град/с)
        float axl_z;        // Ускорение Z (мм/с²)
        float axl_x;        // Ускорение X (град/с²)
        uint8_t led_r;      // Красный светодиод (0-100%)
        uint8_t led_g;      // Зеленый светодиод (0-100%)
        uint8_t led_b;      // Синий светодиод (0-100%)
        uint8_t led_uv;     // УФ светодиод (0-100%)
        uint8_t led_ir;     // ИК светодиод (0-100%)
        uint32_t crc;       // Контрольная сумма
    } EEPROM_Data_t;

    void EEPROM_Init(void);
    void EEPROM_Load(EEPROM_Data_t *d);
    void EEPROM_Save(const EEPROM_Data_t *src);

#ifdef __cplusplus
}
#endif

#endif /* __EEPROM_H */