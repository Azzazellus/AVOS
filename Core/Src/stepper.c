#include "stepper.h"
#include "main.h"
#include <math.h>

/* ===== MECHANICS ===== */
#define Z_STEPS_PER_MM    80.0f
#define X_STEPS_PER_DEG  (1600.0f / 360.0f)

/* ===== TIMING ===== */
#define TIM3_FREQ_HZ 10000.0f
#define MAX_DIVIDER  10000U

/* ===== STATE ===== */
static volatile int32_t cur_z_steps = 0;
static volatile int32_t cur_x_steps = 0;
static volatile int32_t tgt_z_steps = 0;
static volatile int32_t tgt_x_steps = 0;

static volatile bool z_busy = false;
static volatile bool x_busy = false;
static volatile bool homing_z = false;

/* ===== MOTION PARAMS ===== */
static float z_speed_max = 5.0f;
static float x_speed_max = 10.0f;

static float z_accel = 20.0f;
static float x_accel = 40.0f;

/* ===== RUNTIME ===== */
static float z_speed = 0.0f;
static float x_speed = 0.0f;

/* ===== TIM ===== */
static volatile uint16_t z_div = MAX_DIVIDER;
static volatile uint16_t x_div = MAX_DIVIDER;
static volatile uint16_t z_tick = 0;
static volatile uint16_t x_tick = 0;

/* ===== HELPERS ===== */
static uint16_t SpeedToDivider(float steps_per_sec)
{
    if (steps_per_sec <= 1.0f) return MAX_DIVIDER;

    float d = TIM3_FREQ_HZ / steps_per_sec;
    if (d < 1.0f) d = 1.0f;
    if (d > MAX_DIVIDER) d = MAX_DIVIDER;

    return (uint16_t)d;
}

/* ===== INIT ===== */
void Stepper_Init(void)
{
    z_busy = false;
    x_busy = false;
    homing_z = false;

    z_speed = 0.0f;
    x_speed = 0.0f;

    z_div = MAX_DIVIDER;
    x_div = MAX_DIVIDER;
}

/* ===== STATUS ===== */
bool Stepper_IsBusyZ(void)
{
    return z_busy;
}

bool Stepper_IsBusyX(void)
{
    return x_busy;
}

bool Stepper_IsHoming(void)
{
    return homing_z;
}

/* ===== SPEED ===== */
void Stepper_SetSpeedZ_mm_s(float v) { z_speed_max = v; }
void Stepper_SetSpeedX_deg_s(float v){ x_speed_max = v; }

float Stepper_GetSpeedZ_mm_s(void) { return z_speed_max; }
float Stepper_GetSpeedX_deg_s(void){ return x_speed_max; }

/* ===== ACCEL ===== */
void Stepper_SetAccelZ_mm_s2(float a){ z_accel = a; }
void Stepper_SetAccelX_deg_s2(float a){ x_accel = a; }

float Stepper_GetAccelZ_mm_s2(void){ return z_accel; }
float Stepper_GetAccelX_deg_s2(void){ return x_accel; }

/* ===== TARGETS ===== */
void Stepper_SetTargetZ_mm(float z_mm)
{
    tgt_z_steps = (int32_t)roundf(z_mm * Z_STEPS_PER_MM);
    z_busy = (tgt_z_steps != cur_z_steps);
}

void Stepper_SetTargetX_deg(float x_deg)
{
    tgt_x_steps = (int32_t)roundf(x_deg * X_STEPS_PER_DEG);
    x_busy = (tgt_x_steps != cur_x_steps);
}

/* ===== POSITION ===== */
float Stepper_GetPosZ_mm(void)
{
    return (float)cur_z_steps / Z_STEPS_PER_MM;
}

float Stepper_GetPosX_deg(void)
{
    return (float)cur_x_steps / X_STEPS_PER_DEG;
}

/* ===== HOME ===== */
void Stepper_StartHomeZ(void)
{
    homing_z = true;
    z_busy = true;
}

/* ===== EMERGENCY ===== */
void Stepper_EmergencyStop(void)
{
    z_busy = false;
    x_busy = false;
    homing_z = false;

    z_speed = 0.0f;
    x_speed = 0.0f;

    z_div = MAX_DIVIDER;
    x_div = MAX_DIVIDER;
}

/* ===== TASK ===== */
void Stepper_Task(void)
{
    if (z_busy)
    {
        z_speed += z_accel / TIM3_FREQ_HZ;
        if (z_speed > z_speed_max) z_speed = z_speed_max;
        z_div = SpeedToDivider(z_speed * Z_STEPS_PER_MM);
    }

    if (x_busy)
    {
        x_speed += x_accel / TIM3_FREQ_HZ;
        if (x_speed > x_speed_max) x_speed = x_speed_max;
        x_div = SpeedToDivider(x_speed * X_STEPS_PER_DEG);
    }
}

/* ===== ISR ===== */
void Stepper_TIM_ISR(void)
{
    if (z_busy && (++z_tick >= z_div))
    {
        z_tick = 0;
        cur_z_steps += (tgt_z_steps > cur_z_steps) ? 1 : -1;
        if (cur_z_steps == tgt_z_steps)
        {
            z_busy = false;
            z_speed = 0.0f;
        }
    }

    if (x_busy && (++x_tick >= x_div))
    {
        x_tick = 0;
        cur_x_steps += (tgt_x_steps > cur_x_steps) ? 1 : -1;
        if (cur_x_steps == tgt_x_steps)
        {
            x_busy = false;
            x_speed = 0.0f;
        }
    }
}