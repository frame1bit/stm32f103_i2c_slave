
/**
 *  Animation_Style.c
 *  Modif & Optimize by Bria on 16 September 2020.
 *  yeramias.bria@gmail.com
 *  Source Code User Main Program Animation Style.
 */

#include "Animation_Style.h"
#include "app_config.h"

#define	GET_TICK()	HAL_GetTick()

/* Private Defined. ============================================== */
#define TimeHold_Peak (15)
#define TH_BitMusik_MAX (20)
#define TH_BitMusik_MIN (7)
#define Tik_Kedip (CountKedip_VOL * 2)
#define Bright_Speed ((LED_BRIGHTNESS_DEFAULT/*256*/ * Tik_Kedip * TimeUpdate_LED) / TimeOut_DisplayVOL)

volatile uint32_t Cnt_TH_BitMusik = 0;
volatile uint32_t TH_BitMusik = TH_BitMusik_MIN;

/* Declerasi Variable Params Animasi Volume Style. */
hStyleAnim_Volume pStyleAnimVol =
	{
		.Volume = {10, 0, 0, 0},
		.Bass = {10, 0, 0, 0},
		.FlagKedip = 0,
		.LevelVol = 50,
		.timeOut = TimeOut_DisplayVOL,
	};

DisplayProperty_t dispProp = 
{
	.blink_speed = 20,
	.bright_static = 127,
	.color = 0,
	.center_color = 0,
	.flag_blink = 0,
};

/* External Params. variable mode control.*/
extern int8_t mode_visual, lastMode_visual, saveLast_Mode;
extern uint16_t Cnt_WheelColor;
extern uint32_t warna;
extern uint16_t timeChange_potition;
extern short height;
extern volatile uint8_t NoSignal;

uint32_t modeColor[halfRing + 1];
uint8_t mapPixel_LED[count_led + 1];

uint8_t i = 0, dir = 0, speed_up = 0;
int8_t loadPos = 0;

short VU_Level = 1;
volatile uint8_t deb = 0;
volatile int16_t terang = 0;

volatile uint8_t putar = 0;
volatile uint8_t Cnt_Speed = 0;
extern volatile uint32_t temp_var;

uint8_t timerGanti_mode = 0;
uint32_t lastTime_mode = 0, Cnt_timeCh_Color = 0, timeDisplayVol_Cnt = 0;

void reloadTime_DispVol(void)
{
	timeDisplayVol_Cnt = GET_TICK();
}

void Auto_changeColor(const uint16_t time)
{
	if (GET_TICK() - Cnt_timeCh_Color >= time)
	{
		Cnt_timeCh_Color = GET_TICK();

		Cnt_WheelColor++;
		if (Cnt_WheelColor < 38 || Cnt_WheelColor > 120)
			Cnt_WheelColor++;
		if (Cnt_WheelColor > 255)
			Cnt_WheelColor = 0;

			//		warna = Wh7eel(Cnt_WheelColor);
#ifdef BRIGHTNESS_ADC // JH 11 10 22
		temp_var = Wheel(Cnt_WheelColor);
#else
		warna = Wheel(Cnt_WheelColor);
#endif
	}
}

/**
 * @brief	set loadPos variable, just interaface to outside world
 * 			used before run one shoot animation to determine direction 
*/
void set_initial_pos(int8_t pos)
{
	loadPos = pos;
}

void set_default()
{
	i = 0;
	dir = 0;
	NoSignal = 1;
	timeChange_potition = 0;

#if 0
	if (mode_visual == 8)
	{
		for (uint8_t x = 0; x < ledRing1max; x++)
		{
			mapPixel_LED[x] = x;
			mapPixel_LED[ledRing1max + x] = (ledRing2max - 1) - x;
		}
	}
#endif

	//warna = Wheel(Cnt_WheelColor);
	
	clearBuf_led();
}

void updateMode_Anim()
{
	if (mode_visual != lastMode_visual)
	{
		set_default();
		lastMode_visual = mode_visual;
	}
}

void visualMode_0(void) /* Stengah lingkaran putar Patah-Patah. */
{
	if (height)
	{
		if (speed_up == 0)
		{
			speed_up = 1;
			timeChange_potition += 10;
			if (timeChange_potition > TimeOutDirect_Anim)
			{
				dir = ~dir;
				timeChange_potition = 0;
			}
		}
	}

	if (speed_up)
	{
		terang += 380;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;
			Cnt_Speed++;

			if (terang > 511)
			{
				if (dir)
					loadPos--;
				else
					loadPos++;
				Cnt_Speed++;
			}
		}

		if (Cnt_Speed > 18)
		{
			Cnt_Speed = 0;
			speed_up = 0;
		}
	}
	else
	{
		terang += 20;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;
		}
	}

	terang &= 0xFF;

	if (loadPos >= ledRing1max)
		loadPos = 0;
	else if (loadPos < 0)
		loadPos = ledRing1max - 1;

	loadVisual3(dir, loadPos, warna, terang);
}
void visualMode_11(void)
{
	static uint8_t fChangeDirection = 0;
	if (mode_visual == 1)
	{
		dir = 0;
	}
	else if (mode_visual == 2)
	{
		dir = 1;
	}
	if (height)
	{
		if (speed_up == 0)
		{
			speed_up = 1;
			timeChange_potition += 10;
			if (timeChange_potition > TimeOutDirect_Anim)
			{
				dir = ~dir;
				timeChange_potition = 0;
			}
		}
	}

	if (speed_up)
	{
		terang += 380;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;
			Cnt_Speed++;

			if (terang > 511)
			{
				if (dir)
					loadPos--;
				else
					loadPos++;
				Cnt_Speed++;
			}
		}

		if (Cnt_Speed > 18)
		{
			Cnt_Speed = 0;
			speed_up = 0;
		}
	}
	else
	{
		terang += 20;
		if (terang > 255)
		{
			if (dir)
			{

				loadPos--;
			}
			else
			{
				loadPos++;
			}
		}
	}

	if ((loadPos >= lineBawah1max || loadPos < 0))
	{
		if (mode_visual == 3)
		{
			fChangeDirection++;
		}
	}

	terang &= 0xFF;

	if (loadPos >= lineBawah1max)
	{
		loadPos = 0;
	}
	else if (loadPos < 0)
	{
		loadPos = lineBawah1max - 1;
	}

	loadVisual11(dir, loadPos, warna, terang);

	if ((fChangeDirection > 1)) // JH 07 10 22
	{
		dir = ~dir;
		fChangeDirection = 0;
	}
}

