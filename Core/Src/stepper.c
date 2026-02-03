#include "stepper.h"
#include "main.h"
#include "pins.h"
#include <math.h>
#include <stdlib.h>
#include "tim.h"

/* ===== MECHANICS ===== */
#define Z_STEPS_PER_MM    80.0f
#define X_STEPS_PER_REV   1600          // Шагов на полный оборот
#define X_STEPS_PER_DEG   (X_STEPS_PER_REV / 360.0f)  // ~4.444 шагов/градус
#define Z_MAX_LIMIT_MM   1300.0f

/* ===== TIMING ===== */
#define STEPPER_UPDATE_FREQ_HZ 1000.0f

/* ===== STATE ===== */
volatile int32_t cur_z_steps = 0;
volatile int32_t cur_x_steps = 0;
volatile int32_t tgt_z_steps = 0;
volatile int32_t tgt_x_steps = 0;

volatile bool z_busy = false;
volatile bool x_busy = false;
volatile bool homing_z = false;

/* ===== MOTION CONTROL ===== */
static float z_speed_max = 5.0f;
static float x_speed_max = 10.0f;

static float z_accel = 20.0f;
static float x_accel = 40.0f;

static float z_current_speed = 0.0f;
static float x_current_speed = 0.0f;

static uint32_t z_pulse_counter = 0;
static uint32_t x_pulse_counter = 0;
static uint32_t z_pulse_interval = 0;
static uint32_t x_pulse_interval = 0;

/* ===== CRITICAL SECTION HELPERS ===== */
static void Stepper_EnterCritical(void) {
    __disable_irq();
}

static void Stepper_ExitCritical(void) {
    __enable_irq();
}

/* ===== HELPERS ===== */
static uint32_t SpeedToPulseInterval(float speed_mm_s, float steps_per_unit)
{
    if (speed_mm_s <= 0.01f) return 0xFFFFFFFF;

    float steps_per_sec = speed_mm_s * steps_per_unit;
    if (steps_per_sec <= 0.01f) return 0xFFFFFFFF;

    float interval = STEPPER_UPDATE_FREQ_HZ / steps_per_sec;
    if (interval < 1.0f) interval = 1.0f;
    if (interval > STEPPER_UPDATE_FREQ_HZ) interval = STEPPER_UPDATE_FREQ_HZ;

    return (uint32_t)interval;
}

