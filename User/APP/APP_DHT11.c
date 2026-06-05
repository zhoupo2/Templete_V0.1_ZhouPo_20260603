#include "APP_DHT11.h"
#include "Chip_DHT11.h"
#include "Chip_LCD.h"
#include "Formula_Print.h"
#include <stdio.h>

static Chip_DHT11_Data_t s_dht_data;

void APP_DHT11_Init(void)
{
  Chip_DHT11_Init();
  Chip_LCD_Init();

  Chip_LCD_ShowString(4, 4, 160, 16, 16, (uint8_t *)"DHT11 Monitor");
  Chip_LCD_ShowString(4, 28, 160, 16, 16, (uint8_t *)"Temp:");
  Chip_LCD_ShowString(4, 48, 160, 16, 16, (uint8_t *)"Humi:");
}

void APP_DHT11_Update(void)
{
  char line[32];

  if (Chip_DHT11_Read(&s_dht_data) == 0U)
  {
    /* Fixed-width placeholder to prevent ghosting */
    Chip_LCD_ShowString(56, 28, 160, 16, 16, (uint8_t *)"--.- C ");
    Chip_LCD_ShowString(56, 48, 160, 16, 16, (uint8_t *)"-- %   ");
    Printg("DHT11 read fail\r\n");
    return;
  }

  /* %2d ensures 2-digit field width; trailing space clears any residual char */
  snprintf(line, sizeof(line), "%2d.%d C ", s_dht_data.temperature_int, s_dht_data.temperature_dec);
  Chip_LCD_ShowString(56, 28, 160, 16, 16, (uint8_t *)line);

  snprintf(line, sizeof(line), "%2d.%d %% ", s_dht_data.humidity_int, s_dht_data.humidity_dec);
  Chip_LCD_ShowString(56, 48, 160, 16, 16, (uint8_t *)line);

  Printg("Temp:%d.%d C  Humi:%d.%d %%\r\n",
         s_dht_data.temperature_int, s_dht_data.temperature_dec,
         s_dht_data.humidity_int, s_dht_data.humidity_dec);
}
