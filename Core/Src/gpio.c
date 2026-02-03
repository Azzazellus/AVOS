#include "gpio.h"

void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* STATUS LED */
  GPIO_InitStruct.Pin = STS_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STS_LED_GPIO_Port, &GPIO_InitStruct);

  /* STEP PA6 / PA7 - теперь управляются через таймер TIM3 */
  GPIO_InitStruct.Pin = STEP_Z_Pin | STEP_X_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;  // Альтернативная функция (PWM)
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;  // PA6 и PA7 работают с TIM3
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* DIR пины */
  GPIO_InitStruct.Pin = DIR_Z_Pin | DIR_X_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Концевик Z (PC14) - вход с подтяжкой к питанию (NC нормально разомкнут) */
  GPIO_InitStruct.Pin = Z_LIMIT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;  // Подтяжка к VCC, если концевик NC (разомкнут = 1, замкнут = 0)
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Z_LIMIT_GPIO_Port, &GPIO_InitStruct);

  /* Кнопка STOP (PB4) */
  GPIO_InitStruct.Pin = STOP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;  // Подтяжка к VCC, кнопка замыкает на землю
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STOP_GPIO_Port, &GPIO_InitStruct);

  /* Инициализируем DIR в 0 */
  HAL_GPIO_WritePin(DIR_Z_GPIO_Port, DIR_Z_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DIR_X_GPIO_Port, DIR_X_Pin, GPIO_PIN_RESET);
}