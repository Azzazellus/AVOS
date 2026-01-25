#include "main.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

#include "app.h"

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  */
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

/* ===== SYSTEM CLOCK (МИНИМАЛЬНАЯ СТАБИЛЬНАЯ ЗАГЛУШКА) ===== */
void SystemClock_Config(void)
{
  /* Clock already configured by CubeMX or not required */
}

/* ===== ERROR HANDLER (ОБЯЗАТЕЛЬНО ДЛЯ USB) ===== */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    /* fatal error */
  }
}