void visualMode_12(void) /* Stengah lingkaran putar Patah-Patah. */
{
	if (height)
	{
		if (speed_up == 0)
		{
			speed_up = 1;
			timeChange_potition += 10;
			if (timeChange_potition > TimeOutDirect_Anim)
			{
				dir = ~dir;
				timeChange_potition = 0;
			}
		}
	}

	if (speed_up)
	{
		terang += 380;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;
			Cnt_Speed++;

			if (terang > 511)
			{
				if (dir)
					loadPos--;
				else
					loadPos++;
				Cnt_Speed++;
			}
		}

		if (Cnt_Speed > 18)
		{
			Cnt_Speed = 0;
			speed_up = 0;
		}
	}
	else
	{
		terang += 20;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;
		}
	}

	terang &= 0xFF;

	if (loadPos >= lineBawah1max)
		loadPos = 0;
	else if (loadPos < 0)
		loadPos = lineBawah1max - 1;

	loadVisual11(!dir, ((lineBawah1max - 1) - loadPos), warna, terang);
}
void visualMode_1(void) /* Stgh lingkaran kedip Ganti-gantian. */
{
	if (height)
	{
		if (speed_up == 0)
		{
			dir += 5;
			speed_up = 1;
		}
		//		/* Deteksi Bit Musik untuk Ubah Animasi Lampu sesuai Bit Musik */
		//		TH_BitMusik = Cnt_TH_BitMusik;
		//		if(Cnt_TH_BitMusik < TH_BitMusik_MIN) TH_BitMusik = TH_BitMusik_MIN;
		//		Cnt_TH_BitMusik = 0;
	}
	//	else
	//	{
	//		if(Cnt_TH_BitMusik < TH_BitMusik_MAX) Cnt_TH_BitMusik++;
	//	}

	if (speed_up)
	{
		if (++putar > TimeHold_Peak)
		{
			putar = 0;
			speed_up = 0;
		}
	}

	if (dir > 9)
		dir = dir - 9;
	if (NoSignal)
		dir = 0;

	clearBuf_led();
	switch (dir)
	{
	case 0:
	{
		setPixelColor_allRing1(0, ledRing1max, warna); // full
		setPixelColor_allRing2(0, ledRing1max, warna); // full
	}
	break;
	case 1:
	{
		setPixelColor_allRing1(1, halfRing, warna); // kiri
		setPixelColor_allRing2(1, halfRing, warna); // kanan
	}
	break;
	case 2:
	{
		setPixelColor_allRing1(0, ledRing1max, warna); // full atas
	}
	break;
	case 3:
	{
		setPixelColor_allRing1(halfRing2 + 1, halfRing, warna); // atas
		setPixelColor_allRing2(halfRing2 + 1, halfRing, warna); // bawah
	}
	break;
	case 4:
	{
		setPixelColor_allRing2(0, ledRing1max, warna); // full bawah
	}
	break;
	case 5:
	{
		setPixelColor_allRing1(halfRing + 1, halfRing, warna); // kanan
		setPixelColor_allRing2(halfRing + 1, halfRing, warna); // kiri
	}
	break;
	case 6:
	{
		setPixelColor_allRing1(halfRing2 + 1, halfRing, warna); // atas
	}
	break;
	case 7:
	{
		setPixelColor_allRing2(halfRing2 + 1, halfRing, warna); // bawah
	}
	break;
	case 8:
	{
		setPixelColor_allRing1(1, halfRing, warna); // atas kiri
	}
	break;
	case 9:
	{
		setPixelColor_allRing2(1, halfRing, warna); // bawah kiri
	}
	break;
	}
}

void visualMode_2(void) /* VUmeter hadapan (RGB) (jam 9 & 3)/(jam 3 & 9) Refren EDEN Style. */
{
	int32_t tempSpeed = 0;
	int8_t pos = 0, lPos = 0;

	deb = Cnt_WheelColor;
	for (i = 0; i < halfRing; i++)
	{
		modeColor[i] = Wheel(deb);
		deb += 3;
	}

	if (height)
	{
		if (putar == 0)
		{
			speed_up = 1;
			putar = 1;
		}

		/* Deteksi Bit Musik untuk Ubah Animasi Lampu sesuai Bit Musik */
		TH_BitMusik = Cnt_TH_BitMusik;
		if (Cnt_TH_BitMusik < TH_BitMusik_MIN)
			TH_BitMusik = TH_BitMusik_MIN;
		Cnt_TH_BitMusik = 0;
	}
	else
	{
		if (Cnt_TH_BitMusik < TH_BitMusik_MAX)
			Cnt_TH_BitMusik++;
	}

	if (NoSignal) /* jika tdk ada signal input */
	{
		VU_Level = (halfRing2 + 2);
		terang = 80;
	}
	else /* jika ada signal input */
	{
		if (speed_up)
		{
			terang += 380;
			if (terang > 255)
			{
				if (terang > 511)
					VU_Level++;
				if (++VU_Level > halfRing)
					speed_up = 0;
			}
			terang &= 0xFF;
		}
		else
		{
			tempSpeed = map(TH_BitMusik, TH_BitMusik_MIN, TH_BitMusik_MAX, 360, 255);
			terang += tempSpeed;
			if (terang > 255)
			{
				if (terang > 511)
					VU_Level--;
				if (--VU_Level < 1)
					VU_Level = 1;
			}
			if (VU_Level < 3)
				putar = 0;

			terang &= 0xFF;
			terang = 255 - terang;
		}
	}

	if (VU_Level <= 1)
	{
		if (timeChange_potition >= 400) // time == 8s
		{
			dir = ~dir;
			timeChange_potition = 0;
			clearBuf_led();
		}
	}

	for (i = 0; i < halfRing; i++)
	{
		if (!dir)
		{
			lPos = (halfRing2 + 1) + i;
			pos = (halfRing2 - i);
			if (pos < 0)
				pos = ledRing1max + pos;
		}
		else
		{
			lPos = (halfRing + halfRing2) - i;
			pos = (halfRing + halfRing2 + 1) + i;
			if (pos > ledRing1max)
				pos = pos - ledRing1max;
		}

		if (i < VU_Level)
		{
			/* Setting Pixel Ring 1 */
			setPixelColor_Ring1(pos, modeColor[i]);
			setPixelColor_Ring1(lPos, modeColor[i]);
			/* Setting Pixel Ring 2 */
			setPixelColor_Ring2(pos, modeColor[i]);
			setPixelColor_Ring2(lPos, modeColor[i]);
		}
		else
		{
			uint32_t Color_t = 0;
			if (i == VU_Level) /* Buat Degredasi Brightnesst warna */
			{
				if (i > 1)
					Color_t = ws2812_color_Brightness(modeColor[i], terang);
			}
			/* Setting Pixel Ring 1 */
			setPixelColor_Ring1(pos, Color_t);
			setPixelColor_Ring1(lPos, Color_t);
			/* Setting Pixel Ring 2 */
			setPixelColor_Ring2(lPos, Color_t);
		}
	}
}

