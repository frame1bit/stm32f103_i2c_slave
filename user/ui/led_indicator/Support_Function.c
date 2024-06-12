
/**
 *  Support_Function.c
 *  Modif & Optimize by Bria on 16 September 2020.
 *  yeramias.bria@gmail.com
 *  Source Code User Support function Animation LED.
 */

#include "Animation_Style.h"

/* Experiment Brighnesst */
// volatile uint8_t Limit_Bright = 220;

/* ========================== Function Control Animation LED ===================== */
/* Fungsi ini untuk konversi Level volume untuk Auto Gain AUdio Input. */
float getAuto_Gain(uint8_t level)
{
	float tempGain;
	if (level > 75)
		tempGain = 0.35;
	else if (level > 60)
		tempGain = 0.5;
	else if (level > 50)
		tempGain = 0.75;
	else if (level >= 40)
		tempGain = 1.0;
	else if (level > 30)
		tempGain = 1.6;
	else if (level > 20)
		tempGain = 2.25;
	else if (level > 15)
		tempGain = 3.75;
	else if (level > 10)
		tempGain = 5.0;
	else
		tempGain = 5.75;
	return tempGain;
}

void getVol_shadow(uint8_t presentVol, uint8_t *levelOut, uint8_t *shadow)
{
	float temp = (float)presentVol / LED_VOL_DENOMINATOR/*4.76*/;
	uint8_t temp2 = (uint8_t)temp;
	uint8_t temp3 = 0;
	*levelOut = (uint8_t)temp;
	temp3 = (float)(temp - temp2) * 100;
	*shadow = (float)(temp3 * LED_BRIGHTNESS_DEFAULT) / 100;
}

/* ****************************		Prototype Function-1 Suport-ANIMATION		**************************** */
void setPixelColor_Ring1(uint8_t pixel, uint32_t color)
{
	if (pixel >= ledRing1max)
		pixel = 0;
	ws2812_setPixelColor(pixel, color);
	// ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, Limit_Bright));
}

void setPixelColor_Ring2(uint8_t pixel, uint32_t color)
{
	pixel += ledRing1max;
	if (pixel >= ledRing2max)
		pixel = ledRing1max;
	ws2812_setPixelColor(pixel, color);
	// ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, Limit_Bright));
}

void setPixelColor_LineAtas1(uint8_t pixel, uint32_t color)
{
	pixel = lineBawah2max + pixel; // test JH 05 10 22
	if (!(pixel >= lineAtas1max))
	{
		ws2812_setPixelColor(pixel, color);
	}
	// ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, Limit_Bright));
}

void setPixelColor_LineAtas2(uint8_t pixel, uint32_t color)
{
	pixel = (lineAtas2max - 1) - pixel; // test JH 05 10 22
	if (!(pixel <= (lineAtas1max - 1)))
	{
		ws2812_setPixelColor(pixel, color);
	}
	// ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, Limit_Bright));
}

void setPixelColor_LineBawah1(uint8_t pixel, uint32_t color)
{
	if (pixel >= lineBawah1max)
		pixel = 0;
	ws2812_setPixelColor(pixel, color);
	// ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, Limit_Bright));
}

void setPixelColor_LineBawah2(uint8_t pixel, uint32_t color)
{
	pixel = (lineBawah2max - 1) - pixel; // test JH 05 10 22
	if (pixel >= lineBawah2max)
		pixel = lineBawah1max;
	ws2812_setPixelColor(pixel, color);
	// ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, Limit_Bright));
}
/////////////////////////////////////////////////////////////////////////
void setPixelColorBrightness_LineAtas1(uint8_t pixel, uint32_t color, uint8_t brightness)
{
	pixel = lineBawah2max + pixel; // test JH 05 10 22
	if (!(pixel >= lineAtas1max))
	{
		ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, brightness));
	}
	// ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, Limit_Bright));
}

void setPixelColorBrightness_LineAtas2(uint8_t pixel, uint32_t color, uint8_t brightness)
{
	pixel = (lineAtas2max - 1) - pixel; // test JH 05 10 22
	if (!(pixel <= (lineAtas1max - 1)))
	{
		ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, brightness));
	}
}

