#include "led_animation.h"
#include "app_config.h"
#include "apps/sys_app.h"

#define	GET_TICK()	HAL_GetTick()

#if debugTime
volatile uint32_t tCapture = 0, tCapADC = 0, tFFT = 0, tConv = 0, tADC = 0;
volatile uint32_t tAnim = 0;
#endif

#if FFT
volatile int16_t data_ADC[sampleData + 2];
int32_t magnitudeFreq[3] = {0, 0, 0};
volatile float gIN = 1.0;
#else
int16_t adc = 0, addc = 0;
int16_t adc_filter = 0;
volatile float gIN = 1.0;
#endif

volatile uint8_t NoSignal = 1;
volatile uint8_t adcComplate = 0;

/* Init. Params variable mode control.*/
int8_t mode_visual = 1, lastMode_visual = 100, saveLast_Mode = 0;
uint32_t static_color; // JH 23 01 23
uint16_t Cnt_WheelColor = 140;
uint32_t warna = 0;
volatile uint32_t temp_var;
volatile uint8_t Brightness_External;

uint16_t sampling[Max_bufSampling], countSampling = 0;
uint16_t minLVL = 0, maxLVL = 0;
int32_t AvgTH = 0, AvgTH_Filter = 0, AvgTH_Filter_H = 0, LvlAudio_Filter = 0;
short height = 0;

uint16_t timeChange_potition = 200;
uint32_t timeUpdate_LED_Cnt = 0;

static uint8_t bs_speaker_mode = 0;
/* ========================== End Declerasi Variable Control Protype ======================== */

/* =============================== Function Control Animation LED =========================== */
void led_animation_init(void)
{
	ws2812_init(&hspi1);

#if (ENABLE_AUDIO_INPUT_ANIMATION)
	Audio_Sys_Init();
#endif

	/** set default color when auto change color disabled */
#if (!AMBIENT_LED_AUTO_CHANGE)
	warna = LED_COLOR_PURPLE;
#endif

}

void led_animation_handler(void)
{
#if (ENABLE_AUDIO_INPUT_ANIMATION)
	Audio_Sys_Handler();
#endif

	Draw_Anim();
}