void visualMode_3(void) /* VUmeter hadapan (RGB) (jam 12 & 6) JBL Style. */
{
	int32_t tempSpeed = 0;
	deb = Cnt_WheelColor;
	for (i = 0; i < halfRing; i++)
	{
		modeColor[i] = Wheel(deb);
		deb += 3;
	}

	if (height)
	{
		if (putar == 0)
		{
			speed_up = 1;
			putar = 1;
		}

		/* Deteksi Bit Musik untuk Ubah Animasi Lampu sesuai Bit Musik */
		TH_BitMusik = Cnt_TH_BitMusik;
		if (Cnt_TH_BitMusik < TH_BitMusik_MIN)
			TH_BitMusik = TH_BitMusik_MIN;
		Cnt_TH_BitMusik = 0;
	}
	else
	{
		if (Cnt_TH_BitMusik < TH_BitMusik_MAX)
			Cnt_TH_BitMusik++;
	}

	if (NoSignal) /* jika tdk ada signal input */
	{
		VU_Level = (halfRing2 + 2);
		terang = 80;
	}
	else /* jika ada signal input */
	{
		if (speed_up)
		{
			terang += 380;
			if (terang > 255)
			{
				if (terang > 511)
					VU_Level++;
				if (++VU_Level > halfRing)
					speed_up = 0;
			}
			terang &= 0xFF;
		}
		else
		{
			tempSpeed = map(TH_BitMusik, TH_BitMusik_MIN, TH_BitMusik_MAX, 360, 255);
			terang += tempSpeed;
			if (terang > 255)
			{
				if (terang > 511)
					VU_Level--;
				if (--VU_Level < 1)
					VU_Level = 1;
			}
			if (VU_Level < 3)
				putar = 0;

			terang &= 0xFF;
			terang = 255 - terang;
		}
	}

	for (i = 0; i < halfRing; i++)
	{
		if (i < VU_Level)
		{
			/* Setting Pixel Ring 1 */
			setPixelColor_Ring1(halfRing - i, modeColor[i]);
			setPixelColor_Ring1((halfRing + 1) + i, modeColor[i]);
			/* Setting Pixel Ring 2 */
			setPixelColor_Ring2(halfRing - i, modeColor[i]);
			setPixelColor_Ring2((halfRing + 1) + i, modeColor[i]);
		}
		else
		{
			uint32_t Color_t = 0;
			if (i == VU_Level) /* Buat Degredasi Brightnesst warna */
			{
				if (i > 1)
					Color_t = ws2812_color_Brightness(modeColor[i], terang);
			}
			/* Setting Pixel Ring 1 */
			setPixelColor_Ring1(halfRing - i, Color_t);
			setPixelColor_Ring1((halfRing + 1) + i, Color_t);
			/* Setting Pixel Ring 2 */
			setPixelColor_Ring2(halfRing - i, Color_t);
			setPixelColor_Ring2((halfRing + 1) + i, Color_t);
		}
	}
}



void visualMode_4(void) /* Shadow_Up-Down JBL style. */
{
	int8_t dPos1 = 0, chPos = 0;
	if (height)
	{
		if (speed_up == 0)
		{
			speed_up = 1;
			timeChange_potition += 11;
			if (timeChange_potition > TimeOutDirect_Anim) // time == 15s
			{
				dir = ~dir;
				timeChange_potition = 0;
			}
		}
	}

	if (speed_up)
	{
		terang += 290;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;

			if (++Cnt_Speed > 15)
			{
				Cnt_Speed = 0;
				speed_up = 0;
			}
		}
	}
	else
	{
		terang += 20;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;
		}
	}

	terang &= 0xFF;

	clearBuf_led();

	uint8_t Invert_terang = (255 - terang);

	if (loadPos >= ledRing1max)
		loadPos = 0;
	else if (loadPos < 0)
		loadPos = ledRing1max - 1;

	dPos1 = halfRing + loadPos;
	for (int8_t x = 0; x < ledRing1max; x++)
	{
		if (dPos1 >= (ledRing1max + halfRing))
			dPos1 -= ledRing1max;
		if (x == 0)
		{
			if (dir)
				ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, terang));
			else
				ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, Invert_terang));
		}
		if (x == 5)
		{
			if (dir)
				ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, Invert_terang));
			else
				ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, terang));
		}
		if (x > 0 && x < 5)
			ws2812_setPixelColor(dPos1 + 1, warna);
		dPos1++;
	}

	dPos1 = halfRing - loadPos;
	for (int8_t x = 0; x < ledRing1max; x++)
	{
		if (dPos1 < 0)
		{
			dPos1 += ledRing2max;
			chPos = 1;
		}
		if (chPos)
		{
			if (dPos1 < (ledRing2max - halfRing))
			{
				dPos1 -= ledRing1max;
				chPos = 0;
			}
		}
		if (x == 0)
		{
			if (dir)
				ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, terang));
			else
				ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, Invert_terang));
		}
		if (x == 5)
		{
			if (dir)
				ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, Invert_terang));
			else
				ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, terang));
		}
		if (x > 0 && x < 5)
			ws2812_setPixelColor(dPos1, warna);
		dPos1--;
	}
}

void visualMode_5(void) /* Putar Gelembung -> Black-shadow JBL style. */
{
	static uint32_t bitGlow = 0x000F1E3C;
	static uint32_t bitBrighT = 0x00081020;
	//	static uint32_t bitBrighB = 0x0008102;
	static uint32_t bitBrighB = 0x00010204;
	static int8_t dPos = 0;

	if (height)
	{
		if (speed_up == 0)
		{
			speed_up = 1;
			timeChange_potition += 10;
			if (timeChange_potition > 400)
			{
				dir = ~dir;
				timeChange_potition = 0;
			}
		}
	}

	if (speed_up)
	{
		terang += 150;
		if (terang > 255)
		{
			loadPos++;
			if (++Cnt_Speed > 5)
			{
				Cnt_Speed = 0;
				speed_up = 0;
			}
		}
	}
	else
	{
		terang += 12;
		if (terang > 255)
		{
			loadPos++;
		}
	}

	terang &= 0xFF;
	/* ========================== ********* ========================== */
	clearBuf_led();
	if (loadPos > halfRing)
		loadPos = 0;

	if (loadPos != dPos)
	{
		if (bitGlow & 0x00100000) /* geser bit posisi lampu */
		{
			bitGlow <<= 1;
			bitGlow |= 0x00000001;
		}
		else
			bitGlow <<= 1;

		if (bitBrighT & 0x00100000) /* geser bit brighnest TOP */
		{
			bitBrighT <<= 1;
			bitBrighT |= 0x00000001;
		}
		else
			bitBrighT <<= 1;

		if (bitBrighB & 0x00100000) /* geser bit brighnest BOTTOM */
		{
			bitBrighB <<= 1;
			bitBrighB |= 0x00000001;
		}
		else
			bitBrighB <<= 1;

		dPos = loadPos;
	}

	uint8_t Invert_terang = (255 - terang);

	uint32_t temp_ = bitGlow;
	uint32_t temp1_ = bitBrighT;
	uint32_t temp2_ = bitBrighB;
	for (int8_t x = 0; x <= halfRing; x++)
	{
		if (!dir)
		{
			if (temp_ & 0x00000001)
			{
				/* Setting Pixel Ring 1 */
				setPixelColor_Ring1(x, warna);
				setPixelColor_Ring1(ledRing1max - x, warna);

#if (ringCount > 1)
				/* Setting Pixel Ring 2 */
				setPixelColor_Ring2(x, warna);
				setPixelColor_Ring2(ledRing1max - x, warna);
#endif
			}
			temp_ >>= 1;

			if (temp1_ & 0x00000001)
			{
				/* Setting Pixel Ring 1 */
				setPixelColorBrightness_Ring1(x, warna, terang);
				setPixelColorBrightness_Ring1(ledRing1max - x, warna, terang);
#if (ringCount > 1)
				/* Setting Pixel Ring 2 */
				setPixelColorBrightness_Ring2(x, warna, terang);
				setPixelColorBrightness_Ring2(ledRing1max - x, warna, terang);
#endif
			}
			temp1_ >>= 1;

			if (temp2_ & 0x00000001)
			{
				/* Setting Pixel Ring 1 */
				setPixelColorBrightness_Ring1(x, warna, Invert_terang);
				setPixelColorBrightness_Ring1(ledRing1max - x, warna, Invert_terang);
#if (ringCount > 1)
				/* Setting Pixel Ring 2 */
				setPixelColorBrightness_Ring2(x, warna, Invert_terang);
				setPixelColorBrightness_Ring2(ledRing1max - x, warna, Invert_terang);
#endif
			}
			temp2_ >>= 1;
		}
		else
		{
			if (temp_ & 0x00100000)
			{
				/* Setting Pixel Ring 1 */
				setPixelColor_Ring1(x, warna);
				setPixelColor_Ring1(ledRing1max - x, warna);
#if (ringCount > 1)
				/* Setting Pixel Ring 2 */
				setPixelColor_Ring2(x, warna);
				setPixelColor_Ring2(ledRing1max - x, warna);
#endif
			}
			temp_ <<= 1;

			if (temp1_ & 0x00100000)
			{
				/* Setting Pixel Ring 1 */
				setPixelColorBrightness_Ring1(x, warna, terang);
				setPixelColorBrightness_Ring1(ledRing1max - x, warna, terang);
#if (ringCount > 1)
				/* Setting Pixel Ring 2 */
				setPixelColorBrightness_Ring2(x, warna, terang);
				setPixelColorBrightness_Ring2(ledRing1max - x, warna, terang);
#endif
			}
			temp1_ <<= 1;

			if (temp2_ & 0x00100000)
			{
				/* Setting Pixel Ring 1 */
				setPixelColorBrightness_Ring1(x, warna, Invert_terang);
				setPixelColorBrightness_Ring1(ledRing1max - x, warna, Invert_terang);
#if (ringCount > 1)
				/* Setting Pixel Ring 2 */
				setPixelColorBrightness_Ring2(x, warna, Invert_terang);
				setPixelColorBrightness_Ring2(ledRing1max - x, warna, Invert_terang);
#endif
			}
			temp2_ <<= 1;
		}
	}
}

