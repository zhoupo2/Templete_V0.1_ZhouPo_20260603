#include "BSP_IIC.h"

/* I2C 底层占位：当前 DHT11/LCD 未使用 IIC，保留接口供扩展 */
void BSP_IIC_Init(void)
{
}

uint8_t BSP_IIC_Write(uint8_t devAddr, uint8_t reg, uint8_t *data, uint16_t len)
{
  (void)devAddr;
  (void)reg;
  (void)data;
  (void)len;
  return 1;
}

uint8_t BSP_IIC_Read(uint8_t devAddr, uint8_t reg, uint8_t *data, uint16_t len)
{
  (void)devAddr;
  (void)reg;
  (void)data;
  (void)len;
  return 1;
}
