#include "Chip_Print.h"
#include "usart.h"

void Chip_Print_Init(void)
{
}

void Chip_Print_Send(const uint8_t *buf, uint16_t len)
{
  if ((buf == NULL) || (len == 0U))
  {
    return;
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)buf, len, 1000);
}
