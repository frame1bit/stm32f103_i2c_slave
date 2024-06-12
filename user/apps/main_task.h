#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include "main.h"
#include "app_config.h"
#include "utility/timeout.h"
#include "app_event_message.h"
#include "sys_app.h"
#include "communication_iface.h"

#define TIMEOUT_SIMULASI_BOOTING        (5000)
#define TIMEOUT_SIMULASI_BT_PAIR        (2000)
#define TIMEOUT_SIMULASI_WIFI_CONN      (2000)
#define TIMEOUT_SIMULASI_FACTORY_RESET  (6000)
#define TIMEOUT_SIMULASI_BROADCAST      (3000)
#define TIMEOUT_WIFI_NETWORK_CONFIG     (10000)
#define TIMEOUT_WAIT_ACK                (2000)

/**
 * timeout for waiting connection after booting process
 * venicex ready and start to reconnection progress
 * TODO: maybe need to request connection status is already configured or not
 *      so its can speed up the ready time.
 *      so after venicex ready to communication, it can immediately send network status
 *      1. network already configured
 *      2. not configured
*/
#define TIMEOUT_WAIT_CONNECTION         (5000)
/** waiting for spotify authentication process
 * may depend on network quality
*/
#define TIMEOUT_WAIT_SPOTIFY_AUTH       (5000)


/***
 * timeout used to wait from first pressed factory reset till 
 * venicex module reboot
*/
#define TIMEOUT_WAIT_FOR_FACTORY_RESET  (5000)

/** timeout for volume trick 
 * from local key set volume up/down --> send command vol up/down
 * and VX will reply increase volume step back to STM
 * during periode 5 s, command reply will ignored
*/
#define TIMEOUT_VOLUME_TRICK            (5000) //2000

/***
 * timeout use for function change because venice-x modul have
 * time to change function
*/
#define TIMEOUT_FUNCTION_CHANGE         (3000) // 3000
#define TIMEOUT_BLOCKING_FUNCTION_REPLY (3000)  // 3000

#define VOLUME_AUTO_RECOVER_VALUE_SUB   2

/** prototype function */
void main_task_init(void);
void main_task_run(void *arg);
/* end of prototype function */
#endif
