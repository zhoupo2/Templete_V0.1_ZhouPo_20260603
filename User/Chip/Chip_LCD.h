#ifndef __CHIP_LCD_H__
#define __CHIP_LCD_H__

#include <stdint.h>

#define CHIP_LCD_WHITE   0xFFFFU
#define CHIP_LCD_BLACK   0x0000U
#define CHIP_LCD_GREEN   0x07E0U
#define CHIP_LCD_YELLOW  0xFFE0U

void Chip_LCD_Init(void);
void Chip_LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *str);
void Chip_LCD_FillScreen(uint16_t color);
void Chip_LCD_SetBacklight(uint32_t brightness);

#endif
