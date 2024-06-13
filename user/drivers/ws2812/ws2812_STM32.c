/**
 *  ws2812_STM32.c
 *  Created by Yeremias Yosef Bria on 23 October 2019.
 *  yeramias.bria@gmail.com
 *  Source Code LED WS2812
 */

#include "ws2812_STM32.h"
#include "app_config.h"

#define	WS_SPI	0
#define	WS_TIM	1

#define	WS2812_PERIPHERAL	(WS_SPI)	


/* Private typedef Config --------------------------------------------------------*/
/*
#define tH1        0x3E
#define tH2        0x7C

#define tL1        0x06
#define tL2        0x0C
*/
/**
 * formula drive ws2812 using SPI
 * 0 -->	T0H = 0,4 / (1/SPI Baud)
 * 			T0L = 0,85 / (1/SPI Baud)
 * 1 -->	T1H = 0,8 / (1/SPI Baud)
 * 			T1L = 0,45 / (1/SPI Baud)
 * 
 * ex.
 * SPI Baud = 6,5Mbit/s
 * T per bit 	= 1/6,5Mb
 * 				= 0,15384 us / bit
 * TOH = 0,4/(0,15384)
 * 		= 2.6 bit --> +/- 2 bit hi
 * TOL = 0,85/(0,15384)
 * 		= 5,5 bit --> 5 bit lo
 * so, to drive bit 0 it will use 2 bit hi + 5 bit 0
 * its equal to 0x06
 * 
 * 
*/
#define tH1 0x3E	//0b00111110
#define tL1 0x06	//0b00000110

#define sizeBuf count_led * 24

const int16_t _led = count_led;
static uint8_t buf_bitLed[sizeBuf + 1];

static SPI_HandleTypeDef *spi;

__weak uint32_t millis(void)
{
	return HAL_GetTick();
}

__weak long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

__weak void delayMicros(uint16_t delay)
{
	while (delay--)
	{
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}
}

short abs1(short n)
{
	return n >= 0 ? n : n * -1;
}

uint32_t ws2812_color(uint8_t r, uint8_t g, uint8_t b)
{
	return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

uint32_t ws2812_color_Brightness(uint32_t color, uint8_t Brightness)
{
#if Enable_GamaTable
	uint8_t w = gammaTable[Brightness];
#else
	uint8_t w = Brightness;
#endif
	uint8_t r, g, b;
	b = color & 0xff;
	r = (color >> 8) & 0xff;
	g = (color >> 16) & 0xff;

	r = r * w / 255;
	g = g * w / 255;
	b = b * w / 255;

	return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

void ws2812_setPixelColor(uint16_t pixel, uint32_t color)
{
	uint32_t col = color;
	uint16_t p = (uint16_t) pixel * 24;
	for (char i = 0; i < 24; i++)
	{
		/**c_e
		 * note: add condition if pixel index over from
		 * 		maximal buffer size
		*/
		if ( (p + i) > count_led * 24) return;

		col = (color << i);
		if (col & 0x800000)
			buf_bitLed[p + i] = tH1;
		else
			buf_bitLed[p + i] = tL1;
	}
}


void ws2812_show(void)
{
	HAL_SPI_Transmit(spi, buf_bitLed, sizeBuf, 50);
	for (uint8_t i = 0; i < 24; i++)
		HAL_SPI_Transmit(spi, 0, 1, 10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
}

void clearBuf_led(void)
{
	for (char i = 0; i < count_led; i++)
		ws2812_setPixelColor(i, 0);
}

void ws2812_clear(void)
{
	for (int i = 0; i < _led; i++)
		ws2812_setPixelColor(i, 0);
	ws2812_show();
}

void ws2812_init(SPI_HandleTypeDef *hspi)
{
	spi = hspi;
	ws2812_clear();
}

uint32_t Wheel(uint8_t WheelPos)
{
	if (WheelPos < 85)
	{
		return ws2812_color(WheelPos * 3, 255 - WheelPos * 3, 0);
	}
	else
	{
		if (WheelPos < 170)
		{
			WheelPos -= 85;
			return ws2812_color(255 - WheelPos * 3, 0, WheelPos * 3);
		}
		else
		{
			WheelPos -= 170;
			return ws2812_color(0, WheelPos * 3, 255 - WheelPos * 3);
		}
	}
}

void colorWipe(uint32_t c, uint8_t wait)
{
	for (uint16_t i = 0; i < _led; i++)
	{
		ws2812_setPixelColor(i, c);
		ws2812_show();
		delay_ms(wait);
	}
}

void fillColor_up(uint32_t color, uint8_t time)
{
	for (int16_t kontras = 10; kontras <= 255; kontras++)
	{
		for (char j = 0; j < count_led; j++)
			ws2812_setPixelColor(j, ws2812_color_Brightness(color, kontras));
		ws2812_show();
		delay_ms(time);
	}
}

void fillColor_down(uint32_t color, uint8_t time)
{
	for (int16_t kontras = 255; kontras > 10; kontras--)
	{
		for (char j = 0; j < count_led; j++)
			ws2812_setPixelColor(j, ws2812_color_Brightness(color, kontras));
		ws2812_show();
		delay_ms(time);
	}
	ws2812_clear();
	delay_ms(200);
}
