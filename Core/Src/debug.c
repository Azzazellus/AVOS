#include "stm32f4xx_hal.h"
#include "main.h"

void Debug_TestClocks(void)
{
    /* Проверка частот */
    uint32_t sysclk = HAL_RCC_GetSysClockFreq();
    uint32_t hclk = HAL_RCC_GetHCLKFreq();
    uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
    uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();
    
    /* Мигаем в зависимости от частоты */
    if(sysclk < 80000000) {
        /* Ошибка: частота слишком низкая */
        while(1) {
            HAL_GPIO_TogglePin(STS_LED_GPIO_Port, STS_LED_Pin);
            HAL_Delay(500);  // Медленное мигание - ошибка
        }
    }
}

void Debug_TestGPIO(void)
{
    /* Тест светодиода */
    for(int i = 0; i < 5; i++) {
        HAL_GPIO_TogglePin(STS_LED_GPIO_Port, STS_LED_Pin);
        HAL_Delay(100);
    }
}