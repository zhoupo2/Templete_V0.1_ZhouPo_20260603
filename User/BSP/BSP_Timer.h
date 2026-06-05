#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include <stdint.h>

void BSP_Timer_Init(void);
void BSP_Delay_ms(uint32_t ms);
void BSP_Delay_us(uint32_t us);
uint32_t BSP_GetTick(void);

#endif
