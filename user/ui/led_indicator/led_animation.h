#ifndef LED_ANIMATION_H
#define LED_ANIMATION_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "drivers/ws2812/ws2812_STM32.h"
#include "Signal_Process.h"
#include "Animation_Style.h"
#include "apps/sys_app.h"

/** funtion prototype */
void led_animation_init(void);		/* fungsi ini dipanggil di init main program. */
void led_animation_handler(void);	/* Fungsi ini dipanggi di Mainloop program. */

void set_visual_mode(uint8_t vmode);
void set_static_color(uint32_t color, uint8_t bright);
int8_t get_visual_mode(void);
void save_last_mode(void);
void set_vol_display(uint8_t vol);
void set_broadcast_display(SystemConfig_t *cfg);
void set_speaker_mode_display(SystemConfig_t *cfg);
/** end of prototype function */

/** extern resources */
extern SPI_HandleTypeDef hspi2;
//extern ADC_HandleTypeDef hadc1; // remove caused spectrum feature ommited
extern volatile uint8_t Brightness_External;

#ifdef __cplusplus
 }
#endif

#endif /*LED_ANIMATION_H*/