void setPixelColorBrightness_LineBawah1(uint8_t pixel, uint32_t color, uint8_t brightness)
{
	if (pixel >= lineBawah1max)
		pixel = 0;
	ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, brightness));
	// ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, Limit_Bright));
}

void setPixelColorBrightness_LineBawah2(uint8_t pixel, uint32_t color, uint8_t brightness)
{
	pixel = (lineBawah2max - 1) - pixel; // test JH 05 10 22
	if (pixel >= lineBawah2max)
		pixel = lineBawah1max;
	ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, brightness));
	// ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, Limit_Bright));
}
//////////////////////////////////////////////////////////////////////////////////////
void setPixelColorBrightness_Ring1(uint8_t pixel, uint32_t color, uint8_t brightness)
{
	if (pixel >= ledRing1max)
		pixel = 0;
	/* Setting Pixel LED Ring With brightnesst. */
	ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, brightness));
}

void setPixelColorBrightness_Ring2(uint8_t pixel, uint32_t color, uint8_t brightness)
{
	pixel += ledRing1max;
	if (pixel >= ledRing2max)
		pixel = ledRing1max;
	/* Setting Pixel LED Ring With brightnesst. */
	ws2812_setPixelColor(pixel, ws2812_color_Brightness(color, brightness));
}

void setPixelColor_allRing1(uint8_t dynamic_pos, uint8_t jumlahLED, uint32_t color)
{
	uint8_t dPos = dynamic_pos;
	for (char i = 0; i < jumlahLED; i++)
	{
		if (dPos >= ledRing1max)
			dPos = 0;
		/* Setting Pixel LED Ring 1 */
		setPixelColor_Ring1(dPos, color);
		dPos++;
	}
}

void setPixelColor_allRing2(uint8_t dynamic_pos, uint8_t jumlahLED, uint32_t color)
{
	uint8_t dPos = dynamic_pos;
	for (char i = 0; i < jumlahLED; i++)
	{
		if (dPos >= ledRing1max)
			dPos = 0;
		/* Setting Pixel LED Ring 2 */
		setPixelColor_Ring2(dPos, color);
		dPos++;
	}
}

/**
 * @brief visual 13  running with shadow 
 * @note	c_e: remove ring 2 caused hang
*/
void loadVisual3(uint8_t dir, uint8_t dynamic_pos, uint32_t color, uint8_t brightness)
{
	uint8_t dPos = dynamic_pos;
	uint8_t Invert_Bright = (LED_BRIGHTNESS_DEFAULT - brightness);

	clearBuf_led();
	if (!dir)
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_Ring1(dPos, color, Invert_Bright);
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColorBrightness_Ring2(dPos, color, Invert_Bright);
#endif
	}
	else
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_Ring1(dPos, color, brightness);
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColorBrightness_Ring2(dPos, color, brightness);
#endif
	}
	for (char i = 0; i < (halfRing - 2); i++)
	{
		dPos++;
		if (dPos >= ledRing1max)
			dPos = 0;
		/* Setting Pixel LED Ring 1 */
		setPixelColor_Ring1(dPos, color);
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColor_Ring2(dPos, color);
#endif
	}
	if (dir)
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_Ring1(dPos, color, Invert_Bright);
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColorBrightness_Ring2(dPos, color, Invert_Bright);
#endif
	}
	else
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_Ring1(dPos, color, brightness);
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColorBrightness_Ring2(dPos, color, brightness);
#endif
	}
}
void loadVisual11(uint8_t dir, uint8_t dynamic_pos, uint32_t color, uint8_t brightness)
{
	uint8_t dPos = dynamic_pos;
	uint8_t dPos_atas = dynamic_pos;
	uint8_t Invert_Bright = (255 - brightness);

	clearBuf_led();
	if (!dir)
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_LineBawah1(dPos, color, Invert_Bright);
		setPixelColorBrightness_LineBawah2(dPos, color, Invert_Bright);
		setPixelColorBrightness_LineAtas1(dPos, color, Invert_Bright);
		setPixelColorBrightness_LineAtas2(dPos, color, Invert_Bright);
		/* Setting Pixel LED Ring 2 */
		// setPixelColorBrightness_Ring2(dPos, color, Invert_Bright);
	}
	else
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_LineBawah1(dPos, color, brightness);
		setPixelColorBrightness_LineBawah2(dPos, color, brightness);
		setPixelColorBrightness_LineAtas1(dPos, color, brightness);
		setPixelColorBrightness_LineAtas2(dPos, color, brightness);
		/* Setting Pixel LED Ring 2 */
		// setPixelColorBrightness_Ring2(dPos, color, brightness);
	}
	for (char i = 0; i < 2; i++)
	{
		dPos++;
		if (dPos == lineBawah1max)
			dPos = 0;
		/* Setting Pixel LED Ring 1 */
		setPixelColor_LineBawah1(dPos, color);
		setPixelColor_LineBawah2(dPos, color);
		setPixelColor_LineAtas1(dPos, color);
		setPixelColor_LineAtas2(dPos, color);

		/* Setting Pixel LED Ring 2 */
		// setPixelColor_Ring2(dPos, color);
	}

	// setPixelColor_Ring1(dPos, color);
	if (dir)
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_LineBawah1(dPos, color, Invert_Bright);
		setPixelColorBrightness_LineBawah2(dPos, color, Invert_Bright);
		setPixelColorBrightness_LineAtas1(dPos, color, Invert_Bright);
		setPixelColorBrightness_LineAtas2(dPos, color, Invert_Bright);
		/* Setting Pixel LED Ring 2 */
		// setPixelColorBrightness_Ring2(dPos, color, Invert_Bright);
	}
	else
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_LineBawah1(dPos, color, brightness);
		setPixelColorBrightness_LineBawah2(dPos, color, brightness);
		setPixelColorBrightness_LineAtas1(dPos, color, brightness);
		setPixelColorBrightness_LineAtas2(dPos, color, brightness);
		/* Setting Pixel LED Ring 2 */
		// setPixelColorBrightness_Ring2(dPos, color, brightness);
	}
}

