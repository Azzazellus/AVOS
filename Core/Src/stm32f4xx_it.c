#include "main.h"
#include "stm32f4xx_it.h"

/* USER CODE BEGIN Includes */
#include "tim.h"
#include "stepper.h"
#include "usbd_core.h"
/* USER CODE END Includes */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers           */
/******************************************************************************/

void NMI_Handler(void)
{
    while (1) {}
}

void HardFault_Handler(void)
{
    __disable_irq();
    while (1) {}
}

void MemManage_Handler(void)
{
    __disable_irq();
    while (1) {}
}

void BusFault_Handler(void)
{
    __disable_irq();
    while (1) {}
}

void UsageFault_Handler(void)
{
    __disable_irq();
    while (1) {}
}

void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}

void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32 Peripheral Interrupt Handlers                                         */
/******************************************************************************/

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(STOP_Pin);
}

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

/******************************************************************************/
/* HAL CALLBACKS                                                              */
/******************************************************************************/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        Stepper_TIM_ISR();
    }
}

/******************************************************************************/
/* EXTI CALLBACK                                                              */
/******************************************************************************/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == STOP_Pin)
    {
        Stepper_EmergencyStop();
    }
}