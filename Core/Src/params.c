#include "params.h"

system_params_t g_params;

void Params_SetDefaults(void)
{
    /* Axis Z */
    g_params.z_max_mm        = 100.0f;
    g_params.z_speed_mm_s   = 5.0f;
    g_params.z_accel_mm_s2  = 20.0f;

    /* Axis X */
    g_params.x_speed_deg_s  = 30.0f;
    g_params.x_accel_deg_s2 = 100.0f;

    /* Lighting */
    g_params.led_r  = 0;
    g_params.led_g  = 0;
    g_params.led_b  = 0;
    g_params.led_uv = 0;
    g_params.led_ir = 0;
}