void visualMode_6(void) /* Stengah lingkaran putar Patah-Patah bentuk 'S'. */
{
	if (height)
	{
		if (speed_up == 0)
		{
			speed_up = 1;
			timeChange_potition += 10;
			if (timeChange_potition > TimeOutDirect_Anim)
			{
				dir = ~dir;
				timeChange_potition = 0;
			}
		}
	}

	if (speed_up)
	{
		terang += 400;
		if (terang > 255)
		{
			if (terang > 511)
			{
				if (dir)
				{
					loadPos--;
					if (loadPos == halfRing || loadPos == (ledRing1max + halfRing))
						speed_up = 0;
				}
				else
				{
					loadPos++;
					if (loadPos == 1 || loadPos == ledRing1max)
						speed_up = 0;
				}
			}
			if (dir)
			{
				loadPos--;
				if (loadPos == halfRing || loadPos == (ledRing1max + halfRing))
					speed_up = 0;
			}
			else
			{
				loadPos++;
				if (loadPos == 1 || loadPos == ledRing1max)
					speed_up = 0;
			}
		}
	}
	else
	{
		terang += 20;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;
		}
	}

	terang &= 0xFF;

	if (loadPos >= ledRing2max)
		loadPos = 0;
	else if (loadPos < 0)
		loadPos = ledRing2max - 1;

	clearBuf_led();

	int8_t dPos = loadPos;
	uint8_t Invert_terang = (255 - terang);

	if (!dir)
		ws2812_setPixelColor(mapPixel_LED[dPos], ws2812_color_Brightness(warna, Invert_terang));
	else
		ws2812_setPixelColor(mapPixel_LED[dPos], ws2812_color_Brightness(warna, terang));
	for (char i = 0; i < halfRing; i++)
	{
		dPos++;
		if (dPos >= ledRing2max)
			dPos = 0;
		ws2812_setPixelColor(mapPixel_LED[dPos], warna);
	}
	if (dir)
		ws2812_setPixelColor(mapPixel_LED[dPos], ws2812_color_Brightness(warna, Invert_terang));
	else
		ws2812_setPixelColor(mapPixel_LED[dPos], ws2812_color_Brightness(warna, terang));
}

void visualMode_7(void) /* 1/4  (Disk) lingkarang mutar-mutar Patah-Patah. */
{
	if (height)
	{
		if (speed_up == 0)
		{
			speed_up = 1;
			timeChange_potition += 10;
			if (timeChange_potition > TimeOutDirect_Anim)
			{
				dir = ~dir;
				timeChange_potition = 0;
			}
		}
	}

	if (speed_up)
	{
		terang += 260;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;
			if (++Cnt_Speed >= 8)
			{
				Cnt_Speed = 0;
				speed_up = 0;
			}
		}
	}
	else
	{
		terang += 20;
		if (terang > 255)
		{
			if (dir)
				loadPos--;
			else
				loadPos++;
		}
	}

	terang &= 0xFF;
	if (loadPos >= ledRing1max)
		loadPos = 0;
	else if (loadPos < 0)
		loadPos = ledRing1max - 1;
	loadVisual4(dir, loadPos, warna, terang);
}

void visualMode_8(void) /* Random timer Ganti-ganti mode. */
{
	static uint8_t mode = 0;
	if (GET_TICK() - lastTime_mode > 999) /* setip 1 detik. */
	{
		lastTime_mode = GET_TICK();
		if (++timerGanti_mode > TimeOutRandom_Anim) /* jika waktu tercapai ganti mode animasi. */
		{
			timerGanti_mode = 0;
			if (++mode > 7)
				mode = 0;
			/* set to default */
			set_default();
			if (mode == 6)
			{
				for (uint8_t x = 0; x < ledRing1max; x++)
				{
					mapPixel_LED[x] = x;
					mapPixel_LED[ledRing1max + x] = (ledRing2max - 1) - x;
				}
			}
		}
	}
	switch (mode)
	{
	case 0:
		visualMode_0();
		break;
	case 1:
		visualMode_1();
		break;
	case 2:
		visualMode_2();
		break;
	case 3:
		visualMode_3();
		break;
	case 4:
		visualMode_4();
		break;
	case 5:
		visualMode_5();
		break;
	case 6:
		visualMode_6();
		break;
	case 7:
		visualMode_7();
		break;
	}
}

void visualMode_9(void) /* ambient nyala semua JMH 04 10 22 */
{
	setPixelColor_allRing1(0, ledRing1max, warna); // full
}

void visualMode_SingleColor(uint32_t RGB_Code) /* ambient nyala semua JMH 22 12 22 */
{
	setPixelColor_allRing1(0, ledRing1max, RGB_Code); // full
}

void visualMode_20(void) /* ambient nyala semua JMH 22 12 22 */
{
	setPixelColor_allRing1(0, ledRing1max, 0x00FF00); // full
}

void visualMode_21(void) /* ambient nyala semua JMH 22 12 22 */
{
	setPixelColor_allRing1(0, ledRing1max, 0x0000FF); // full
}