/* ===== DIRECTION CONTROL ===== */
void Stepper_SetDirZ(uint8_t dir)
{
    HAL_GPIO_WritePin(DIR_Z_PORT, DIR_Z_PIN, dir ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Stepper_SetDirX(uint8_t dir)
{
    HAL_GPIO_WritePin(DIR_X_PORT, DIR_X_PIN, dir ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* ===== INIT ===== */
void Stepper_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = DIR_Z_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DIR_Z_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DIR_X_PIN;
    HAL_GPIO_Init(DIR_X_PORT, &GPIO_InitStruct);

    Stepper_SetDirZ(0);
    Stepper_SetDirX(0);

    Stepper_EnterCritical();
    z_busy = false;
    x_busy = false;
    homing_z = false;
    z_current_speed = 0.0f;
    x_current_speed = 0.0f;
    z_pulse_counter = 0;
    x_pulse_counter = 0;
    z_pulse_interval = 0xFFFFFFFF;
    x_pulse_interval = 0xFFFFFFFF;
    cur_z_steps = 0;
    cur_x_steps = 0;
    tgt_z_steps = 0;
    tgt_x_steps = 0;
    Stepper_ExitCritical();

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
}

/* ===== STATUS ===== */
bool Stepper_IsBusyZ(void) {
    Stepper_EnterCritical();
    bool busy = z_busy;
    Stepper_ExitCritical();
    return busy;
}

bool Stepper_IsBusyX(void) {
    Stepper_EnterCritical();
    bool busy = x_busy;
    Stepper_ExitCritical();
    return busy;
}

bool Stepper_IsHoming(void) {
    Stepper_EnterCritical();
    bool homing = homing_z;
    Stepper_ExitCritical();
    return homing;
}

/* ===== SPEED ===== */
void Stepper_SetSpeedZ_mm_s(float v) {
    Stepper_EnterCritical();
    z_speed_max = v;
    Stepper_ExitCritical();
}

void Stepper_SetSpeedX_deg_s(float v) {
    Stepper_EnterCritical();
    x_speed_max = v;
    Stepper_ExitCritical();
}

float Stepper_GetSpeedZ_mm_s(void) {
    Stepper_EnterCritical();
    float speed = z_speed_max;
    Stepper_ExitCritical();
    return speed;
}

float Stepper_GetSpeedX_deg_s(void) {
    Stepper_EnterCritical();
    float speed = x_speed_max;
    Stepper_ExitCritical();
    return speed;
}

/* ===== ACCEL ===== */
void Stepper_SetAccelZ_mm_s2(float a) {
    Stepper_EnterCritical();
    z_accel = a;
    Stepper_ExitCritical();
}

void Stepper_SetAccelX_deg_s2(float a) {
    Stepper_EnterCritical();
    x_accel = a;
    Stepper_ExitCritical();
}

float Stepper_GetAccelZ_mm_s2(void) {
    Stepper_EnterCritical();
    float accel = z_accel;
    Stepper_ExitCritical();
    return accel;
}

float Stepper_GetAccelX_deg_s2(void) {
    Stepper_EnterCritical();
    float accel = x_accel;
    Stepper_ExitCritical();
    return accel;
}

/* ===== POSITION SET ===== */
void Stepper_SetPositionZ_mm(float z_mm)
{
    Stepper_EnterCritical();
    cur_z_steps = (int32_t)roundf(z_mm * Z_STEPS_PER_MM);
    tgt_z_steps = cur_z_steps;
    Stepper_ExitCritical();
}

void Stepper_SetPositionX_deg(float x_deg)
{
    while (x_deg >= 360.0f) x_deg -= 360.0f;
    while (x_deg < 0.0f) x_deg += 360.0f;

    Stepper_EnterCritical();
    cur_x_steps = (int32_t)roundf(x_deg * X_STEPS_PER_DEG);
    tgt_x_steps = cur_x_steps;
    Stepper_ExitCritical();
}

/* ===== TARGETS WITH LIMIT CHECK ===== */
void Stepper_SetTargetZ_mm(float z_mm)
{
    if (z_mm < 0.0f) z_mm = 0.0f;
    if (z_mm > Z_MAX_LIMIT_MM) z_mm = Z_MAX_LIMIT_MM;

    int32_t new_target = (int32_t)roundf(z_mm * Z_STEPS_PER_MM);

    Stepper_EnterCritical();

    if (new_target != tgt_z_steps)
    {
        tgt_z_steps = new_target;
        z_busy = (tgt_z_steps != cur_z_steps);

        if (z_busy)
        {
            if (tgt_z_steps > cur_z_steps)
                Stepper_SetDirZ(1);
            else
                Stepper_SetDirZ(0);
        }
        else
        {
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
            z_current_speed = 0.0f;
            z_pulse_interval = 0xFFFFFFFF;
        }
    }

    Stepper_ExitCritical();
}

void Stepper_SetTargetX_deg(float x_deg)
{
    // Нормализуем угол 0-360
    while (x_deg >= 360.0f) x_deg -= 360.0f;
    while (x_deg < 0.0f) x_deg += 360.0f;

    int32_t new_target = (int32_t)roundf(x_deg * X_STEPS_PER_DEG);

    Stepper_EnterCritical();

    // Вычисляем кратчайший путь для циклической оси
    int32_t diff = new_target - (cur_x_steps % X_STEPS_PER_REV);
    if (diff < 0) diff += X_STEPS_PER_REV;

    if (diff > X_STEPS_PER_REV/2) {
        diff -= X_STEPS_PER_REV;
    }

    tgt_x_steps = cur_x_steps + diff;
    x_busy = (diff != 0);

    if (x_busy)
    {
        if (diff > 0)
            Stepper_SetDirX(1);
        else
            Stepper_SetDirX(0);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
        x_current_speed = 0.0f;
        x_pulse_interval = 0xFFFFFFFF;
    }

    Stepper_ExitCritical();
}

/* ===== POSITION ===== */
float Stepper_GetPosZ_mm(void) {
    Stepper_EnterCritical();
    int32_t steps = cur_z_steps;
    Stepper_ExitCritical();
    return (float)steps / Z_STEPS_PER_MM;
}

float Stepper_GetPosX_deg(void) {
    Stepper_EnterCritical();
    int32_t steps = cur_x_steps;
    Stepper_ExitCritical();

    float deg = (float)steps / X_STEPS_PER_DEG;
    while (deg >= 360.0f) deg -= 360.0f;
    while (deg < 0.0f) deg += 360.0f;

    return deg;
}

/* ===== STEP COUNTERS ===== */
int32_t Stepper_GetStepsZ(void) {
    Stepper_EnterCritical();
    int32_t steps = cur_z_steps;
    Stepper_ExitCritical();
    return steps;
}

int32_t Stepper_GetStepsX(void) {
    Stepper_EnterCritical();
    int32_t steps = cur_x_steps;
    Stepper_ExitCritical();
    return steps;
}

/* ===== HOME ===== */
void Stepper_StartHoming(void)
{
    Stepper_EnterCritical();
    homing_z = true;
    z_busy = true;
    Stepper_SetDirZ(0);
    z_speed_max = 1.0f;
    z_current_speed = 1.0f;
    z_pulse_interval = SpeedToPulseInterval(z_current_speed, Z_STEPS_PER_MM);
    Stepper_ExitCritical();
}

/* ===== EMERGENCY ===== */
void Stepper_EmergencyStop(void)
{
    Stepper_EnterCritical();

    z_busy = false;
    x_busy = false;
    homing_z = false;

    z_current_speed = 0.0f;
    x_current_speed = 0.0f;

    z_pulse_interval = 0xFFFFFFFF;
    x_pulse_interval = 0xFFFFFFFF;

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);

    Stepper_ExitCritical();
}

/* ===== TASK ===== */
void Stepper_Task(void)
{
    Stepper_EnterCritical();

    if (z_busy)
    {
        if (z_current_speed < z_speed_max)
        {
            z_current_speed += z_accel / STEPPER_UPDATE_FREQ_HZ;
            if (z_current_speed > z_speed_max)
                z_current_speed = z_speed_max;

            z_pulse_interval = SpeedToPulseInterval(z_current_speed, Z_STEPS_PER_MM);
        }
    }

    if (x_busy)
    {
        if (x_current_speed < x_speed_max)
        {
            x_current_speed += x_accel / STEPPER_UPDATE_FREQ_HZ;
            if (x_current_speed > x_speed_max)
                x_current_speed = x_speed_max;

            x_pulse_interval = SpeedToPulseInterval(x_current_speed, X_STEPS_PER_DEG);
        }
    }

    Stepper_ExitCritical();
}

/* ===== ISR ===== */
void Stepper_TIM_ISR(void)
{
    // Ось Z - проверка концевика в первую очередь
    if (homing_z && HAL_GPIO_ReadPin(Z_LIMIT_PORT, Z_LIMIT_PIN) == GPIO_PIN_RESET)
    {
        // Обнуляем позицию и останавливаем движение
        cur_z_steps = 0;
        tgt_z_steps = 0;
        z_busy = false;
        homing_z = false;
        z_current_speed = 0.0f;
        z_pulse_interval = 0xFFFFFFFF;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
        z_speed_max = 5.0f;
        return;
    }

    // Генерация импульсов для оси Z
    if (z_busy && z_pulse_interval != 0xFFFFFFFF)
    {
        if (++z_pulse_counter >= z_pulse_interval)
        {
            z_pulse_counter = 0;

            // Генерируем импульс (100 тактов = ~1.19 мкс при 84 МГц)
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 100);

            if (HAL_GPIO_ReadPin(DIR_Z_PORT, DIR_Z_PIN) == GPIO_PIN_SET)
            {
                cur_z_steps++;
            }
            else
            {
                cur_z_steps--;
            }

            // Проверяем достижение цели
            if (cur_z_steps == tgt_z_steps)
            {
                z_busy = false;
                z_current_speed = 0.0f;
                z_pulse_interval = 0xFFFFFFFF;
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
            }
        }
        else if (z_pulse_counter == 1)
        {
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
        }
    }

    // Генерация импульсов для оси X
    if (x_busy && x_pulse_interval != 0xFFFFFFFF)
    {
        if (++x_pulse_counter >= x_pulse_interval)
        {
            x_pulse_counter = 0;

            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 100);

            if (HAL_GPIO_ReadPin(DIR_X_PORT, DIR_X_PIN) == GPIO_PIN_SET)
            {
                cur_x_steps++;
            }
            else
            {
                cur_x_steps--;
            }
            // Проверяем достижение цели
            if (cur_x_steps == tgt_x_steps)
            {
                x_busy = false;
                x_current_speed = 0.0f;
                x_pulse_interval = 0xFFFFFFFF;
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
            }
        }
        else if (x_pulse_counter == 1)
        {
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
        }
    }
}