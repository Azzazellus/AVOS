#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>

/* Инициализация всего приложения */
void App_Init(void);

/* Основной цикл приложения */
void App_Loop(void);

/* Emergency STOP */
bool App_IsEmergencyStop(void);
void App_ClearEmergencyStop(void);

#endif /* APP_H */