void visualMode_10(void) /* ambient nyala simple garis2  JMH 04 10 22 */
{
	static int PosLine1 = 5, PosLine2 = 5, PosLine3 = 13, PosLine4 = 13;
	static int DelayJH;
	if (DelayJH < 10)
	{
		DelayJH++;
	}
	else
	{
		DelayJH = 0;

		/****************Line Led Bawah**************/
		for (char i = 0; i < 10; i++)
			ws2812_setPixelColor(i, 0); // clear buffer untuk line bawah
		if (PosLine1 >= (lineBawah1max - 1))
		{
			PosLine1 = 0;
		}
		else
		{
			PosLine1++;
		}

		if (PosLine2 <= (lineBawah1max))
		{
			PosLine2 = (lineBawah2max - 1);
		}
		else
		{
			PosLine2--;
		}

		setPixelColor_LineBawah1(PosLine1, warna);
		setPixelColor_LineBawah2(PosLine2, warna);
		////////////////////////////////////////////////////////////
		/****************Line Led Atas**************/
		if (PosLine1 % 2 == 0)
		{
			for (char i = 10; i < count_led; i++)
				ws2812_setPixelColor(i, 0); // clear buffer untuk line atas
			if (PosLine3 >= (lineAtas1max - 1))
			{
				PosLine3 = lineBawah2max;
			}
			else
			{
				PosLine3++;
			}

			if (PosLine4 <= (lineAtas1max))
			{
				PosLine4 = (lineAtas2max - 1);
			}
			else
			{
				PosLine4--;
			}
			setPixelColor_LineAtas1(PosLine3, warna);
			setPixelColor_LineAtas2(PosLine4, warna);
		}
	}
}

void visualMode_13(void) /* VU Meter Lurus JH 20 01 23 */
{
	uint8_t n; // JH 16 11 22
	int32_t tempSpeed = 0;
	deb = Cnt_WheelColor;
	for (i = 0; i < count_led; i++) // JH 16 11 22
	{
		modeColor[i] = Wheel(deb);
		deb += 3;
	}

	if (height)
	{
		if (putar == 0)
		{
			speed_up = 1;
			putar = 1;
		}

		/* Deteksi Bit Musik untuk Ubah Animasi Lampu sesuai Bit Musik */
		TH_BitMusik = Cnt_TH_BitMusik;
		if (Cnt_TH_BitMusik < TH_BitMusik_MIN)
			TH_BitMusik = TH_BitMusik_MIN;
		Cnt_TH_BitMusik = 0;
	}
	else
	{
		if (Cnt_TH_BitMusik < TH_BitMusik_MAX)
			Cnt_TH_BitMusik++;
	}

	if (NoSignal) /* jika tdk ada signal input */
	{
		VU_Level = halfRing; //(halfRing_Circle_1 + 2);
		terang = 150;
	}
	else /* jika ada signal input */
	{
		if (speed_up)
		{
			terang += 380;
			if (terang > 255)
			{
				if (terang > 511)
					VU_Level++;
				if (++VU_Level > count_led) // JH 16 11 22
					speed_up = 0;
			}
			terang &= 0xFF;
		}
		else
		{
			tempSpeed = map(TH_BitMusik, TH_BitMusik_MIN, TH_BitMusik_MAX, 360, 255);
			terang += tempSpeed;
			if (terang > 255)
			{
				if (terang > 511)
					VU_Level--;
				if (--VU_Level < 1)
					VU_Level = 1;
			}
			if (VU_Level < 3)
				putar = 0;

			terang &= 0xFF;
			terang = 255 - terang;
		}
	}

	for (i = 0; i < count_led; i++)
	{
		if (i < VU_Level)
		{
			/* Setting Pixel Ring 1 */
			ws2812_setPixelColor(i, modeColor[i]); // modif mapping JH 15 11 12 //JH 23 11 22
												   // setPixelColor_Ring1((halfRing_Sauron) + i, modeColor[i]);
		}
		else
		{
			uint32_t Color_t = 0;
			if (i == VU_Level) /* Buat Degredasi Brightnesst warna */
			{
				if (i > 1)
					Color_t = ws2812_color_Brightness(modeColor[i], terang);
			}
			/* Setting Pixel Ring 1 */
			ws2812_setPixelColor(i, Color_t); // modif mapping JH 15 11 12
											  // setPixelColor_Ring1((halfRing_Sauron) + i, Color_t);
		}
	}
}


void animVolume(void) /* Animasi Volume. */
{
	/* ======== Fungsi Time Kedip Vol 1 ================ */
	if (pStyleAnimVol.FlagKedip & flag_maxVol_1)
	{
		/* jika vol max, time out display volume dicepatin */
		//		pStyleAnimVol.timeOut = 1300;
		if (!(pStyleAnimVol.Volume.CountKedip & 0x01))
		{
			pStyleAnimVol.Volume.BrighKedip += Bright_Speed;
			if (pStyleAnimVol.Volume.BrighKedip > LED_BRIGHTNESS_DEFAULT)
			{
				pStyleAnimVol.Volume.BrighKedip = LED_BRIGHTNESS_DEFAULT;
				pStyleAnimVol.Volume.CountKedip++;
			}
		}
		else
		{
			pStyleAnimVol.Volume.BrighKedip -= Bright_Speed;
			if (pStyleAnimVol.Volume.BrighKedip < 10)
			{
				pStyleAnimVol.Volume.BrighKedip = 0;
				pStyleAnimVol.Volume.CountKedip++;
			}
		}

		if (pStyleAnimVol.Volume.CountKedip >= Tik_Kedip)
		{
			pStyleAnimVol.FlagKedip &= ~flag_maxVol_1;
			pStyleAnimVol.Volume.BrighKedip = 0;
			pStyleAnimVol.Volume.CountKedip = 0;
		}
	}
	/* ======== Fungsi Time Kedip Vol 2 ================ */
	if (pStyleAnimVol.FlagKedip & flag_maxVol_2)
	{
		/* jika vol max, time out display volume dicepatin */
		//		pStyleAnimVol.timeOut = 1300;
		if (!(pStyleAnimVol.Bass.CountKedip & 0x01))
		{
			pStyleAnimVol.Bass.BrighKedip += Bright_Speed;
			if (pStyleAnimVol.Bass.BrighKedip > 255)
			{
				pStyleAnimVol.Bass.BrighKedip = 255;
				pStyleAnimVol.Bass.CountKedip++;
			}
		}
		else
		{
			pStyleAnimVol.Bass.BrighKedip -= Bright_Speed;
			if (pStyleAnimVol.Bass.BrighKedip < 10)
			{
				pStyleAnimVol.Bass.BrighKedip = 0;
				pStyleAnimVol.Bass.CountKedip++;
			}
		}

		if (pStyleAnimVol.Bass.CountKedip >= Tik_Kedip)
		{
			pStyleAnimVol.FlagKedip &= ~flag_maxVol_2;
			pStyleAnimVol.Bass.BrighKedip = 0;
			pStyleAnimVol.Bass.CountKedip = 0;
		}
	}
	/* ========================= End Code Kedip-kedip ============================ */

	int8_t dpos = 0;
	for (i = 0; i < ledRing1max; i++)
	{
		/* ring 1 (Volume) */
		/* degredasi warna level volume 1 */
#if !request_VolColor
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 55, 120));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 120, 60));
#elif request_VolColor == 1
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 240, 168));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 168, 240));
#elif request_VolColor == 2
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 220, 165));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 165, 220));
#elif request_VolColor == 3
		warna = 165;
#elif request_VolColor == 4
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 205, 168));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 168, 205));
#elif request_VolColor == 5
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 205, 168));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 168, 205));
#elif request_VolColor == 6 // JH 27 01 23 orange
		warna = LED_COLOR_CHANGE_VOLUME;//0x30FF00;