void loadVisual4(uint8_t dir, uint8_t dynamic_pos, uint32_t color, uint8_t brightness)
{
	uint8_t dPos = dynamic_pos;
	uint8_t dPos2 = (dynamic_pos + halfRing);
	uint8_t Invert_Bright = (255 - brightness);

	if (dPos2 >= ledRing1max)
		dPos2 = (dPos2 - ledRing1max);

	clearBuf_led();
	if (!dir)
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_Ring1(dPos, color, Invert_Bright);
		setPixelColorBrightness_Ring1(dPos2, color, Invert_Bright);
		
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColorBrightness_Ring2(dPos, color, Invert_Bright);
		setPixelColorBrightness_Ring2(dPos2, color, Invert_Bright);
#endif
	}
	else
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_Ring1(dPos, color, brightness);
		setPixelColorBrightness_Ring1(dPos2, color, brightness);
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColorBrightness_Ring2(dPos, color, brightness);
		setPixelColorBrightness_Ring2(dPos2, color, brightness);
#endif
	}
	for (char i = 0; i < 4; i++)
	{
		dPos++;
		if (dPos >= ledRing1max)
			dPos = 0;
		dPos2++;
		if (dPos2 >= ledRing1max)
			dPos2 = (dPos2 - ledRing1max);

		/* Setting Pixel LED Ring 1 */
		setPixelColor_Ring1(dPos, color);
		setPixelColor_Ring1(dPos2, color);
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColor_Ring2(dPos, color);
		setPixelColor_Ring2(dPos2, color);
#endif
	}
	if (dir)
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_Ring1(dPos, color, Invert_Bright);
		setPixelColorBrightness_Ring1(dPos2, color, Invert_Bright);
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColorBrightness_Ring2(dPos, color, Invert_Bright);
		setPixelColorBrightness_Ring2(dPos2, color, Invert_Bright);
#endif
	}
	else
	{
		/* Setting Pixel LED Ring 1 */
		setPixelColorBrightness_Ring1(dPos, color, brightness);
		setPixelColorBrightness_Ring1(dPos2, color, brightness);
#if (ringCount > 1)
		/* Setting Pixel LED Ring 2 */
		setPixelColorBrightness_Ring2(dPos, color, brightness);
		setPixelColorBrightness_Ring2(dPos2, color, brightness);
#endif
	}
}
/* =================================================================================== */
