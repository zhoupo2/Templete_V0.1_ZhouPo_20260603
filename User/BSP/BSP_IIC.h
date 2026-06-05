#ifndef __BSP_IIC_H__
#define __BSP_IIC_H__

#include <stdint.h>

void BSP_IIC_Init(void);
uint8_t BSP_IIC_Write(uint8_t devAddr, uint8_t reg, uint8_t *data, uint16_t len);
uint8_t BSP_IIC_Read(uint8_t devAddr, uint8_t reg, uint8_t *data, uint16_t len);

#endif
