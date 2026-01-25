#ifndef PINS_H
#define PINS_H

#include "stm32f4xx_hal.h"

/* ===================== STEPPER ===================== */
// STEP (TIM3)
#define STEP_Z_PORT    GPIOA
#define STEP_Z_PIN     GPIO_PIN_6   // TIM3_CH1

#define STEP_X_PORT    GPIOA
#define STEP_X_PIN     GPIO_PIN_7   // TIM3_CH2

// DIR
#define DIR_Z_PORT     GPIOB
#define DIR_Z_PIN      GPIO_PIN_13

#define DIR_X_PORT     GPIOB
#define DIR_X_PIN      GPIO_PIN_15

/* ===================== ENCODER ===================== */
#define ENC_A_PORT     GPIOB
#define ENC_A_PIN      GPIO_PIN_6

#define ENC_B_PORT     GPIOB
#define ENC_B_PIN      GPIO_PIN_7

#define ENC_BTN_PORT   GPIOB
#define ENC_BTN_PIN    GPIO_PIN_5

/* ===================== LIMIT / STOP ===================== */
#define Z_LIMIT_PORT   GPIOC
#define Z_LIMIT_PIN    GPIO_PIN_14   // NC

#define STOP_PORT      GPIOB
#define STOP_PIN       GPIO_PIN_4

/* ===================== STATUS LED ===================== */
#define STS_LED_PORT   GPIOC
#define STS_LED_PIN    GPIO_PIN_13

#endif /* PINS_H */