#include "BSP_Timer.h"
#include "tim.h"

void BSP_Timer_Init(void)
{
  HAL_TIM_Base_Start(&htim2);
}

void BSP_Delay_ms(uint32_t ms)
{
  HAL_Delay(ms);
}

void BSP_Delay_us(uint32_t us)
{
  uint32_t start = __HAL_TIM_GET_COUNTER(&htim2);

  while ((__HAL_TIM_GET_COUNTER(&htim2) - start) < us)
  {
  }
}

uint32_t BSP_GetTick(void)
{
  return HAL_GetTick();
}
