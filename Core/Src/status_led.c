#include "status_led.h"
#include "stm32f4xx_hal.h"

/*
 * STATUS LED
 * PC13
 */
#define STATUS_LED_PORT GPIOC
#define STATUS_LED_PIN  GPIO_PIN_13

#define TOGGLE_0_5HZ_MS 1000U
#define TOGGLE_1HZ_MS    500U
#define TOGGLE_3HZ_MS    166U

#define ACTIVITY_TIMEOUT_MS 300U

static VCP_State_t vcp_state = VCP_DISCONNECTED;
static uint32_t last_toggle_ms = 0;
static uint32_t last_activity_ms = 0;

static uint32_t get_toggle_period(void)
{
    switch (vcp_state)
    {
        case VCP_DISCONNECTED: return TOGGLE_0_5HZ_MS;
        case VCP_CONNECTED:    return TOGGLE_1HZ_MS;
        case VCP_ACTIVE:       return TOGGLE_3HZ_MS;
        default:               return TOGGLE_0_5HZ_MS;
    }
}

void StatusLED_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    gpio.Pin   = STATUS_LED_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(STATUS_LED_PORT, &gpio);
    HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_RESET);

    vcp_state = VCP_DISCONNECTED;
    last_toggle_ms = HAL_GetTick();
    last_activity_ms = 0;
}

void StatusLED_SetVCPState(VCP_State_t state)
{
    vcp_state = state;

    if (state != VCP_ACTIVE)
    {
        last_activity_ms = 0;
    }
}

void StatusLED_NotifyActivity(void)
{
    last_activity_ms = HAL_GetTick();
    vcp_state = VCP_ACTIVE;
}

void StatusLED_Task(void)
{
    uint32_t now = HAL_GetTick();

    if (vcp_state == VCP_ACTIVE)
    {
        if ((now - last_activity_ms) > ACTIVITY_TIMEOUT_MS)
        {
            vcp_state = VCP_CONNECTED;
        }
    }

    uint32_t period = get_toggle_period();

    if ((now - last_toggle_ms) >= period)
    {
        last_toggle_ms = now;
        HAL_GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
    }
}