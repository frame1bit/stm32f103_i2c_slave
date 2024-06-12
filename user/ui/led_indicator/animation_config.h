/**
 * @file animation_config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ANIMATION_CONFIG_H
#define ANIMATION_CONFIG_H

#include "drivers/ws2812/ws2812_STM32.h"

/* Jika ingin Debug time FFT dan Proses Animasi. *(1):Enable */
#define debugTime						(0)
//#define BRIGHTNESS_ADC //JH 17 10 22 

/* Private Definend Macro ----------------------------- */
#define BASS_ONLY						(0)
#define BASS_TREBLE						(1)

/* ************************		User Defined Params-1 CONTROL-LED & Signal ********************* */
/* Set RGB Led Trigger (Bass or Bass Treble). */
#define RGB_TRIGGER					(BASS_ONLY)		/* BassOnly a/ BassTreble trigger RGB-LED*/
/* ================================ */
#define BASS_NOISE_LVL				(30)
#if RGB_TRIGGER == BASS_TREBLE
	#define TREBLE_NOISE_LVL		(50)
#endif

#define Max_bufSampling 			(20)
#define TH_LevelTrigger				(10)

#define Anim_volBlock				(1)
/* ************************************* */
#define ringCount					(1)		/* jumlah Ring LED */

/* Time Update Animation-LED (saran Minimal time 20ms). */
#define TimeUpdate_LED				(20)	/* Time Update LED (.. ms) */
#define maxMode							(uint8_t) (4)	/* 9 mode + 1 untuk clear. */

/* ************************		User Defined Params-2 CONTROL-ANIMATION		********************* */
#define request_VolColor			(6)			/* request DQA Warna Volume & Bass. */

#define TimeOutRandom_Anim			(13)		/* 12s.		Ganti-ganti Mode Auto. */
#define TimeOutDirect_Anim			(650)		/* time out direction animasi left/right (time*20ms) *ganti saat ada signal */
#define TimeChange_Color			(350)
#define CountKedip_VOL				(4)			/* Jumlah Kedip saat Volume Level Maximal. */
#define TimeOut_DisplayVOL			(1500)	/* Waktu Time Out Kedip dan Display Level Volume. (..ms)*/
#define Tim_Detect_NoSignal			(1500)	/* Waktu Time Out Detect No Signal Input. (..ms)*/


#define flag_maxVol_1				(uint8_t) (0x01)
#define flag_maxVol_2				(uint8_t) (0x02)
#define	flag_bt_pairing				(uint8_t) (0x04)


/*----------------------------------------------------------------------------*/
#define TimOutDetect_NoSig	        (Tim_Detect_NoSignal / TimeUpdate_LED)

#define ledRing1max					(count_led / ringCount)
#define ledRing2max					(ledRing1max * 2)
#define halfRing					(count_led / (ringCount * 2))
#define halfRing2					(count_led / (ringCount * 4))
#define halfRing4					(count_led / (ringCount * 8))

#define lineBawah1max       (5)
#define lineBawah2max       (10)
#define lineAtas1max        (13)
#define lineAtas2max        (16)

/**
 * denominator on formula used in funtion getVol_shadow
 * used for animation volume progress 
 * formula :
 * (max volume in percent) / (ring number)
*/
#define	LED_VOL_DENOMINATOR	((float) (100/CONFIG_LED_NUMBER) )	/** 100/led_num*/


/** add for centerpiece speaker config *******************/
/**
 * 1: enable auto change ambient light
 * 0: disable auto change ambient light
*/
#define	AMBIENT_LED_AUTO_CHANGE		(0)

/**
 * enable audio adc spectrum using adc input
 * 1: enable
 * 0: disable
*/
#define	ENABLE_AUDIO_INPUT_ANIMATION	(0)

#define	RGB_TO_GRB(r, g, b)		(uint32_t)(g<<16 | r<<8 | b)


