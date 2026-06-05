/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   SPI4 for ST7735 LCD
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern SPI_HandleTypeDef hspi4;

void MX_SPI4_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */
