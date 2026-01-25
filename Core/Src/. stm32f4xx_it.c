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
/* USER CODE END Includes */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/

__attribute__((noreturn)) void NMI_Handler(void)
{
    for (;;)
    {
    }
}

__attribute__((noreturn)) void HardFault_Handler(void)
{
    for (;;)
    {
    }
}

__attribute__((noreturn)) void MemManage_Handler(void)
{
    for (;;)
    {
    }
}

__attribute__((noreturn)) void BusFault_Handler(void)
{
    for (;;)
    {
    }
}

__attribute__((noreturn)) void UsageFault_Handler(void)
{
    for (;;)
    {
    }
}

void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/*                 STM32F4xx Peripheral Interrupt Handlers                    */
/******************************************************************************/

/* ===== TIM3 — базовый тик 10 кГц ===== */
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
    Stepper_TIM_ISR();
}

/* ===== EXTI — EMERGENCY STOP ===== */
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_Callback(STOP_Pin);
}

/* ===== HAL CALLBACK ===== */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == STOP_Pin)
    {
        Stepper_EmergencyStop();
    }
}