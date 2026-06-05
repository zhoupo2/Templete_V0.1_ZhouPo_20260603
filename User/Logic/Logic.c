#include "Logic.h"
#include "APP_DHT11.h"
#include "BSP_Timer.h"
#include "BSP_IIC.h"
#include "Chip_Print.h"
#include "Formula_Print.h"

static uint32_t s_last_update_ms = 0;

void Logic_Init(void)
{
  BSP_IIC_Init();
  BSP_Timer_Init();
  Chip_Print_Init();
  APP_DHT11_Init();
  s_last_update_ms = BSP_GetTick();
}

void Print_Init(void)
{
  Printg("DHT11 init OK\r\n");
}

void Logic_Loop(void)
{
  uint32_t now = BSP_GetTick();

  if ((now - s_last_update_ms) >= 2000U)
  {
    s_last_update_ms = now;
    APP_DHT11_Update();
  }
}

void Print_Loop(void)
{
}
