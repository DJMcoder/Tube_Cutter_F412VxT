/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#define SERVO_PWM_Pin GPIO_PIN_0
#define SERVO_PWM_GPIO_Port GPIOA
#define STEP_EN_Pin GPIO_PIN_9
#define STEP_EN_GPIO_Port GPIOD
#define STEP_DIR_Pin GPIO_PIN_10
#define STEP_DIR_GPIO_Port GPIOD
#define STEP_PULSE_Pin GPIO_PIN_11
#define STEP_PULSE_GPIO_Port GPIOD
#define B1_Pin GPIO_PIN_9
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI9_5_IRQn
#define STEPPER_DOWN_Pin GPIO_PIN_10
#define STEPPER_DOWN_GPIO_Port GPIOC
#define STEPPER_UP_Pin GPIO_PIN_11
#define STEPPER_UP_GPIO_Port GPIOC
#define LD7_Pin GPIO_PIN_3
#define LD7_GPIO_Port GPIOD
#define LD6_Pin GPIO_PIN_4
#define LD6_GPIO_Port GPIOD
#define LD5_Pin GPIO_PIN_5
#define LD5_GPIO_Port GPIOD
#define LD4_Pin GPIO_PIN_6
#define LD4_GPIO_Port GPIOD
#define LD3_Pin GPIO_PIN_7
#define LD3_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
