#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdio.h>

#define SOFTWARE_VERSION                (13)

#define CONFIG_FIRMWARE_I2C_SLAVE_ID    (0x51)

#define delay_ms(x)                     HAL_Delay(x)


/* debug option */
//#define DEBUG_UART

#ifdef DEBUG_UART
    /* printf definetion */
    #define DEBUG_MSG( fmt, ...)        \                                                                     
        printf( "[%08d] " fmt ,         \
        HAL_GetTick(), ##__VA_ARGS__)

	#define DBG( fmt, ...)        \
			printf(fmt , ##__VA_ARGS__)
#else
    #define DEBUG_MSG( FLAG, fmt, ... )
    #define DBG( fmt, ...)
#endif

/**
 * number of WS2812 used 
*/
#define CONFIG_LED_NUMBER               (5)

/**
 * set function to STANDBY after power on
 * 0: disable
 * 1: enable
*/
#define CONFIG_ENABLE_STANDBY               (0)
#if (CONFIG_ENABLE_STANDBY)
/**
 * this config used to determine is stanby is depend on 
 * spotify status
 * if spotify status already logged, so after set booting,
 * it will direct to spotify function 
 * standby function just used after factory reset
*/
#define CONFIG_STANDBY_DEPEND_NETWORK_CONNECTION    (1)
#endif

#define CONFIG_DEFAULT_FUNCTION                     (SYS_MODE_SPOTIFY_CONNECT)

/***
 * config to determine error indicator when speaker cannot connect 
 * to wifi access point
 * 0: disable
 * 1: enable 
 * 
 * failed connect to access point caused by:
 * 1. wrong wifi passphrase
 * 2. access point out of range
*/
#define CONFIG_NETWORK_ERROR_INDICATOR_ENABLE           (1)

/**
 *  set max system volume
*/
#define	SYS_VOL_MAX	                        (30)

/***
 * default volume after factory reset 
*/
#define DEFAULT_SYS_VOL                     (15)


/** config to use waiting timer to send key command bluetooth and
 * key command spotify
 * 1: enable
 * 0: disable
*/
#define CONFIG_WAIT_CHANGE_MODE_VENICEX     (1)
#define CONFIG_ROLLING_MODE                 (1)


#endif /* APP_CONFIG_H */
