
/**
 *  Animation_Style.h
 *  Modif & Optimize by Bria on 15 September 2020.
 *  yeramias.bria@gmail.com
 *  Library User Main Program Animation Style LED.
 */

#ifndef __Animation_Style_h
#define __Animation_Style_h

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "animation_config.h"
//#include "led_animation.h"

/* Params Anim Volume */
typedef struct
{
	uint8_t Level;
	uint8_t Shadow;
	uint8_t CountKedip;
	int16_t BrighKedip;
} hStyle_Volume;

/** blink no periode */
typedef struct
{
	/** blinking speed (max 255) */
	uint8_t blink_speed;

	/** brightness value, used as max brighness in blink*/
	uint8_t bright_static;

	/** base color except center_color*/
	uint32_t color;
	
	/***
	 * center color used in cp_set_dynamic_color animation
	 * blink control depend on flag_blink
	*/
	uint32_t center_color;

	/**
	 * blink use for center_color
	 * 1: blink 	
	 * 0: no blink, if center_color not NULL so color will be used
	*/
	uint8_t flag_blink;

} DisplayProperty_t;

typedef struct
{
	hStyle_Volume Volume;
	hStyle_Volume Bass;
	uint8_t FlagKedip;
	int8_t LevelVol;
	uint32_t timeOut;
} hStyleAnim_Volume;

/* ************************		Begin Prototype Function-1 Suport-ANIMATION		********************* */
/* Fungsi ini untuk konversi Level volume untuk Auto Gain AUdio Input. */
float getAuto_Gain(uint8_t level);
void getVol_shadow(uint8_t presentVol, uint8_t *levelOut, uint8_t *shadow);

void set_default(void);
void updateMode_Anim(void);
void setPixelColor_Ring1(uint8_t pixel, uint32_t color);
void setPixelColor_Ring2(uint8_t pixel, uint32_t color);
void setPixelColorBrightness_Ring1(uint8_t pixel, uint32_t color, uint8_t brightness);
void setPixelColorBrightness_Ring2(uint8_t pixel, uint32_t color, uint8_t brightness);
void setPixelColor_allRing1(uint8_t dynamic_pos, uint8_t jumlahLED, uint32_t color);
void setPixelColor_allRing2(uint8_t dynamic_pos, uint8_t jumlahLED, uint32_t color);
void loadVisual3(uint8_t dir, uint8_t dynamic_pos, uint32_t color, uint8_t brightness);
void loadVisual4(uint8_t dir, uint8_t dynamic_pos, uint32_t color, uint8_t brightness);
void loadVisual11(uint8_t dir, uint8_t dynamic_pos, uint32_t color, uint8_t brightness);
void Auto_changeColor(const uint16_t time);
/* ************************		End Prototype Function-1 Suport-ANIMATION		********************* */

void visualMode_0(void);						/* Stengah lingkaran putar Patah-Patah (Start Static). */
void visualMode_1(void);						/* Stgh lingkaran kedip_Ganti-gantian. */
void visualMode_2(void);						/* VUmeter hadapan (RGB) (jam 9 & 3)/(jam 3 & 9) Refren EDEN Style. */
void visualMode_3(void);						/* VUmeter hadapan (RGB) (jam 12 & 6) JBL Style. */
void visualMode_4(void);						/* Shadow_Up-Down JBL style. */
void visualMode_5(void);						/* Putar Gelembung -> Black-shadow JBL style. */
void visualMode_6(void);						/* Stengah lingkaran putar Patah-Patah bentuk 'S'. */
void visualMode_7(void);						/* 1/4  (Disk) lingkarang mutar-mutar Patah-Patah. */
void visualMode_8(void);						/* Random timer Ganti-ganti mode. */
void visualMode_SingleColor(uint32_t RGB_Code); // nembak 1 warna aja JMH 22 12 22
void animVolume(void);							/* Animasi Volume. */
void visualMode_9(void);						/* experiment */
void visualMode_10(void);						/* experiment 2*/
void visualMode_11(void);						/* experiment == 4 line searah ke belakang*/
void visualMode_12(void);						/* experiment == 4 line searah ke depan*/
void visualMode_13(void);						// VU meter lurus JH 20 01 23
void reloadTime_DispVol(void);

void set_initial_pos(int8_t pos);
void cp_visual_mode0(void);
void cp_set_single_color(uint32_t rgb_code);
void cp_set_dynamic_color(void);
void cp_animation_blink(uint8_t speed);
void cp_set_single_color_brightness(uint32_t color);
void cp_visual_mode2(void);
void cp_visual_mode3(void);
void cp_visual_mode4(uint32_t color);
void cp_visual_mode5(uint8_t direction);
void cp_visual_mode6(void);
void cp_visual_mode7(void);
void cp_animation_half_blink(uint8_t *blink_state, uint8_t mode, uint32_t static_color);


extern hStyleAnim_Volume pStyleAnimVol;
extern DisplayProperty_t dispProp;

#ifdef __cplusplus
}
#endif

#endif
