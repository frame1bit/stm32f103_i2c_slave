
/**
 *  ws2812_STM32.h
 *  Created by Yeremias Yosef Bria on 23 October 2019.
 *  yeramias.bria@gmail.com
 *  Library LED WS2812
 */
#ifndef __ws2812_STM32_h
#define __ws2812_STM32_h

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "app_config.h"


/* ============ User Defined Config Led (Jumlah LED) ============ */
#define count_led           (CONFIG_LED_NUMBER)

#define Enable_GamaTable    (1)

#if Enable_GamaTable
  static const uint8_t gammaTable[256] =
      {
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
          3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7,
          7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12,
          13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
          20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
          30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
          42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
          58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
          76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
          97, 99, 100, 102, 103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120,
          122, 124, 125, 127, 129, 130, 132, 134, 136, 137, 139, 141, 143, 145, 146, 148,
          150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180,
          182, 184, 186, 188, 191, 193, 195, 197, 199, 202, 204, 206, 209, 211, 213, 215,
          218, 220, 223, 225, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252, 255};
#endif

  uint32_t millis(void);
  long map(long x, long in_min, long in_max, long out_min, long out_max);
  short abs1(short n);
  uint32_t ws2812_color(uint8_t r, uint8_t g, uint8_t b);
  uint32_t ws2812_color_Brightness(uint32_t color, uint8_t Brightness);
  void ws2812_setPixelColor(uint16_t pixel, uint32_t color);
  void ws2812_show(void);
  void clearBuf_led(void);
  void ws2812_clear(void);
  void ws2812_init(SPI_HandleTypeDef *hspi);
  void colorWipe(uint32_t c, uint8_t wait);
  uint32_t Wheel(uint8_t WheelPos);
  void fillColor_up(uint32_t color, uint8_t time);
  void fillColor_down(uint32_t color, uint8_t time);

#ifdef __cplusplus
}
#endif

#endif
