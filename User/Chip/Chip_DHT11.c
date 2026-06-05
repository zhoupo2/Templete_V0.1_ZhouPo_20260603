#include "Chip_DHT11.h"
#include "main.h"
#include "tim.h"
#include "BSP_Timer.h"
#include "Formula_Print.h"

#define DHT11_DEBUG 1

/*
 * DHT11 1-Wire with PA1 boost driver.
 *
 * PA0 (A0): open-drain + pull-up — handles all DHT11 communication
 * PA1 (A1): input+PU at idle, briefly switched to push-pull output
 *           during the start signal's LOW and HIGH phases.
 *
 * Wiring: PA0 and PA1 shorted → DHT11 DATA pin
 */
#define DHT11_PORT   GPIOA
#define DHT11_PIN    GPIO_PIN_0
#define BOOST_PORT   GPIOA
#define BOOST_PIN    GPIO_PIN_1

/* PA0: open-drain, stays in output mode */
#define DHT11_LOW()  (DHT11_PORT->BSRR = (uint32_t)DHT11_PIN << 16U)
#define DHT11_HIGH() (DHT11_PORT->BSRR = DHT11_PIN)
#define DHT11_READ() ((DHT11_PORT->IDR & DHT11_PIN) != 0U)

/*
 * PA1: push-pull boost (input at idle).
 * Each macro is a complete mode+level transition:
 *   DRIVE_LOW:  ODR=0 → output → actively drives LOW
 *   DRIVE_HIGH: ODR=1 → output → actively drives HIGH
 *   RELEASE:    input mode → hi-Z, pull-up holds HIGH
 */
#define BOOST_DRIVE_LOW()  do { \
  BOOST_PORT->BSRR = (uint32_t)BOOST_PIN << 16U; \
  BOOST_PORT->MODER = (BOOST_PORT->MODER & ~GPIO_MODER_MODER1) | GPIO_MODER_MODER1_0; \
} while(0)

#define BOOST_DRIVE_HIGH() do { \
  BOOST_PORT->BSRR = BOOST_PIN; \
  BOOST_PORT->MODER = (BOOST_PORT->MODER & ~GPIO_MODER_MODER1) | GPIO_MODER_MODER1_0; \
} while(0)

#define BOOST_RELEASE()   (BOOST_PORT->MODER &= ~GPIO_MODER_MODER1)

static uint8_t DHT11_WaitLevel(uint8_t level, uint32_t timeout_us)
{
  uint32_t start = __HAL_TIM_GET_COUNTER(&htim2);

  while (DHT11_READ() != level)
  {
    if ((__HAL_TIM_GET_COUNTER(&htim2) - start) > timeout_us)
    {
      return 0;
    }
  }
  return 1;
}

void Chip_DHT11_Init(void)
{
  GPIO_InitTypeDef gpio = {0};

  /* PA0: open-drain output + pull-up */
  gpio.Pin = DHT11_PIN;
  gpio.Mode = GPIO_MODE_OUTPUT_OD;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(DHT11_PORT, &gpio);
  DHT11_HIGH();

  /* PA1: input + pull-up at idle (hi-Z) */
  gpio.Pin = BOOST_PIN;
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(BOOST_PORT, &gpio);

#if DHT11_DEBUG
  Printg("DHT11: PA0 OD+PU, PA1 IN+PU (v3)\r\n");
#endif
}

uint8_t Chip_DHT11_Read(Chip_DHT11_Data_t *data)
{
  uint8_t buf[5] = {0};
  uint8_t i;
  uint8_t j;
  uint32_t t0;

  if (data == NULL)
  {
    return 0;
  }

  data->valid = 0;

  /* --- Start signal LOW --- */
  DHT11_LOW();
  BOOST_DRIVE_LOW();
  BSP_Delay_ms(25);

  /* --- Start signal HIGH with boost (20us only) --- */
  DHT11_HIGH();
  BOOST_DRIVE_HIGH();
  BSP_Delay_us(20);
  BOOST_RELEASE();

  /* --- Wait for DHT11 ACK --- */
  if (!DHT11_WaitLevel(0, 150))
  {
#if DHT11_DEBUG
    Printg("FAIL@1: no ACK LOW\r\n");
#endif
    return 0;
  }

  if (!DHT11_WaitLevel(1, 150))
  {
#if DHT11_DEBUG
    Printg("FAIL@2: no ACK HIGH\r\n");
#endif
    return 0;
  }

  /* --- Read 40 bits --- */
  for (i = 0; i < 5; i++)
  {
    for (j = 0; j < 8; j++)
    {
      /* Wait for bit start LOW. Timeout 100us covers ACK_HIGH tail (~80us)
         before the first data bit, plus margin. */
      if (!DHT11_WaitLevel(0, 100))
      {
#if DHT11_DEBUG
        Printg("FAIL@3: bit[%d][%d] start\r\n", i, j);
#endif
        return 0;
      }

      if (!DHT11_WaitLevel(1, 100))
      {
#if DHT11_DEBUG
        Printg("FAIL@4: bit[%d][%d] rise\r\n", i, j);
#endif
        return 0;
      }

      BSP_Delay_us(35);
      if (DHT11_READ())
      {
        buf[i] |= (uint8_t)(1U << (7U - j));

        if (!DHT11_WaitLevel(0, 100))
        {
#if DHT11_DEBUG
          Printg("FAIL@5: bit[%d][%d] end\r\n", i, j);
#endif
          return 0;
        }
      }
    }
  }

  if ((uint8_t)(buf[0] + buf[1] + buf[2] + buf[3]) != buf[4])
  {
#if DHT11_DEBUG
    Printg("FAIL@6: chk [%02X %02X %02X %02X %02X]\r\n",
           buf[0], buf[1], buf[2], buf[3], buf[4]);
#endif
    return 0;
  }

  data->humidity_int    = buf[0];
  data->humidity_dec    = buf[1];
  data->temperature_int = buf[2];
  data->temperature_dec = buf[3];
  data->valid = 1;

#if DHT11_DEBUG
  Printg("OK: T=%d.%d C  H=%d.%d %%\r\n",
         data->temperature_int, data->temperature_dec,
         data->humidity_int, data->humidity_dec);
#endif
  return 1;
}
