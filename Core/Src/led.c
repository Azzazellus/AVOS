#include "led.h"
#include "tim.h"
#include "stm32f4xx_hal.h"

/*
LED_R  -> TIM2 CH1 (PA5)
LED_G  -> TIM2 CH3 (PA2)
LED_B  -> TIM2 CH4 (PA3)
LED_UV -> TIM1 CH3 (PA10)
LED_IR -> TIM1 CH1 (PA8)
*/

static uint8_t r = 0, g = 0, b = 0, uv = 0, ir = 0;
static bool led_enabled = true;

static uint32_t pct_to_ccr(uint8_t pct, uint32_t arr)
{
    if (pct > 100) pct = 100;
    return (arr * pct) / 100;
}

void LED_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

static void apply(void)
{
    uint32_t arr1 = __HAL_TIM_GET_AUTORELOAD(&htim1);
    uint32_t arr2 = __HAL_TIM_GET_AUTORELOAD(&htim2);

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, led_enabled ? pct_to_ccr(r, arr2) : 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, led_enabled ? pct_to_ccr(g, arr2) : 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, led_enabled ? pct_to_ccr(b, arr2) : 0);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, led_enabled ? pct_to_ccr(uv, arr1) : 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, led_enabled ? pct_to_ccr(ir, arr1) : 0);
}

void LED_SetR(uint8_t p){ r = p; apply(); }
void LED_SetG(uint8_t p){ g = p; apply(); }
void LED_SetB(uint8_t p){ b = p; apply(); }
void LED_SetUV(uint8_t p){ uv = p; apply(); }
void LED_SetIR(uint8_t p){ ir = p; apply(); }

void LED_On(void)
{
    led_enabled = true;
    apply();
}

void LED_Off(void)
{
    led_enabled = false;
    apply();
}

void LED_Trigger(uint32_t duration_ms)
{
    LED_On();
    HAL_Delay(duration_ms);
    LED_Off();
}