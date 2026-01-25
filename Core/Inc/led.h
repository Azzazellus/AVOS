#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>

/* ===== INIT ===== */
void LED_Init(void);

/* ===== SET BRIGHTNESS (0..100 %) ===== */
void LED_SetR(uint8_t percent);
void LED_SetG(uint8_t percent);
void LED_SetB(uint8_t percent);
void LED_SetUV(uint8_t percent);
void LED_SetIR(uint8_t percent);

/* ===== CONTROL ===== */
void LED_On(void);
void LED_Off(void);

/* ===== TRIGGER ===== */
void LED_Trigger(uint32_t duration_ms);

#endif /* LED_H */