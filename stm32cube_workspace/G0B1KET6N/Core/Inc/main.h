/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_i2c.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_crs.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_spi.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#ifdef USE_RTT
  #include <SEGGER_RTT.h>
#endif
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
#define TLC5955_SPI2_LAT_Pin LL_GPIO_PIN_9
#define TLC5955_SPI2_LAT_GPIO_Port GPIOB
#define SPI1_DC_Pin LL_GPIO_PIN_0
#define SPI1_DC_GPIO_Port GPIOA
#define SPI1_RESET_Pin LL_GPIO_PIN_3
#define SPI1_RESET_GPIO_Port GPIOA
#define I2C3_INT_Pin LL_GPIO_PIN_5
#define I2C3_INT_GPIO_Port GPIOA
#define I2C3_INT_EXTI_IRQn EXTI4_15_IRQn
#define TLC5955_SPI2_GSCLK_Pin LL_GPIO_PIN_0
#define TLC5955_SPI2_GSCLK_GPIO_Port GPIOD
#define TLC5955_SPI2_MOSI_Pin LL_GPIO_PIN_7
#define TLC5955_SPI2_MOSI_GPIO_Port GPIOB
#define TLC5955_SPI2_SCK_Pin LL_GPIO_PIN_8
#define TLC5955_SPI2_SCK_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
