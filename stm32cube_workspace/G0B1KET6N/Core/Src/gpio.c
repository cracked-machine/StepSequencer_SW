/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /**/
  LL_GPIO_SetOutputPin(TLC5955_SPI2_LAT_GPIO_Port, TLC5955_SPI2_LAT_Pin);

  /**/
  LL_GPIO_SetOutputPin(TLC5955_SPI2_MOSI_GPIO_Port, TLC5955_SPI2_MOSI_Pin);

  /**/
  LL_GPIO_SetOutputPin(TLC5955_SPI2_SCK_GPIO_Port, TLC5955_SPI2_SCK_Pin);

  /**/
  LL_GPIO_ResetOutputPin(SPI1_DC_GPIO_Port, SPI1_DC_Pin);

  /**/
  LL_GPIO_ResetOutputPin(SPI1_RESET_GPIO_Port, SPI1_RESET_Pin);

  /**/
  GPIO_InitStruct.Pin = TLC5955_SPI2_LAT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(TLC5955_SPI2_LAT_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SPI1_DC_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(SPI1_DC_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SPI1_RESET_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(SPI1_RESET_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = TLC5955_SPI2_MOSI_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(TLC5955_SPI2_MOSI_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = TLC5955_SPI2_SCK_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(TLC5955_SPI2_SCK_GPIO_Port, &GPIO_InitStruct);

  /**/
  LL_SYSCFG_EnableFastModePlus(LL_SYSCFG_I2C_FASTMODEPLUS_PB9);

  /**/
  LL_SYSCFG_EnableFastModePlus(LL_SYSCFG_I2C_FASTMODEPLUS_PB7);

  /**/
  LL_SYSCFG_EnableFastModePlus(LL_SYSCFG_I2C_FASTMODEPLUS_PB8);

  /**/
  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTA, LL_EXTI_CONFIG_LINE5);

  /**/
  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTA, LL_EXTI_CONFIG_LINE10);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_5;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_10;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(I2C3_INT_GPIO_Port, I2C3_INT_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinPull(ROTENC_SW_EXTI_GPIO_Port, ROTENC_SW_EXTI_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinMode(I2C3_INT_GPIO_Port, I2C3_INT_Pin, LL_GPIO_MODE_INPUT);

  /**/
  LL_GPIO_SetPinMode(ROTENC_SW_EXTI_GPIO_Port, ROTENC_SW_EXTI_Pin, LL_GPIO_MODE_INPUT);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI4_15_IRQn, 0);
  NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
