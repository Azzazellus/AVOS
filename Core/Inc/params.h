#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>

typedef struct
{
    /* ===== Axis Z ===== */
    float z_max_mm;
    float z_speed_mm_s;
    float z_accel_mm_s2;

    /* ===== Axis X ===== */
    float x_speed_deg_s;
    float x_accel_deg_s2;

    /* ===== Lighting ===== */
    uint8_t led_r;    // 0–100 %
    uint8_t led_g;
    uint8_t led_b;
    uint8_t led_uv;
    uint8_t led_ir;

} system_params_t;

extern system_params_t g_params;

/* Значения по умолчанию */
void Params_SetDefaults(void);

#endif /* PARAMS_H */