#else
#warning "Request Mode Anim-Volume tidak ditemukan"
#endif
		/* ============================================================================ */

		if (pStyleAnimVol.FlagKedip & flag_maxVol_1) /* jika Volume-1 100% maka kedip-kedip */
		{
			ws2812_setPixelColor(i, ws2812_color_Brightness(warna, (uint8_t)pStyleAnimVol.Volume.BrighKedip));
		}
		else
		{
			if (i < pStyleAnimVol.Volume.Level)
				ws2812_setPixelColor(i, ws2812_color_Brightness(warna, LED_BRIGHTNESS_DEFAULT));
			else if (i == pStyleAnimVol.Volume.Level)
				ws2812_setPixelColor(i, ws2812_color_Brightness(warna, pStyleAnimVol.Volume.Shadow));
			else
				ws2812_setPixelColor(i, 0);
		}

#if (ringCount > 1)
		/* ring 2 (Super Bass) */
		/* degredasi warna level volume 2 */
#if !request_VolColor
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 120, 55));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 60, 120));
#elif request_VolColor == 1
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 168, 240));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 240, 168));
#elif request_VolColor == 2
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 165, 220));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 220, 165));
#elif request_VolColor == 3
		warna = 165;
#elif request_VolColor == 4
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 168, 205));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 205, 168));
#elif request_VolColor == 5
		if (i <= halfRing)
			warna = Wheel(map(i, 0, halfRing, 205, 168));
		else
			warna = Wheel(map(i, halfRing, ledRing1max, 168, 205));
#else
#warning "Request Mode Anim-Volume tidak ditemukan"
#endif
		/* ============================================================================ */


		dpos = (ledRing1max + halfRing + 1) + i;
		if (dpos >= ledRing2max)
			dpos -= ledRing1max;
		if (pStyleAnimVol.FlagKedip & flag_maxVol_2) /* jika Volume-2 100% maka kedip-kedip */
		{
			ws2812_setPixelColor(dpos, ws2812_color_Brightness(warna, (uint8_t)pStyleAnimVol.Bass.BrighKedip));
		}
		else
		{
			if (i < pStyleAnimVol.Bass.Level)
				ws2812_setPixelColor(dpos, warna);
			else if (i == pStyleAnimVol.Bass.Level)
				ws2812_setPixelColor(dpos, ws2812_color_Brightness(warna, pStyleAnimVol.Bass.Shadow));
			else
				ws2812_setPixelColor(dpos, 0);
		}
#endif /*#if (ringCount > 1)*/

	}

	/* TimeOut Display Volume Handler =================================================	*/
	if (!pStyleAnimVol.FlagKedip)
	{
		if (GET_TICK() - timeDisplayVol_Cnt > pStyleAnimVol.timeOut) /* time-out Anim Level Volume Display */
		{
			// pStyleAnimVol.FlagKedip &= ~( flag_maxVol_2 | flag_maxVol_1);
			// pStyleAnimVol.timeOut = TimeOut_DisplayVOL;
			mode_visual = saveLast_Mode;
			clearBuf_led();
		}
	}
}

/***
 * @brief	set animation blink
 * @note	no display property needed
*/
void cp_animation_blink(uint8_t speed)
{
	static uint8_t blink_state = 0;
	static int16_t bright_blink;

	if (blink_state)
	{
		bright_blink += speed;
		if (bright_blink > LED_BRIGHTNESS_DEFAULT)
		{
			bright_blink = LED_BRIGHTNESS_DEFAULT;
			blink_state = 0;
		}
	}
	else
	{
		bright_blink -= speed;
		if (bright_blink < 10)
		{
			bright_blink = 0;
			blink_state = 1;
		}
	}

	for (i = 0; i < ledRing1max; i++)
	{
		ws2812_setPixelColor(i, 
							ws2812_color_Brightness(dispProp.color, (uint8_t) bright_blink)
							);
	}
}

/***
 * @brief animation on half blink, used to indicate speaker mode
 * mix mode: 	red blink left and right
 * left mode: 	red blink left
 * right mode: 	red blink right
 * 
 * @param blink_state: initial state used to blink count
 * @param mode: current mode (speaker mode)
 * @param color_static: static color used for current function (color not blink)
 * 
*/
void cp_animation_half_blink(uint8_t *blink_state, uint8_t mode, uint32_t color_static)
{
	#define	BLINK_NUM	4
	#define	SPEED		20
	static int16_t bright_blink;
	static uint32_t dmic_bright[2] = {0};

	if (*blink_state & 0x01)
	{
		bright_blink += SPEED;
		if (bright_blink > 255)
		{
			bright_blink = 255;
			*blink_state += 1;
		}
	}
	else
	{
		bright_blink -= SPEED;
		if (bright_blink < 10)
		{
			bright_blink = 0;
			*blink_state += 1;
		}
	}

	/** beahviour , left, right alternatively */
	if (mode==0) 
	{
		if (*blink_state & 0x01)
		{
			dmic_bright[0] = ws2812_color_Brightness(LED_COLOR_RED, (uint8_t) LED_BRIGHTNESS_DEFAULT);
			dmic_bright[1] = ws2812_color_Brightness(color_static, (uint8_t) LED_BRIGHTNESS_DEFAULT);
		}
		else
		{
			dmic_bright[1] = ws2812_color_Brightness(LED_COLOR_RED, (uint8_t) LED_BRIGHTNESS_DEFAULT);
			dmic_bright[0] = ws2812_color_Brightness(color_static, (uint8_t) LED_BRIGHTNESS_DEFAULT);
		}
	}
	else if (mode==1)  /** blink in one side TODO: adjust position based on speaker */
	{
		dmic_bright[0] = ws2812_color_Brightness(LED_COLOR_RED, (uint8_t) bright_blink);
		dmic_bright[1] = ws2812_color_Brightness(color_static, (uint8_t) LED_BRIGHTNESS_DEFAULT);
	}
	else if (mode==2) /** blink in one side TODO: adjust position based on speaker */
	{
		dmic_bright[0] = ws2812_color_Brightness(color_static, (uint8_t) LED_BRIGHTNESS_DEFAULT);
		dmic_bright[1] = ws2812_color_Brightness(LED_COLOR_RED, (uint8_t) bright_blink);
	}
	else
	{/*reserved*/}

	for (i = 0; i < ledRing1max; i++)
	{
		if (i < halfRing)
		{
			ws2812_setPixelColor(i, dmic_bright[0]);
		}
		else if (i < ledRing1max)
		{
			ws2812_setPixelColor(i, dmic_bright[1]);
		}
	}

	if (*blink_state > BLINK_NUM){
		/** go to previous visual mode */
	 	mode_visual = saveLast_Mode;
	 	clearBuf_led();
	}

}

/**
 * centerpiece led set to static color 
 * 
*/
void cp_set_single_color(uint32_t rgb_code)
{
	setPixelColor_allRing1(0, ledRing1max, rgb_code);
}

void cp_set_single_color_brightness(uint32_t color)
{
	for(i =0; i < ledRing1max; i++)
	{
		ws2812_setPixelColor(i, ws2812_color_Brightness(color, dispProp.bright_static));
	}
}


