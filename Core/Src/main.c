#include "main.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"
#include "app.h"

void SystemClock_Config(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USB_DEVICE_Init();

  App_Init();

  while (1)
  {
    App_Loop();
  }
}

/* ===== SYSTEM CLOCK для STM32F401 ===== */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Настройка регулятора напряжения */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /* Настройка источников тактирования */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  /* Для STM32F401 с HSE 25 МГц */
  RCC_OscInitStruct.PLL.PLLM = 25;        /* M = 25 (25MHz / 25 = 1MHz) */
  RCC_OscInitStruct.PLL.PLLN = 192;       /* N = 192 (1MHz * 192 = 192MHz) */
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; /* P = 2 (192MHz / 2 = 96MHz SYSCLK) */
  RCC_OscInitStruct.PLL.PLLQ = 4;         /* Q = 4 (192MHz / 4 = 48MHz для USB) */

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Настройка тактирования шин */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;     /* HCLK = 96 MHz */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;      /* APB1 = 48 MHz */
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;      /* APB2 = 96 MHz */

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }

  /* Для STM32F401 USB получает тактирование от PLLQ автоматически */
  /* Дополнительная настройка не требуется! */
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    /* Аварийное мигание */
    HAL_GPIO_TogglePin(STS_LED_GPIO_Port, STS_LED_Pin);
    HAL_Delay(100);
  }
}