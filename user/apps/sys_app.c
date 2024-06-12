#include "main.h"
#include "app_config.h"
#include "sys_app.h"
#include "utility/timeout.h"
#include "communication_iface.h"


/** set global flag */
gFlag_t gflag_sys;

/**
 * global system information 
*/
SystemConfig_t system_config = 
{
    .system_volume = DEFAULT_SYS_VOL,
    .current_function = SYS_MODE_BOOTING,
    .blink_periode = 500
};


/***
 * @brief   set default parameter after factory reset
 *          - system function
 *          - system volume
*/
void system_app_set_default(void)
{
	// removed
}

/**
 * @brief   system application init 
*/
void system_app_init(void)
{
    if ( !read_reg ) 
        return;
        
    read_reg->id = CONFIG_FIRMWARE_I2C_SLAVE_ID;
    read_reg->boot_info.bit.ready = 0;
}