/**
 * @brief	set dynamic color with different middle color
 * @param	color: basic color
 * @param	center: middle color 
 * @note	set behaviour on dispProp !
*/
void cp_set_dynamic_color(void)
{
	static int16_t bright_blink;
	static uint8_t blink_state = 0;

	if (dispProp.flag_blink)
	{
		if (blink_state)
		{
			bright_blink += dispProp.blink_speed;
			if (bright_blink > dispProp.bright_static)
			{
				bright_blink = dispProp.bright_static;
				blink_state = 0;
			}
		}
		else
		{
			bright_blink -= dispProp.blink_speed;
			if (bright_blink < 10)
			{
				bright_blink = 0;
				blink_state = 1;
			}
		}
	}

	for(i = 0; i < ledRing1max; i += 1)
	{
		if (i < (halfRing-CENTER_LED_NUM) )
		{
			ws2812_setPixelColor(i, ws2812_color_Brightness(dispProp.color, dispProp.bright_static));
		}
		else if (i >= (halfRing-CENTER_LED_NUM) && i <= (halfRing+CENTER_LED_NUM))
		{
			if (dispProp.flag_blink)
			{
				ws2812_setPixelColor(i, ws2812_color_Brightness(dispProp.center_color, bright_blink));
			}
			else
			{
				ws2812_setPixelColor(i, ws2812_color_Brightness(dispProp.center_color, dispProp.bright_static));
			}
		}
		else if (i < ledRing1max)
		{
			ws2812_setPixelColor(i, ws2812_color_Brightness(dispProp.color, dispProp.bright_static));
		}
	}
}

/**
 * animasi ring wiper , step from 0 to max ledRing1Max
 * and back from ledRing1Max to min
*/
void cp_visual_mode0(void)
{
	int8_t dPos1 = 0, chPos = 0;
	uint8_t Invert_Bright, dPos;

	clearBuf_led();
	
	terang += 100;
	if (terang > 255)
	{
		loadPos = (loadPos + 1);
		if (loadPos == ledRing1max){
			dir = ~dir; 
			loadPos = 0;
		}
		terang = 0x0;
	}

	terang &= 0xff;

	dPos = loadPos;
	Invert_Bright = (255 - terang);
	if (dir)
	{
		setPixelColor_allRing1(loadPos, ledRing1max-loadPos,warna);
	}
	else
	{
		setPixelColor_allRing1(ledRing1max-loadPos, loadPos, warna);
	}
}

/**
 * centerpiece animation
 * led half ring running around 
*/
void cp_visual_mode1(void)
{
	uint8_t dPos, Invert_Bright;

	terang += 20;
	if (terang > 255)
	{
		if (dir)
			loadPos--;
		else
			loadPos++;
	}

	terang &= 0xFF;

	if (loadPos >= ledRing1max)
		loadPos = 0;
	else if (loadPos < 0)
		loadPos = ledRing1max - 1;

	dPos = loadPos;
	Invert_Bright = (255 - terang);

	clearBuf_led();
	if (!dir)
	{
		setPixelColorBrightness_Ring1(dPos, warna, Invert_Bright);
	}
	else
	{
		setPixelColorBrightness_Ring1(dPos, warna, terang);
	}
	for (char i = 0; i < (halfRing - 2); i++)
	{
		dPos++;
		if (dPos >= ledRing1max)
			dPos = 0;
		setPixelColor_Ring1(dPos, warna);
	}
	if (dir)
	{
		setPixelColorBrightness_Ring1(dPos, warna, Invert_Bright);
	}
	else
	{
		setPixelColorBrightness_Ring1(dPos, warna, terang);
	}
}

/***
 * centerpiece animation mode 2
 * runing 1 led to the right and back to left (min to max and max to min)
 * total 3 led with led middle static 
 * 2 led for smooth effect transition (fading)
*/
void cp_visual_mode2(void)
{
	uint8_t bright_invert, dPos;

	terang += 35;
	if(terang > LED_BRIGHTNESS_DEFAULT)
	{
		if (dir)
			loadPos--;
		else
			loadPos++;
	}

	if (loadPos >= ledRing1max-1) {
		loadPos = ledRing1max - 1;
		dir = 1;
	}
	else if (loadPos <= 0) {
		loadPos = 0;
		dir = 0;
	}

	clearBuf_led();
	//terang &= /*0xff*/LED_BRIGHTNESS_DEFAULT;
	if (terang > LED_BRIGHTNESS_DEFAULT) terang = 0;
	bright_invert = LED_BRIGHTNESS_DEFAULT - terang;
	dPos = loadPos;
	if (!dir) {
		setPixelColorBrightness_Ring1(dPos, dispProp.color, bright_invert);
	}
	else
	{
		setPixelColorBrightness_Ring1(dPos, dispProp.color, terang);
	}
	
	dPos += 1;
	setPixelColor_Ring1(dPos, dispProp.color);
	dPos += 1;
	if (dPos > ledRing1max-1) /*dPos = ledRing1max-1*/return;
	if (dir)
	{
		setPixelColorBrightness_Ring1(dPos, dispProp.color, bright_invert);
	}
	else
	{
		setPixelColorBrightness_Ring1(dPos, dispProp.color, terang);
	}
}

/**
 * centerpiece animation
 * loading animation 
 * led from led-0 to max_led
 * led from led-0 to max_led-1
 * led from led-0 to max_led-2
 * ...
 * dst
 * 
*/
void cp_visual_mode3(void)
{
	static uint8_t dPos = ledRing1max-1;
	uint8_t bright_invert;

	terang = (terang + 100);

	if (terang > 255)
	{
		terang = 0;
		loadPos += 1;
		if(loadPos > dPos-1) {
			loadPos = 0;
			dPos--;
			if (dPos <= 0) dPos = ledRing1max-1;
		}
		else if (loadPos <= 0) {
			loadPos = 0;
		}
	}
	clearBuf_led();

	ws2812_setPixelColor(loadPos, LED_COLOR_ERROR);
	for(i = dPos; i < ledRing1max; i++)
	{
		ws2812_setPixelColor(i, LED_COLOR_ERROR);
	}
}

/**
 * centerpiece animation
 * running halRing (ledRing1Max/2)
*/
void cp_visual_mode4(uint32_t color)
{
	terang += 50;
	if (terang > LED_BRIGHTNESS_DEFAULT)
	{
		if (dir)
			loadPos--;
		else
			loadPos++;
	}

	//terang &= 0xFF;
	if (terang > LED_BRIGHTNESS_DEFAULT) terang = 0;

	if (loadPos >= ledRing1max)
		loadPos = 0;
	else if (loadPos < 0)
		loadPos = ledRing1max - 1;

	loadVisual3(dir, loadPos, color, terang);
}


