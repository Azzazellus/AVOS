/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"

/* USER CODE BEGIN Includes */
#include "tim.h"
#include "stepper.h"
#include "usbd_core.h"
/* USER CODE END Includes */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;  // Важно для USB!
extern TIM_HandleTypeDef htim2;            // Для TIM2
extern TIM_HandleTypeDef htim3;            // Для TIM3

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
    /* В NMI можно попасть при сбое часов, проверке памяти и т.д. */
    while (1)
    {
        /* Аварийное мигание: 1 короткое, 1 длинное */
        HAL_GPIO_WritePin(STS_LED_GPIO_Port, STS_LED_Pin, GPIO_PIN_RESET);
        for(volatile int i = 0; i < 1000000; i++);  // Задержка
        HAL_GPIO_WritePin(STS_LED_GPIO_Port, STS_LED_Pin, GPIO_PIN_SET);
        for(volatile int i = 0; i < 3000000; i++);
    }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
    /* Hard Fault - самая серьезная ошибка */
    __disable_irq();
    while (1)
    {
        /* Мигаем 4 раза быстро - код ошибки Hard Fault */
        for(int i = 0; i < 4; i++) {
            HAL_GPIO_WritePin(STS_LED_GPIO_Port, STS_LED_Pin, GPIO_PIN_RESET);
            for(volatile int j = 0; j < 500000; j++);
            HAL_GPIO_WritePin(STS_LED_GPIO_Port, STS_LED_Pin, GPIO_PIN_SET);
            for(volatile int j = 0; j < 500000; j++);
        }
        for(volatile int i = 0; i < 2000000; i++);  // Пауза
    }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* 2 мигания - Memory Fault */
        for(int i = 0; i < 2; i++) {
            HAL_GPIO_TogglePin(STS_LED_GPIO_Port, STS_LED_Pin);
            for(volatile int j = 0; j < 1000000; j++);
        }
        for(volatile int i = 0; i < 3000000; i++);
    }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* 3 мигания - Bus Fault */
        for(int i = 0; i < 3; i++) {
            HAL_GPIO_TogglePin(STS_LED_GPIO_Port, STS_LED_Pin);
            for(volatile int j = 0; j < 1000000; j++);
        }
        for(volatile int i = 0; i < 3000000; i++);
    }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* 4 мигания - Usage Fault */
        for(int i = 0; i < 4; i++) {
            HAL_GPIO_TogglePin(STS_LED_GPIO_Port, STS_LED_Pin);
            for(volatile int j = 0; j < 1000000; j++);
        }
        for(volatile int i = 0; i < 3000000; i++);
    }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
    /* Обычно не используется в приложениях без RTOS */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
    /* Используется отладчиком */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
    /* Используется RTOS для переключения контекста */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    HAL_IncTick();

    /* Если используется HAL Delay, нужно вызвать HAL_SYSTICK_IRQHandler */
    HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles RCC global interrupt.
  * @note Обычно не используется, но объявлен в CubeMX
  */
void RCC_IRQHandler(void)
{
    /* Обработка прерываний RCC (CSSC, HSE готов, PLL готов и т.д.) */
    /* Обычно не требуется в стандартных приложениях */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);

    /* Если TIM2 используется для чего-то, добавьте обработку здесь */
    /* Например, для энкодера или другого таймера */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
    /* Обработка прерывания таймера 3 */
    HAL_TIM_IRQHandler(&htim3);

    /* Вызов обработчика степпера */
    Stepper_TIM_ISR();
}

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
    /* Обработка прерывания по линии EXTI4 (кнопка STOP) */
    HAL_GPIO_EXTI_IRQHandler(STOP_Pin);
}

/**
  * @brief This function handles USB On The Go FS global interrupt.
  * @note КРИТИЧЕСКИ ВАЖНО для работы USB!
  */
void OTG_FS_IRQHandler(void)
{
    /* Обработка прерываний USB */
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == STOP_Pin)
    {
        /* Emergency STOP */
        Stepper_EmergencyStop();

        /* Индикация срабатывания аварийной остановки */
        for(int i = 0; i < 10; i++) {
            HAL_GPIO_TogglePin(STS_LED_GPIO_Port, STS_LED_Pin);
            HAL_Delay(50);
        }
    }
}

/**
  * @brief  Эта функция вызывается при ошибке в обработчике прерывания таймера
  * @param  htim: указатель на структуру таймера
  * @retval None
  */
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        /* Ошибка таймера 3 - критично для степпера */
        Stepper_EmergencyStop();

        /* Сигнализируем об ошибке */
        while(1) {
            for(int i = 0; i < 5; i++) {
                HAL_GPIO_TogglePin(STS_LED_GPIO_Port, STS_LED_Pin);
                for(volatile int j = 0; j < 500000; j++);
            }
            for(volatile int i = 0; i < 2000000; i++);
        }
    }
}