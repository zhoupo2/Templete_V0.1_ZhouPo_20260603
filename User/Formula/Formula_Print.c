#include "Formula_Print.h"
#include "Chip_Print.h"
#include <stdarg.h>
#include <stdio.h>

void Printg(const char *fmt, ...)
{
  char buf[128];
  va_list args;
  int len;

  va_start(args, fmt);
  len = vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  if (len > 0)
  {
    Chip_Print_Send((const uint8_t *)buf, (uint16_t)len);
  }
}