enum
{
	LED_EVENT_BOOTING= 0x01,
	LED_EVENT_STATIC_COLOR,
	LED_EVENT_DYNAMIC_COLOR,
	LED_EVENT_BLINK	,
	LED_EVENT_ERROR,
	LED_EVENT_FACTORY_RESET,
	LED_EVENT_TEMP_SHOW,
	LED_EVENT_ONE_SHOOT,
	LED_EVENT_SPEAKER_MODE,
	LED_EVENT_SETUP_MODE,
	LED_EVENT_BT_BROADCAST,
};

/** set color definition in GRB format */
#define	LED_COLOR_RED			RGB_TO_GRB(255 ,0 ,0)
#define	LED_COLOR_GREEN			RGB_TO_GRB(0, 255, 0)
#define	LED_COLOR_BLUE			RGB_TO_GRB(0, 0, 255)
#define	LED_COLOR_YELLOW		RGB_TO_GRB(255, 255, 0)
#define	LED_COLOR_WHITE			RGB_TO_GRB(255, 255, 255)
#define	LED_COLOR_CYAN			RGB_TO_GRB(0, 255, 255)//(uint32_t)(0xFF00FF)
#define	LED_COLOR_MAGENTA		RGB_TO_GRB(255, 0, 255)//(uint32_t)(0x00FFFF)
#define	LED_COLOR_PURPLE		RGB_TO_GRB(128, 0, 128)//(uint32_t)(0x008080)
#define	LED_COLOR_ORANGE		RGB_TO_GRB(255, 100, 0)//(uint32_t)(0x64FF00)

/** color defined by ID (industrial design) */
#define	LED_COLOR_BLUETOOTH_BLUE	RGB_TO_GRB(0, 40, 255)//(uint32_t)(0x8200FC)
#define	LED_COLOR_SPOTIFY_GREEN		RGB_TO_GRB(10, 255, 10)//(uint32_t)(0xB91D54)
#define	LED_COLOR_BROADCAST_YELLOW	RGB_TO_GRB(180, 255, 0)

/** just for test */
#define	LED_COLOR_CHANGE_VOLUME_WHITE	LED_COLOR_WHITE
#define	LED_COLOR_CHANGE_VOLUME_PINK	RGB_TO_GRB(255, 0, 255)//(uint32_t)0x00FFFF
#define	LED_COLOR_CHANGE_VOLUME_PURPLE	RGB_TO_GRB(153, 0, 204)//(uint32_t)0x0099CC
/* end just for test */

#define	LED_COLOR_BLUETOOTH				LED_COLOR_BLUETOOTH_BLUE//LED_COLOR_CYAN//LED_COLOR_BLUE
#define	LED_COLOR_SPOTIFY				LED_COLOR_SPOTIFY_GREEN//LED_COLOR_GREEN
#define	LED_COLOR_NETWORK_DISCONNECTED	LED_COLOR_SPOTIFY_GREEN
#define	LED_COLOR_ERROR					LED_COLOR_RED
#define	LED_COLOR_STANDBY				LED_COLOR_WHITE
#define	LED_COLOR_BT_BROADCAST_MASTER	LED_COLOR_RED	// cyan
#define	LED_COLOR_BT_BROADCAST_SLAVE	LED_COLOR_BROADCAST_YELLOW//LED_COLOR_YELLOW
#define	LED_COLOR_CHANGE_VOLUME			LED_COLOR_WHITE

#define	LED_BRIGHTNESS_DEFAULT		(150)
/************************
 * linear equation
 * 255/150 = 10/5 --> based on experiment blink brightness
 * y = mx + b
 * m = (y2-y1)/(x2-x1)
 * m = (10-5)/(255-150)
 * m = 5/105 = 0,04
 * --------------------
 * --> x,y (255,10)
 * 10 = (0,04) * 255 + b
 * 10 = 12,14 + b
 * b = 10 - 12,14
 * b = -2
*/
#define	LED_BLINK_SPEED				(unsigned char)( (0.04 * LED_BRIGHTNESS_DEFAULT) - 2 )

/***
 * set number of centered led number
 * @note used on cp_set_dynamic_color function in Anymation_Style.c
*/
#define	CENTER_LED_NUM				(2)	// for 36 led


#endif /*ANIMATION_CONFIG_H*/
