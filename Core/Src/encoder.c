#include "encoder.h"
#include "main.h"

/* ==== GPIO ==== */
#define ENC_A_PIN   GPIO_PIN_6
#define ENC_B_PIN   GPIO_PIN_7
#define ENC_BTN_PIN GPIO_PIN_5

#define ENC_PORT GPIOB

#define BTN_DEBOUNCE_MS 50

static uint8_t  last_ab;
static int32_t  jog_delta;
static JogAxis_t axis = AXIS_Z;

static uint32_t last_btn_ms;
static uint8_t  last_btn_state;

static uint8_t read_ab(void)
{
    uint8_t a = HAL_GPIO_ReadPin(ENC_PORT, ENC_A_PIN) ? 1 : 0;
    uint8_t b = HAL_GPIO_ReadPin(ENC_PORT, ENC_B_PIN) ? 1 : 0;
    return (a << 1) | b;
}

static const int8_t enc_table[16] =
{
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0
};

void Encoder_Init(void)
{
    last_ab = read_ab();
    jog_delta = 0;

    last_btn_state = HAL_GPIO_ReadPin(ENC_PORT, ENC_BTN_PIN);
    last_btn_ms = HAL_GetTick();
}

void Encoder_Task(void)
{
    /* --- rotation --- */
    uint8_t ab = read_ab();
    uint8_t idx = (last_ab << 2) | ab;

    int8_t step = enc_table[idx];
    if (step)
    {
        jog_delta += step;
        last_ab = ab;
    }

    /* --- button --- */
    uint8_t btn = HAL_GPIO_ReadPin(ENC_PORT, ENC_BTN_PIN);
    uint32_t now = HAL_GetTick();

    if (btn != last_btn_state && (now - last_btn_ms) > BTN_DEBOUNCE_MS)
    {
        last_btn_ms = now;
        last_btn_state = btn;

        if (btn == GPIO_PIN_RESET)
        {
            axis = (axis == AXIS_X) ? AXIS_Z : AXIS_X;
        }
    }
}

JogAxis_t Encoder_GetAxis(void)
{
    return axis;
}

int32_t Encoder_GetJogDelta(void)
{
    return jog_delta;
}

void Encoder_ResetJog(void)
{
    jog_delta = 0;
}