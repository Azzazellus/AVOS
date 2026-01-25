/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STS_LED_Pin GPIO_PIN_13
#define STS_LED_GPIO_Port GPIOC
#define Z_LIMIT_Pin GPIO_PIN_14
#define Z_LIMIT_GPIO_Port GPIOC
#define LED_G_Pin GPIO_PIN_2
#define LED_G_GPIO_Port GPIOA
#define LED_B_Pin GPIO_PIN_3
#define LED_B_GPIO_Port GPIOA
#define LED_R_Pin GPIO_PIN_5
#define LED_R_GPIO_Port GPIOA
#define STEP_Z_Pin GPIO_PIN_6
#define STEP_Z_GPIO_Port GPIOA
#define STEP_X_Pin GPIO_PIN_7
#define STEP_X_GPIO_Port GPIOA
#define DIR_Z_Pin GPIO_PIN_13
#define DIR_Z_GPIO_Port GPIOB
#define DIR_X_Pin GPIO_PIN_15
#define DIR_X_GPIO_Port GPIOB
#define LED_IR_Pin GPIO_PIN_8
#define LED_IR_GPIO_Port GPIOA
#define LED_UV_Pin GPIO_PIN_10
#define LED_UV_GPIO_Port GPIOA
#define STOP_Pin GPIO_PIN_4
#define STOP_GPIO_Port GPIOB
#define ENC_BTN_Pin GPIO_PIN_5
#define ENC_BTN_GPIO_Port GPIOB
#define ENC_A_Pin GPIO_PIN_6
#define ENC_A_GPIO_Port GPIOB
#define ENC_B_Pin GPIO_PIN_7
#define ENC_B_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */