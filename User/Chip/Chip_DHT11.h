#ifndef __CHIP_DHT11_H__
#define __CHIP_DHT11_H__

#include <stdint.h>

typedef struct
{
  uint8_t humidity_int;
  uint8_t humidity_dec;
  uint8_t temperature_int;
  uint8_t temperature_dec;
  uint8_t valid;
} Chip_DHT11_Data_t;

void Chip_DHT11_Init(void);
uint8_t Chip_DHT11_Read(Chip_DHT11_Data_t *data);

#endif