void Audio_Sys_Init(void)
{
	//HAL_TIM_Base_Start(&htim3);
#if FFT
	//HAL_ADC_Start_DMA(&hadc1, (uint32_t *)data_ADC, sampleData); // removed bacause spectrum feature ommited
	/* FFT Init. . . . . . . . . . . */
	FFT_init();
#else
	HAL_ADC_Start_IT(&hadc1);
#endif

	warna = Wheel(Cnt_WheelColor);
}
/* ----------------------------------------------------------------------------------------- */
/* *******************		User Prototype Function-2 Handler-ANIMATION		******************** */
void Audio_Sys_Handler(void)
{
#if FFT
	{
		static int32_t tt1 = 0;
		if (adcComplate)
		{
			adcComplate = 0;
#if debugTime
			tADC = GET_TICK - tCapADC;
			tCapture = GET_TICK;
#endif

			FFT_analysis((int16_t *)data_ADC, dataFFT);
/* Reset ADC untuk Baca lagi ADC. */
#ifdef BRIGHTNESS_ADC // JH 11 10 22 //untuk set brightness via ADC
			Brightness_External = (data_ADC[0] >> 2);
			if (Brightness_External < 38)
				Brightness_External = 0; // JH 06 10 22
#endif
			//HAL_ADC_Start_DMA(&hadc1, (uint32_t *)data_ADC, sampleData); // removed bacause spectrum feature ommited
			//HAL_TIM_Base_Start(&htim3);

#if debugTime
			tCapADC = GET_TICK();
			tFFT = tCapADC - tCapture;
			tCapture = tCapADC;
#endif
			/* konversi output bilangan komplex ke data absoluted magnitude Level BAR 5 Band */
			ComplexTo_5_FreqBand(dataFFT, magnitudeFreq);

			/* Get trigger Audio Filter RGB-LED */
#if RGB_TRIGGER == BASS_ONLY
			/* Filter Bass (untuk trigger anim). */
			/* Hanya Bass sebagai acuan trigger anim. */
			tt1 = magnitudeFreq[0];
#elif RGB_TRIGGER == BASS_TREBLE
			/* Filter Bass dan Treble (untuk trigger anim). */
			/* Jika tidak ada Bass, maka treble sebagai acuan trigger. */
			if (magnitudeFreq[0] >= magnitudeFreq[1])
				tt1 = magnitudeFreq[0];
			else
				tt1 = magnitudeFreq[1];
#endif

			sampling[countSampling] = tt1;
			if (++countSampling >= Max_bufSampling)
				countSampling = 0;
			minLVL = maxLVL = sampling[0];
			for (uint8_t i = 0; i < Max_bufSampling; i++)
			{
				if (sampling[i] < minLVL)
					minLVL = sampling[i];
				else if (sampling[i] > maxLVL)
					maxLVL = sampling[i];
			}
			AvgTH = ((minLVL + maxLVL) / 2);

			/* Filter Source & reduce noise. . . . */
			int32_t tempFilter;
			tempFilter = (tt1 * 40);
			LvlAudio_Filter = (float)(((LvlAudio_Filter * 60) + tempFilter) / 100);

			tempFilter = (AvgTH * 10);										  // 4
			AvgTH_Filter = (float)(((AvgTH_Filter * 90) + tempFilter) / 100); // 96

#if debugTime
			tConv = GET_TICK() - tCapture;
#endif
		}
	}
#else
	{
#ifndef BRIGHTNESS_ADC // JH 11 10 22
		if ((GET_TICK() - tlast) > 1)
		{
			tlast = GET_TICK();
			//    volatile int16_t tt2 = addc;
			volatile int16_t tt1 = adc_filter;

			if (tt1 > 50)
				tt1 -= 50;
			else
				tt1 = 0;

			//		if(tt1 < lastlevel) tt1 = lastlevel-2;
			//		lastlevel = tt1;

			sampling[countSampling] = tt1;
			if (++countSampling >= bufSampling)
				countSampling = 0;
			minLVL = maxLVL = sampling[0];
			for (uint8_t i = 0; i < bufSampling; i++)
			{
				if (sampling[i] < minLVL)
					minLVL = sampling[i];
				else if (sampling[i] > maxLVL)
					maxLVL = sampling[i];
			}
			maxAVG = (float)(maxAVG * 0.9391) + (0.0609 * maxLVL);
			minAVG = (float)(minAVG * 0.9391) + (0.0609 * minLVL);

			avgLVL = ((minAVG + maxAVG) / 2);
			avgLVL = (maxAVG + avgLVL) / 2;

			if (tt1 > avgLVL + 2)
				height = ledRing1max;
			else if (tt1 < avgLVL)
				height = 1;

			/*
					bufTx[0]=0xAA;
					bufTx[1]=tt2 & 0x00FF;
					bufTx[2]=tt2 >> 8;
					//bufTx[2]=adc_filter & 0x00FF;
					bufTx[3]=tt1 & 0x00FF;
					bufTx[4]=tt1 >> 8;
					bufTx[5]=avgLVL & 0x00FF;
					bufTx[6]=avgLVL >> 8;
					HAL_UART_Transmit(&huart1,bufTx,7,10);
			*/

			if (height > last_height)
			{
				timeNo_signal = 0;
				kk = 0;
				if (++fallCount > 4)
				{
					height = last_height + 1;
					fallCount = 0;
					last_height = height;
				}
				else
					height = last_height;
			}
			else if (height < last_height)
			{
				timeNo_signal = 0;
				kk = 0;
				if (++fallCount > 9)
				{
					height = last_height - 1;
					fallCount = 0;
					if (height < 1)
						height = 0;
					last_height = height;
				}
				else
					height = last_height;
			}
			else
			{
				height = last_height;
				if (++timeNo_signal > 250) // 0.8s => tdk deteksi signal input = 0;
				{
					kk = 1;
					timeNo_signal = 0;
				}
			}

			if (Volume < 4)
				height = 0;
		}
#else
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)data_ADC, sampleData);
		HAL_TIM_Base_Start(&htim3);
