#include "app.h"
#include "stepper.h"
#include "status_led.h"
#include "cli.h"
#include "vcp_monitor.h"

void App_Init(void)
{
    // Инициализация в правильном порядке
    StatusLED_Init();     // Сначала светодиод статуса
    Stepper_Init();       // Затем шаговые двигатели
    CLI_Init();           // Командный интерфейс
    VCP_Init();           // USB VCP

    // Всегда запускаем homing после инициализации
    Stepper_StartHoming();
}

void App_Loop(void)
{
    // Основной цикл обработки задач
    Stepper_Task();       // Управление шаговыми двигателями
    CLI_Task();           // Обработка команд
    VCP_Task();           // USB VCP мониторинг
    StatusLED_Task();     // Светодиод статуса
}