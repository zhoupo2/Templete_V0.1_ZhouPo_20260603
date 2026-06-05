#ifndef __CHIP_PRINT_H__
#define __CHIP_PRINT_H__

#include <stdint.h>

void Chip_Print_Init(void);
void Chip_Print_Send(const uint8_t *buf, uint16_t len);

#endif