#endif
	}
#endif
}

void set_vol_display(uint8_t vol)
{
    uint8_t temp_vol = (uint8_t)(vol*100/SYS_VOL_MAX);

    if (temp_vol >= 100)
    {
        pStyleAnimVol.FlagKedip |= flag_maxVol_1;
    }
    else
    {
        pStyleAnimVol.FlagKedip &= (~flag_maxVol_1);
    }
    getVol_shadow(temp_vol, &pStyleAnimVol.Volume.Level, &pStyleAnimVol.Volume.Shadow);
    pStyleAnimVol.LevelVol = temp_vol;

    if ( (get_visual_mode() != LED_EVENT_TEMP_SHOW) && (get_visual_mode() != LED_EVENT_ONE_SHOOT) )
    {
        save_last_mode();
    }

    set_visual_mode(LED_EVENT_TEMP_SHOW);

    reloadTime_DispVol();
}

/***
 * @brief   trigger display for broadcast acti
*/
void set_broadcast_display(SystemConfig_t *cfg)
{
    if (get_visual_mode() != LED_EVENT_TEMP_SHOW && \
		get_visual_mode() != LED_EVENT_ONE_SHOOT && \
		get_visual_mode() != LED_EVENT_SPEAKER_MODE)
    {
        save_last_mode();
    }

    set_visual_mode(LED_EVENT_ONE_SHOOT);

    /** set initial value for broadcast display 
     * visual mode ONE SHOOT
     * set initial pos based on state
    */
    if (cfg->bt_broadcast.role == BT_BROADCAST_ON_ROLE_MASTER || \
		cfg->bt_broadcast.role == BT_BROADCAST_ON_ROLE_SLAVE)
    {
        set_initial_pos(halfRing);
    }
    else
    {
        set_initial_pos(0);
    }
    
}

void set_speaker_mode_display(SystemConfig_t *cfg)
{
	if (get_visual_mode() != LED_EVENT_TEMP_SHOW && \
		get_visual_mode() != LED_EVENT_SPEAKER_MODE && \
		get_visual_mode() != LED_EVENT_ONE_SHOOT)
    {
        save_last_mode();
    }

	set_visual_mode(LED_EVENT_SPEAKER_MODE);
	bs_speaker_mode = 0;
}

static uint32_t function_to_color(uint8_t func)
{
	switch(func)
	{
		case SYS_MODE_BT_A2DP:
			return LED_COLOR_BLUETOOTH;
		case SYS_MODE_SPOTIFY_CONNECT:
		case SYS_MODE_NETWORK_CONFIG:
			return LED_COLOR_SPOTIFY;
	}

	return LED_COLOR_RED;
}

void set_visual_mode(uint8_t vmode)
{
	mode_visual = vmode;
}

int8_t get_visual_mode(void)
{
	return (mode_visual);
}

void save_last_mode(void)
{
	saveLast_Mode = mode_visual;
}

/***
 * set static color with GRB format
*/
void set_static_color(uint32_t color, uint8_t bright)
{
	static_color = color;
	dispProp.bright_static = bright;

#if (!AMBIENT_LED_AUTO_CHANGE)
	warna = color;
#endif
}