/**
 * centerpiece animation 
 * one-shoot
 * for enable or disable indication 
 * 
 * - half ring from rear to front (enable)
 * - half ring from front to rear (disable)
*/
void cp_visual_mode5(uint8_t direction)
{
	int8_t dPos1 = 0, chPos = 0;

	dir = direction;

	/** dir == 0 indicate something off
	 * and dir == 1 indicate something on
	*/
	if (dir) warna = LED_COLOR_RED;
	else warna = LED_COLOR_PURPLE;

	terang += 150;
	if (terang > 255)
	{
		if (dir)
			loadPos++;
		else
			loadPos--;
	}

	terang &= 0xFF;
	clearBuf_led();

	uint8_t Invert_terang = (255 - terang);

	dPos1 = halfRing + loadPos;
	for (int8_t x = 0; x < loadPos; x++)
	{
		if (x == 0)
		{
			if (dir)
				ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, Invert_terang));
			else
				ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, terang));
		}
		if (x == (halfRing/2))
		{
			if (dir)
				ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, terang));
			else
				ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, Invert_terang));
		}
		if (x > 0 && x < (halfRing/2))
			ws2812_setPixelColor(dPos1 + 1, warna);
		dPos1++;
	}

	dPos1 = halfRing - loadPos;
	for (int8_t x = 0; x < loadPos; x++)
	{
		if (x == 0)
		{
			if (dir)
				ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, Invert_terang));
			else
				ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, terang));
		}
		if (x == (halfRing/2))
		{
			if (dir)
				ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, terang));
			else
				ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, Invert_terang));
		}
		if (x > 0 && x < (halfRing/2))
			ws2812_setPixelColor(dPos1, warna);
		dPos1--;
	}

	if ( (!dir && loadPos <= 0) || (dir && loadPos >= halfRing-1) )
	 {
	 	/** go to previous visual mode */
	 	mode_visual = saveLast_Mode;
	 	clearBuf_led();
	 }
}


void cp_visual_mode6(void)
{
	int8_t dPos1 = 0, chPos = 0;
	dir  = 1;
	terang += 100;
	if (terang > 255)
	{
		if (dir)
			loadPos--;
		else
			loadPos++;
	}

	terang &= 0xFF;

	clearBuf_led();

	uint8_t Invert_terang = (255 - terang);

	if (loadPos >= ledRing1max)
		loadPos = 0;
	else if (loadPos < 0)
		loadPos = ledRing1max-1;

	dPos1 = /*halfRing +*/ loadPos;

	for (int8_t x = 0; x < ledRing1max; x++)
	{
		ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, terang));
		dPos1++;
	}

	for (int8_t x = 0; x < ledRing1max; x++)
	{
		ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, terang));
		dPos1++;
	}

	for (int8_t x = 0; x < ledRing1max; x++)
	{
		ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, terang));
		dPos1++;
	}
	
	// for (int8_t x = 0; x < ledRing1max; x++)
	// {
	// 	 if (dPos1 >= (ledRing1max + halfRing))
	// 	 	dPos1 -= ledRing1max;

	// 	if (x == 0)
	// 	{
	// 		if (dir)
	// 			ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, terang));
	// 		else
	// 			ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, Invert_terang));
	// 	}
	// 	if (x == (halfRing/2))
	// 	{
	// 		if (dir)
	// 			ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, Invert_terang));
	// 		else
	// 			ws2812_setPixelColor(dPos1 + 1, ws2812_color_Brightness(warna, terang));
	// 	}
	// 	if (x > 0 && x < (halfRing/2))
	// 		ws2812_setPixelColor(dPos1 + 1, warna);
	// 	dPos1++;
	// }

	// dPos1 = halfRing - loadPos;
	// for (int8_t x = 0; x < loadPos; x++)
	// {
	// 	 if (dPos1 < 0)
	// 	 {
	// 	 	dPos1 += ledRing2max;
	// 	 	chPos = 1;
	// 	 }
	// 	 if (chPos)
	// 	 {
	// 	 	if (dPos1 < (ledRing2max - halfRing))
	// 	 	{
	// 	 		dPos1 -= ledRing1max;
	// 	 		chPos = 0;
	// 	 	}
	// 	 }
	// 	if (x == 0)
	// 	{
	// 		if (dir)
	// 			ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, terang));
	// 		else
	// 			ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, Invert_terang));
	// 	}
	// 	if (x == (halfRing/2))
	// 	{
	// 		if (dir)
	// 			ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, Invert_terang));
	// 		else
	// 			ws2812_setPixelColor(dPos1, ws2812_color_Brightness(warna, terang));
	// 	}
	// 	if (x > 0 && x < (halfRing/2))
	// 		ws2812_setPixelColor(dPos1, warna);
	// 	dPos1--;
	// }
}


/**
 * centerpiece animation 
 * bubble rolling 
 * 
 * refer to visualMode_5
*/
void cp_visual_mode7(void)
{
	static uint32_t bitGlow = 0x000F1E3C;
	static uint32_t bitBrighT = 0x00081020;
	//	static uint32_t bitBrighB = 0x0008102;
	static uint32_t bitBrighB = 0x00010204;
	static int8_t dPos = 0;

	/** auto change color 
	 * if and only if AMBIENT_LED_AUTO_CHANGE not active
	 * this function will change warna variable 
	 * based on TimeChange_Color time
	*/
#if (AMBIENT_LED_AUTO_CHANGE==0)
	Auto_changeColor(70);
#endif

	terang += 20;	/** rolling speed  */
	if (terang > 255)
	{
		loadPos++;
	}

	terang &= 0xFF;
	/* ========================== ********* ========================== */
	clearBuf_led();
	if (loadPos > halfRing)
		loadPos = 0;

	if (loadPos != dPos)
	{
		if (bitGlow & 0x00100000) /* geser bit posisi lampu */
		{
			bitGlow <<= 1;
			bitGlow |= 0x00000001;
		}
		else
			bitGlow <<= 1;

		if (bitBrighT & 0x00100000) /* geser bit brighnest TOP */
		{
			bitBrighT <<= 1;
			bitBrighT |= 0x00000001;
		}
		else
			bitBrighT <<= 1;

		if (bitBrighB & 0x00100000) /* geser bit brighnest BOTTOM */
		{
			bitBrighB <<= 1;
			bitBrighB |= 0x00000001;
		}
		else
			bitBrighB <<= 1;

		dPos = loadPos;
	}

	uint8_t Invert_terang = (255 - terang);

	uint32_t temp_ = bitGlow;
	uint32_t temp1_ = bitBrighT;
	uint32_t temp2_ = bitBrighB;
	for (int8_t x = 0; x <= halfRing; x++)
	{
		if (!dir)
		{
			if (temp_ & 0x00000001)
			{
				setPixelColor_Ring1(x, warna);
				setPixelColor_Ring1(ledRing1max - x, warna);
			}
			temp_ >>= 1;

			if (temp1_ & 0x00000001)
			{
				setPixelColorBrightness_Ring1(x, warna, terang);
				setPixelColorBrightness_Ring1(ledRing1max - x, warna, terang);
			}
			temp1_ >>= 1;

			if (temp2_ & 0x00000001)
			{
				setPixelColorBrightness_Ring1(x, warna, Invert_terang);
				setPixelColorBrightness_Ring1(ledRing1max - x, warna, Invert_terang);
			}
			temp2_ >>= 1;
		}
		else
		{
			if (temp_ & 0x00100000)
			{
				setPixelColor_Ring1(x, warna);
				setPixelColor_Ring1(ledRing1max - x, warna);
			}
			temp_ <<= 1;

			if (temp1_ & 0x00100000)
			{
				setPixelColorBrightness_Ring1(x, warna, terang);
				setPixelColorBrightness_Ring1(ledRing1max - x, warna, terang);
			}
			temp1_ <<= 1;

			if (temp2_ & 0x00100000)
			{
				setPixelColorBrightness_Ring1(x, warna, Invert_terang);
				setPixelColorBrightness_Ring1(ledRing1max - x, warna, Invert_terang);
			}
			temp2_ <<= 1;
		}
	}
}