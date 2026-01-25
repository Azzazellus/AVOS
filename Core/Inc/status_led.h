#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Статусы VCP:
 *  - DISCONNECTED : нет подключения
 *  - CONNECTED    : есть подключение, нет обмена
 *  - ACTIVE       : есть обмен RX/TX
 */
typedef enum
{
    VCP_DISCONNECTED = 0,
    VCP_CONNECTED,
    VCP_ACTIVE
} VCP_State_t;

/* Инициализация */
void StatusLED_Init(void);

/* Основная задача (вызывать в main loop) */
void StatusLED_Task(void);

/* Установка текущего состояния VCP */
void StatusLED_SetVCPState(VCP_State_t state);

/* Уведомление о любом RX/TX событии */
void StatusLED_NotifyActivity(void);

#endif /* STATUS_LED_H */