void Draw_Anim(void) /* write animasi */
{
	/* update animasi jika time update tercapai. */
	if (GET_TICK() - timeUpdate_LED_Cnt >= TimeUpdate_LED)
	{
		timeUpdate_LED_Cnt = GET_TICK();

		/* Call update Animation mode function. */
		updateMode_Anim();

#if (AMBIENT_LED_AUTO_CHANGE)
		/* Call generator change color function. */
		Auto_changeColor(TimeChange_Color);
#endif

#if (ENABLE_AUDIO_INPUT_ANIMATION)
		/* Get data audio input FIlter to trigger LED Animation. */
		Get_Audio_Trigger();
#endif

#ifdef BRIGHTNESS_ADC // JH 11 10 22
		warna = ws2812_color_Brightness(temp_var, Brightness_External);
#endif
		switch (mode_visual)
		{
		/**
		 * show animation, dont save visual mode
		 * dont have display property (DispProp) control
		 * no timeout
		*/
		case LED_EVENT_BOOTING:
			//cp_visual_mode1();
			//cp_visual_mode3();
			//visualMode_5();
			//cp_visual_mode4();
			cp_visual_mode7();
			break; 

		/**
		 * show static color with control from display property (DispProp)
		 * have middle color static led with blink behaviour
		*/
		case LED_EVENT_DYNAMIC_COLOR:
			cp_set_dynamic_color();
			break;

		case LED_EVENT_BT_BROADCAST:
			cp_set_dynamic_color();
			
			break;

		case LED_EVENT_STATIC_COLOR:
			cp_set_single_color_brightness(dispProp.color);
			break;

		/**
		 * show animation, dont save visual mode
		 * dont have display property (DispProp) control
		*/
		case LED_EVENT_FACTORY_RESET:
			cp_visual_mode4(LED_COLOR_RED);
			break;

		/***
		 * show level with timeout, save last visual mode
		 * dont have display property (DispProp) control
		*/
		case LED_EVENT_TEMP_SHOW:
			animVolume();
			break; /* display volume level */

		/***
		 * blink no timeout, dont save visual mode
		 * have display property (DispProp) control
		*/
		case LED_EVENT_ERROR:
		case LED_EVENT_BLINK:
			cp_animation_blink(LED_BLINK_SPEED);
			break;

		/**
		 * one shoot behaviour, save last visual mode
		 * dont have display property (DispProp) control
		*/
		case LED_EVENT_ONE_SHOOT:
			cp_visual_mode5( (system_config.bt_broadcast.role) ? 0 : 1);
			break;


		/***
		 * animation used in network setup mode 
		*/
		case LED_EVENT_SETUP_MODE:
			cp_visual_mode2();
			break;

		/**
		 * set animation for speaker mode
		 * 0: mix, 1: L Mode 2: R Mode
		 *  
		*/
		case LED_EVENT_SPEAKER_MODE:
			cp_animation_half_blink(&bs_speaker_mode, 
									system_config.speaker_mode, 
									function_to_color(system_config.current_function));
			break;

		default:
			clearBuf_led();
			break; /* No Mode Anim. */
		}

		/* Limit max time change. */
		if (timeChange_potition < TimeOutDirect_Anim)
			timeChange_potition++;

#if debugTime
		tAnim = GET_TICK() - timeUpdate_LED_Cnt;
#endif

		/* Update LED-strip. . . . */
		ws2812_show();
	}
}
/* ----------------------------------------------------------------------------------------- */

/* ========================================================================================= */
void Get_Audio_Trigger(void)
{
	static uint32_t timeNo_signal = 0;
	/* Get data audio input FIlter to trigger LED Animation. */
	if (LvlAudio_Filter <= AvgTH_Filter)
	{
		height = 0;
		/* deteksi jika tidak ada signal. */
		/* wait >= TimeOut Detect No Signal: LED mode standby. */
		if (++timeNo_signal > TimOutDetect_NoSig)
		{
			NoSignal = 1;
			timeNo_signal = 0;
		}
	}
	else if (LvlAudio_Filter > (AvgTH_Filter + TH_LevelTrigger))
	{
		height = 10;

		timeNo_signal = 0;
		NoSignal = 0;
	}
#ifdef BRIGHTNESS_ADC // JH 11 10 22
	pStyleAnimVol.LevelVol = 0;
#endif
	if (pStyleAnimVol.LevelVol < 4)
	{
		height = 0;
		NoSignal = 1;
	}
}
