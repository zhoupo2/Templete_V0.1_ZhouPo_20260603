#include "Chip_LCD.h"
#include "st7735.h"
#include "font.h"
#include "spi.h"
#include "main.h"
#include "tim.h"
#include <stdio.h>

#define LCD_RS_SET   HAL_GPIO_WritePin(LCD_WR_RS_GPIO_Port, LCD_WR_RS_Pin, GPIO_PIN_SET)
#define LCD_RS_RESET HAL_GPIO_WritePin(LCD_WR_RS_GPIO_Port, LCD_WR_RS_Pin, GPIO_PIN_RESET)
#define LCD_CS_SET   HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_CS_RESET HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)

static ST7735_IO_t s_lcd_io;
static ST7735_Object_t s_lcd_obj;
extern ST7735_Ctx_t ST7735Ctx;
static uint16_t s_point_color = CHIP_LCD_WHITE;
static uint16_t s_back_color = CHIP_LCD_BLACK;

static int32_t lcd_bus_init(void);
static int32_t lcd_gettick(void);
static int32_t lcd_writereg(uint8_t reg, uint8_t *pdata, uint32_t length);
static int32_t lcd_readreg(uint8_t reg, uint8_t *pdata);
static int32_t lcd_senddata(uint8_t *pdata, uint32_t length);
static int32_t lcd_recvdata(uint8_t *pdata, uint32_t length);

static int32_t lcd_bus_init(void)
{
  return ST7735_OK;
}

static int32_t lcd_gettick(void)
{
  return (int32_t)HAL_GetTick();
}

static int32_t lcd_writereg(uint8_t reg, uint8_t *pdata, uint32_t length)
{
  int32_t result;

  LCD_CS_RESET;
  LCD_RS_RESET;
  result = HAL_SPI_Transmit(&hspi4, &reg, 1, 100);
  LCD_RS_SET;
  if (length > 0U)
  {
    result += HAL_SPI_Transmit(&hspi4, pdata, (uint16_t)length, 500);
  }
  LCD_CS_SET;
  return (result == HAL_OK) ? ST7735_OK : ST7735_ERROR;
}

static int32_t lcd_readreg(uint8_t reg, uint8_t *pdata)
{
  int32_t result;

  LCD_CS_RESET;
  LCD_RS_RESET;
  result = HAL_SPI_Transmit(&hspi4, &reg, 1, 100);
  LCD_RS_SET;
  result += HAL_SPI_Receive(&hspi4, pdata, 1, 500);
  LCD_CS_SET;
  return (result == HAL_OK) ? ST7735_OK : ST7735_ERROR;
}

static int32_t lcd_senddata(uint8_t *pdata, uint32_t length)
{
  int32_t result;

  LCD_CS_RESET;
  result = HAL_SPI_Transmit(&hspi4, pdata, (uint16_t)length, 1000);
  LCD_CS_SET;
  return (result == HAL_OK) ? ST7735_OK : ST7735_ERROR;
}

static int32_t lcd_recvdata(uint8_t *pdata, uint32_t length)
{
  int32_t result;

  LCD_CS_RESET;
  result = HAL_SPI_Receive(&hspi4, pdata, (uint16_t)length, 500);
  LCD_CS_SET;
  return (result == HAL_OK) ? ST7735_OK : ST7735_ERROR;
}

static void Chip_LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
  uint8_t temp;
  uint8_t t;
  uint8_t t1;
  uint16_t y0 = y;
  uint16_t x0 = x;
  uint16_t colortemp = s_point_color;
  uint32_t h;
  uint32_t w;
  uint16_t write[16][8];
  uint16_t count = 0;

  ST7735_GetXSize(&s_lcd_obj, &w);
  ST7735_GetYSize(&s_lcd_obj, &h);
  num = (uint8_t)(num - ' ');

  if (mode == 0U)
  {
    for (t = 0; t < size; t++)
    {
      temp = (size == 12U) ? asc2_1206[num][t] : asc2_1608[num][t];
      for (t1 = 0; t1 < 8U; t1++)
      {
        if ((temp & 0x80U) != 0U)
        {
          s_point_color = (uint16_t)((colortemp & 0xFFU) << 8) | (colortemp >> 8);
        }
        else
        {
          s_point_color = (uint16_t)((s_back_color & 0xFFU) << 8) | (s_back_color >> 8);
        }
        write[count][t / 2U] = s_point_color;
        count++;
        if (count >= size)
        {
          count = 0;
        }
        temp <<= 1;
        y++;
        if (y >= h)
        {
          s_point_color = colortemp;
          return;
        }
        if ((y - y0) == size)
        {
          y = y0;
          x++;
          if (x >= w)
          {
            s_point_color = colortemp;
            return;
          }
          break;
        }
      }
    }
  }

  ST7735_FillRGBRect(&s_lcd_obj, x0, y0, (uint8_t *)&write, (size == 12U) ? 6U : 8U, size);
  s_point_color = colortemp;
}

void Chip_LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *str)
{
  uint16_t x0 = x;

  width += x;
  height += y;
  while ((*str <= '~') && (*str >= ' '))
  {
    if (x >= width)
    {
      x = x0;
      y += size;
    }
    if (y >= height)
    {
      break;
    }
    Chip_LCD_ShowChar(x, y, *str, size, 0);
    x += size / 2U;
    str++;
  }
}

void Chip_LCD_FillScreen(uint16_t color)
{
  ST7735_LCD_Driver.FillRect(&s_lcd_obj, 0, 0, ST7735Ctx.Width, ST7735Ctx.Height, color);
}

void Chip_LCD_SetBacklight(uint32_t brightness)
{
  if (brightness > 1000U)
  {
    brightness = 1000U;
  }
  __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, brightness);
}

void Chip_LCD_Init(void)
{
  uint32_t lcd_id = 0;

  s_lcd_io.Init = lcd_bus_init;
  s_lcd_io.DeInit = NULL;
  s_lcd_io.Address = 0;
  s_lcd_io.WriteReg = lcd_writereg;
  s_lcd_io.ReadReg = lcd_readreg;
  s_lcd_io.SendData = lcd_senddata;
  s_lcd_io.RecvData = lcd_recvdata;
  s_lcd_io.GetTick = lcd_gettick;

  ST7735Ctx.Orientation = ST7735_ORIENTATION_LANDSCAPE_ROT180;
  ST7735Ctx.Panel = HannStar_Panel;
  ST7735Ctx.Type = ST7735_0_9_inch_screen;

  ST7735_RegisterBusIO(&s_lcd_obj, &s_lcd_io);
  ST7735_LCD_Driver.Init(&s_lcd_obj, ST7735_FORMAT_RBG565, &ST7735Ctx);
  ST7735_LCD_Driver.ReadID(&s_lcd_obj, &lcd_id);
  ST7735_LCD_Driver.SetBrightness(&s_lcd_obj, 100);
  Chip_LCD_FillScreen(CHIP_LCD_BLACK);

  /* Turn on LCD backlight via TIM1_CH2N (PE10) PWM, 50% brightness */
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
  __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 500);

  (void)lcd_id;
}
