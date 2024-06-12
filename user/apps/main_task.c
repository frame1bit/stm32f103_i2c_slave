#include "main_task.h"

EventContext msgSend;
uint8_t mainTaskState = TASK_STATE_IDLE;
uint8_t subTaskState[2] = {TASK_STATE_IDLE};
enum 
{
    SUBT_BTBROADCAST = 0,
    /**reserved*/
};

uint8_t subTaskNetworkState = TASK_STATE_IDLE;

/**
 * timer used to validity check connection
 * is it actually disconnected or just temporary
 * lost connection 
*/
TIMER tmrWaitConn;

TIMER tmrIdle;
TIMER tmrFsVolumeChange;
TIMER tmrFactoryTimeout;    // timeout if factory reset fail, it will reset flag
                            // so user can press factory reset again
TIMER tmrCheckStandbySetupMode;

/** this timer used to tricky volume feedback from VX module
 * after increase or decrease volume from local key, VX module wiil reply pre step 
 * so it will caused repetition effect
*/
TIMER tmrVolumeSync;

/*** time used to waiting FS module stable when change function
 * if user press button mode too fast its caused the FS module not be able processed
 * key command bluetoothe and key command spotify
 * FS module take sometime to ready in switch mode
*/
TIMER tmrWaitChangeMode;
/***this timeout timer used for blocking current  function reply
 * from VX module until current function reply from VX stable (latest)
*/
TIMER tmrBlockingFunctionReplyVx;


/** timeout used for auto broadcast mode */
TIMER tmrAutoBroadcast;
TIMER tmrSimMaster, tmrSimSlave;
#define TIMEOUT_BROADCAST_MASTER    6000
#define TIMEOUT_BROADCAST_SLAVE     6000

/** this timer used to set timeout in broadcast slave mode 
 * and not trigger the volume sync
*/
TIMER tmrVolSyncTimeout;

#if 1//(CONFIG_STANDBY_DEPEND_NETWORK_CONNECTION)
//static TIMER tmrWifiWaitConn;
static TIMER tmrWaitForWakeup;
#endif

TIMER tmrBroadcastPreventFault;

TIMER tmrUpdateI2CReg;
uint8_t _reg[12];
uint16_t counter = 0;

#define SET_ANIMATION_PROPERTY(s, _blink_speed, _bright_static, _color, _center_color, _flag_blink) \
		s.blink_speed = _blink_speed,       \
		s.bright_static = _bright_static,   \
        s.color = _color,                   \
        s.center_color = (_center_color!=NULL) ? _center_color : _color,     \
        s.flag_blink = _flag_blink

/** makro to whitelist message in broadacast mode slave 
 * (ev->eventId != MSG_BT_BROADCAST) && (system_config.bt_broadcast.role == BT_BROADCAST_ON_ROLE_SLAVE)
*/
#define IS_MSG_BLOCKED_ON_BT_BROADCAST_SLAVE(msg) \
        ( (system_config.bt_broadcast.role == BT_BROADCAST_ON_ROLE_SLAVE) && \
            (msg != MSG_BT_BROADCAST && \
             msg != MSG_BT_BROADCAST_SYNC_VOL && \
             msg != MSG_SPEAKER_MODE))


static void task_state_control(uint8_t *state_id, uint8_t state)
{
    *state_id = state;
}

static uint8_t get_next_mode(uint8_t cur_mode)
{
    if (cur_mode == SYS_MODE_BT_A2DP) return SYS_MODE_SPOTIFY_CONNECT;
    else if (cur_mode == SYS_MODE_SPOTIFY_CONNECT) return SYS_MODE_BT_A2DP;
    else if (cur_mode == SYS_MODE_NETWORK_CONFIG) 
    {
        if (fs_comm_get_wifi_status() != FS_WIFI_STATE_CONNECTED)
        {
            return SYS_MODE_BT_A2DP;
        }
        else
        {
            return SYS_MODE_SPOTIFY_CONNECT;
        }
    }
    else if (cur_mode == SYS_MODE_FACTORY_RESET) return SYS_MODE_SPOTIFY_CONNECT;
    /** reserved for other function */
}

static uint8_t convert_fs_mode(uint8_t fsmode)
{
    if (fsmode == FS_MODE_BLUETOOTH)
    {
        return SYS_MODE_BT_A2DP;
    }
    else if (fsmode == FS_MODE_SPOTIFY)
    {
        return SYS_MODE_SPOTIFY_CONNECT;
    }
#if (CONFIG_ENABLE_STANDBY)
    else if (fsmode == FS_MODE_SPOTIFY)
    {
        return SYS_MODE_STANDBY;
    }
#endif
    
    /* may this can be in NO_MODE 00h */


    return SYS_MODE_IDLE;
}

/**
 * @brief do mode change, if task state is paused (speaker role = slave) --> ignore
 * 
 * @param state_id taskState
 * @param ev event ID
 */
static void do_change_task(uint8_t *state_id, EventContext *ev)
{
    if (ev->eventId == MSG_MODE && *state_id != TASK_STATE_PAUSE)
    {

        if ( GET_FLAG(FLAG_CSB_SCANNING_STATUS) )
            return;
            
#if (CONFIG_ROLLING_MODE)
        TimeoutSet(&tmrWaitChangeMode, TIMEOUT_FUNCTION_CHANGE);
#endif // CONFIG_ROLLING_MODE

        *state_id = TASK_STATE_STOP;
        system_config.current_function = get_next_mode(system_config.current_function);
    }
    else if (ev->eventId == MSG_SET_MODE)
    {

#if (CONFIG_WAIT_CHANGE_MODE_VENICEX)
        if ( !IsTimeout(&tmrBlockingFunctionReplyVx) )
            return;
#endif
        uint8_t tempFunc = convert_fs_mode(FS.config.data1.bit.mode);
        if ( (system_config.current_function != tempFunc) && (tempFunc != SYS_MODE_IDLE) )
        {
            *state_id = TASK_STATE_STOP;
            system_config.current_function = tempFunc;

            SET_FLAG( FLAG_IS_MODE_FROM_FS );
        }
    }
#if (CONFIG_ENABLE_STANDBY)
    else if (ev->eventId == MSG_POWER)
    {
        *state_id = TASK_STATE_STOP;
        /** backup current function before go to standby */
        system_config.pre_function = system_config.current_function;
        system_config.current_function = SYS_MODE_STANDBY;
    }
#endif
    else if(0 /*reserved*/)
    {

    }
    else
    {}
    
}


/***
 * @brief   used to check is current mode is standby
 *          if current mode is standby and in setup mode so it must
 *          be wake up to trigger frontier to show SSID
 *          this function may executed in RUN_STATE so it use timer protection so 
 *          it will not trigger key command standby repeatedly
*/
static void do_check_standby_and_setup_mode(void)
{
    if ( fs_comm_get_wifi_status() == FS_WIFI_SETUP_MODE && fs_comm_get_mode() == FS_MODE_STANDBY )
    {
        if ( IsTimeout(&tmrCheckStandbySetupMode) )
        {
            /** wakeup from standby state */
            fs_comm_send_command(KC_STANDBY, KC_EVENT_KEY_PRESSED);
            TimeoutSet(&tmrCheckStandbySetupMode, 2000);
        }
    }
}

enum
{
    /** state from internal system*/
    BT_NO_ACK = 0,
    /** state from bt broadcaster, role wait for reply */
    BT_ACK,
};

/***
 * @brief get broadcast role
 * @param   ack: is ack (wait reply from bt broadcaster)
 * @return  bt role
*/
static uint8_t get_broadcast_role(/*void*/ uint8_t ack)
{
    /** @removed: bt broadcast */
}

/***
 * @brief get CSB device connection status
 * 
*/
static uint8_t get_broadcast_device_connection_status(void)
{

}
/**
 * @brief   reset device connection status variable before 
 *          send command bt broadcast role
*/
static void reset_broadcast_device_connection_status(void)
{

}

/**
 * subtask for bluetooth broadcast process handle
 * flow:
 * user press buttton (short pressed) to determine role of bt broadcast
 * rolling state from 0: off, 1: master, 2: slave
 * 
*/
static void subtask_bt_broadcast(SystemConfig_t *sc)
{
    /** @removed: bt broadcast */
}



/**
 * @brief common task using on several task handler
 * @note    
*/
static void task_common(EventContext *ev)
{
    static uint8_t tx_data = 0;
    static uint16_t temp = 0;

    if (ev->eventId != MSG_NONE)
    {

    /** @removed: bt broadcast block on slave */

    if ( (system_config.current_function == SYS_MODE_BOOTING && ev->eventId != MSG_FACTORY_RESET) && 
            system_config.venicex_state != VENICEX_STATE_READY)
        return;

        switch(ev->eventId)
        {
            /** volume up event id */
            case MSG_VOL_UP:
                if (system_config.system_volume < SYS_VOL_MAX)
                {
                    system_config.system_volume++;
                }


                TimeoutSet(&tmrVolumeSync, TIMEOUT_VOLUME_TRICK);
                fs_comm_send_command(KC_VOLUME_UP, KC_EVENT_KEY_PRESSED);

                break;

            /** volume down event id */
            case MSG_VOL_DW:
                if (system_config.system_volume > 0)
                {
                    system_config.system_volume--;
                }

                TimeoutSet(&tmrVolumeSync, TIMEOUT_VOLUME_TRICK);
                fs_comm_send_command(KC_VOLUME_DOWN, KC_EVENT_KEY_PRESSED);

                /** @removed: bt broadcast */
                break;
            
            /** set to direct value */
            case MSG_BT_BROADCAST_SYNC_VOL: /**  message from bt broadcast sync */
            case MSG_FS_VOL_SET: /** message source from FS uart event */
                /** @removed: setting volume */
                /**
                 * if any messge from FS it will sync volume to slave device
                */
                if (ev->eventId == MSG_FS_VOL_SET)
                {
                    /** sync volume on slave speaker */
                    /** @removed: bt broadcast */
                }
                break;

            case MSG_PLAY_PAUSE:
                fs_comm_send_command(KC_PLAY_PAUSE, KC_EVENT_KEY_PRESSED);
                break;

            case MSG_BT_BROADCAST:
                /** @removed: BT broadcast */

                TimeoutSet(&tmrBroadcastPreventFault, 500);
                break;

            case MSG_PRE:
                fs_comm_send_command(KC_SKIP_PREVIOUS, KC_EVENT_KEY_PRESSED);
                break;

            case MSG_NEXT:
                fs_comm_send_command(KC_SKIP_NEXT, KC_EVENT_KEY_PRESSED);
                break;

            case MSG_SPEAKER_MODE:
                system_config.speaker_mode = (system_config.speaker_mode + 1) % SPEAKER_MODE_SUM;
                /** @removed: speaker mode */
                break;

            case MSG_MODE:
                CLR_FLAG( FLAG_IS_MODE_FROM_FS );
                break;

#if (CONFIG_ENABLE_STANDBY)
            case MSG_POWER:
                /** send command standby to FS module */
                /**
                if ( fs_comm_get_mode() != FS_MODE_STANDBY )
                {
                    fs_comm_send_command(KC_SPOTIFY_MODE, KC_EVENT_KEY_PRESSED);
                }
                */
                break;
#endif
            /** factory reset event id */
            case MSG_FACTORY_RESET:
                /** ignore if any re trigger from factory reset */
                // if (system_config.venicex_state == VENICEX_STATE_FACTORY_START)
                //     return;

                // ignore if there are any factory reset retrigger
                if ( (/*GET_FLAG( FLAG_FS_FACTORY_RESET_START ) && */!IsTimeout(&tmrFactoryTimeout)) 
                     || system_config.current_function == SYS_MODE_FACTORY_RESET ) 
                    return;

                SET_FLAG( FLAG_FS_FACTORY_RESET_START );
                fs_comm_send_command(KC_FACTORY_RESET, KC_EVENT_KEY_PRESSED);
                TimeoutSet(&tmrFactoryTimeout, 5000);
                break;

            default: break;
        }
    }

    /** saving to nvm memory handler (eeprom) */
    /** @removed: nvm  saving */

    subtask_bt_broadcast(&system_config);


    if ( GET_FLAG( FLAG_FS_FACTORY_RESET_START ) )
    {
        /** check factory reset status from venice x module */
        if ( fs_comm_get_factory_status() )
        {
            system_config.venicex_state = VENICEX_STATE_FACTORY_START;
            /** clear flag , this will be one execute */
            CLR_FLAG( FLAG_FS_FACTORY_RESET_START );
            // goto factory reset task 
            mainTaskState = TASK_STATE_STOP;
            system_config.current_function = SYS_MODE_FACTORY_RESET;
        }
    }


    /** just test for i2c polling data */
    /** wait here */
    if (IsTimeout(&tmrUpdateI2CReg))
    {
        TimeoutSet(&tmrUpdateI2CReg, 100);
        if ( !FLAG_COUNT_DIRECTION ) {
            counter = (counter + 1);
            if (counter == 30) FLAG_COUNT_DIRECTION = 1;
        }
        else
        {
            counter = (counter - 1);
            if (counter == 0) FLAG_COUNT_DIRECTION = 0;
        }
        /** just to set counter */
        read_reg->aux1 = (uint8_t) counter;
    }

}

void task_network_configuration(void *arg)
{
    /** used for save previous wifi state
     * if there is change from venice-x command
     * so this variable used for indicator (led) update
    */
    static uint8_t pre_wifi_state = 0xf;

    EventContext *ev = (EventContext *)(arg);

    do_change_task(&mainTaskState, ev);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;
            break;

        case TASK_STATE_INIT:
            if (fs_comm_get_wifi_status() == FS_WIFI_STATE_DISCONNECTED)
            {
                /** green blink */
                /** @removed: indicator */

                /** this routine needed when speaker disconnected from access point
                 * so user can still can use spotify & bluetooth mode
                 * if this routine not activated, mode still in bluetooth but the indicator already
                 * in spotify connected mode (led green)
                 * 
                 * see. CONFIG_WAIT_CHANGE_MODE_VENICEX config cause mode cannot change to spotify
                 * in task_spotify_connect
                */
                fs_comm_send_command(KC_SPOTIFY_MODE, KC_EVENT_KEY_PRESSED);
            } 
            else if(fs_comm_get_wifi_status() == FS_WIFI_SETUP_MODE)
            {

                /** @removed: indicator */
            }
            pre_wifi_state = fs_comm_get_wifi_status();
            mainTaskState = TASK_STATE_RUN;
            break;

        case TASK_STATE_RUN:
            /** waiting for wifi connection */
            if (fs_comm_get_wifi_status() == FS_WIFI_STATE_CONNECTED)
            {
                mainTaskState = TASK_STATE_STOP;
                system_config.current_function = get_next_mode(system_config.current_function);
            }

            if ( pre_wifi_state != fs_comm_get_wifi_status() )
            {
                pre_wifi_state = fs_comm_get_wifi_status();
                mainTaskState = TASK_STATE_INIT;
            }

            do_check_standby_and_setup_mode();

#if (CONFIG_NETWORK_ERROR_INDICATOR_ENABLE)
            /**
             * check the wifi reconnection status 
             * this may used to indicate if user apply wrong password in wifi setup mode
             * if this happen send indicator to user that something wrong
             * indicate with red blinking
            */
            if ( /*(fs_comm_get_error() & FS_ERR_WIFI_RECONNECTION)*/fs_comm_get_err_wifi_reconnection() == 1 )
            {
                /** set task to pause state */
                mainTaskState = TASK_STATE_WAIT;
                /** @removed: indicator */

            }
#endif // CONFIG_NETWORK_ERROR_INDICATOR_ENABLE            
            break;

        case TASK_STATE_WAIT:
            /** reserved */
            break;

        case TASK_STATE_PAUSE:

            break;

        case TASK_STATE_STOP:
            mainTaskState = TASK_STATE_IDLE;
            break;

        default: 
            break;
    }

    task_common(ev);

}

void task_idle(void *arg)
{
    
    EventContext *ev = (EventContext *)(arg);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;
            break;

        case TASK_STATE_INIT:
            mainTaskState = TASK_STATE_RUN;
            TimeoutSet(&tmrIdle, 1000);
            /** @removed: indicator */
            break;

        case TASK_STATE_RUN:
            if (IsTimeout(&tmrIdle))
            {
                mainTaskState = TASK_STATE_STOP;
                /** after idle, go to default function */
                system_config.current_function = CONFIG_DEFAULT_FUNCTION;
            }
            break;

        case TASK_STATE_PAUSE:
            /** reserved */
            break;

        case TASK_STATE_STOP:
            mainTaskState = TASK_STATE_IDLE;
            break;

        default: 
            break;
    }
}


/**
 * @brief task for factory reset
 * 
 * reset some parameter
 * - set default function to CONFIG_DEFAULT_FUNCTION
 * - set default system volume to DEFAULT_SYS_VOL
 *
 * send command factory reset to other module (Venice-X)     
 * note:
 * - if  CONFIG_ENABLE_NVM not enabled, so NVM use from VX
 *   VX value after factory:
 *   - volume = 8
*/
static void task_factory(void *arg)
{
    EventContext *ev = (EventContext *)(arg);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;
            break;

        case TASK_STATE_INIT:
            /** @removed: indicator */

#if (CONFIG_ENABLE_NVM)
            /** @removed: nvm */
#endif
            mainTaskState = TASK_STATE_RUN;
            break;

        case TASK_STATE_RUN:

            if (system_config.venicex_state == VENICEX_STATE_READY && fs_comm_get_factory_status() == 0)  
            {
                mainTaskState = TASK_STATE_STOP;

                /** set current module after factory reset
                 * to default function 
                */
                system_config.current_function = get_next_mode(system_config.current_function);
                
                /** set to the default value */
                system_app_set_default();

                /** send unmute to frontier */
                fs_comm_send_command(KC_UNMUTE, KC_EVENT_KEY_PRESSED);

            }
            break;

        case TASK_STATE_PAUSE:
            /* reserved for temporary*/
            break;

        case TASK_STATE_STOP:
            /** do deinit task */
            mainTaskState = TASK_STATE_IDLE;
            break;
        default: break;
    }

}

/**
 * @brief   task standby
*/
void task_standby(void *arg)
{
    EventContext *ev = (EventContext *)(arg);

    /** ignore all message except POWER */
    if (ev->eventId != MSG_POWER && ev->eventId != MSG_NONE)
    {
        return;
    }

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;
            /** @removed: power amp */

            if (fs_comm_get_mode() != FS_MODE_STANDBY)
            {
                /** send command standby to venicex module */
                fs_comm_send_command(KC_STANDBY, KC_EVENT_KEY_PRESSED );
            }
            break;

        case TASK_STATE_INIT:
            if (system_config.venicex_state == VENICEX_STATE_READY)
            {
                mainTaskState = TASK_STATE_RUN;
                /** @removed: indicator */
            }

            break;

        case TASK_STATE_RUN:
            if (ev->eventId == MSG_POWER)
            {
                mainTaskState = TASK_STATE_STOP;
            }
            break;

        case TASK_STATE_STOP:
            /** do deinit task */
            mainTaskState = TASK_STATE_IDLE;
    
            if (fs_comm_get_wifi_status() == FS_WIFI_SETUP_MODE)
            {
                system_config.current_function = SYS_MODE_SPOTIFY_CONNECT;      // current ST function will set to spotify --> redirect to 
                                                                                // task_network_configuration 
            }
            else
            {
                /**
                 * if not in setup mode, get last function 
                */
                system_config.current_function = system_config.pre_function;
                if (system_config.current_function == SYS_MODE_BT_A2DP)
                {
                    fs_comm_send_command(KC_BLUETOOTH_MODE, KC_EVENT_KEY_PRESSED);
                }
                else if (system_config.current_function == SYS_MODE_SPOTIFY_CONNECT)
                {
                    fs_comm_send_command(KC_SPOTIFY_MODE, KC_EVENT_KEY_PRESSED);
                }
            }

            /** @removed: power amp */
            break;

        default: break;
    }
    
}

/***
 * @brief   task for booting process
 *          waiting for other module (Venice X) in ready state 
 * @param   ev: event message 
*/
static void task_booting(void *arg)
{
    EventContext *ev = (EventContext *)(arg);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;
            /** @removed: power amp */
            break;

        case TASK_STATE_INIT:
            mainTaskState = TASK_STATE_RUN;
            /** set indicator for booting process */
            /** @removed: indicator */
            system_config.venicex_state = VENICEX_STATE_BOOTING_PROCESS;
            //TimeoutSet(&tmrWifiWaitConn, TIMEOUT_WAIT_CONNECTION);
            break;

        case TASK_STATE_RUN:
            /** wait first command from venice x 
             * see. communication_iface.c --> communication_fs_handler()
            */
            if (system_config.venicex_state == VENICEX_STATE_READY)
            {
                mainTaskState = TASK_STATE_STOP;
                TimeoutSet(&tmrWaitForWakeup, 2000);

                read_reg->boot_info.bit.ready = 1;
            }           
            break;

        case TASK_STATE_PAUSE:
            /** reserved */
            break;

        case TASK_STATE_STOP:
            if ( !IsTimeout(&tmrWaitForWakeup) )
                return;

            /** do deinit task */
            mainTaskState = TASK_STATE_IDLE;

#if (CONFIG_ENABLE_STANDBY)
#if (CONFIG_STANDBY_DEPEND_NETWORK_CONNECTION)
            if (  fs_comm_get_wifi_status() == FS_WIFI_STATE_CONNECTED )
            {
                system_config.current_function = SYS_MODE_SPOTIFY_CONNECT;
            }
            else if ( fs_comm_get_wifi_status() == FS_WIFI_SETUP_MODE )
            {
                /** goto task network configuration if state is in wifi setup mode */
                system_config.current_function = SYS_MODE_STANDBY;
            }
            else 
            {
                system_config.current_function = SYS_MODE_NETWORK_CONFIG;
            }
#else
            system_config.current_function = SYS_MODE_STANDBY;
#endif // end of CONFIG_STANDBY_DEPEND_NETWORK_CONNECTION
            
#else // else of CONFIG_ENABLE_STANDBY
            system_config.current_function = CONFIG_DEFAULT_FUNCTION;
#endif // end of CONFIG_ENABLE_STANDBY

            /** wakeup PA when not in standby mode */
            if ( system_config.current_function != SYS_MODE_STANDBY )
            {
                /** @removed: power amp */
            }
            break;

        default: break;
    }

    task_common(ev);
}


void task_bluetooth_a2dp(void *arg)
{
    /** use for save previous bt state
     * if there is change from venice-x command
     * so this variable used for indicator (led) update
    */
    static uint8_t pre_bt_state = 0xf;

    EventContext *ev = (EventContext *)(arg);

    do_change_task(&mainTaskState, ev);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;    
            break;

        case TASK_STATE_INIT:
#if (CONFIG_ROLLING_MODE)
            if ( IsTimeout(&tmrWaitChangeMode) )
#else
            if ( 1 )
#endif
            {
                /* check is change funtion trigger from module fs or not */
                if ( !GET_FLAG(FLAG_IS_MODE_FROM_FS) /*&& fs_comm_get_wifi_status() != FS_WIFI_SETUP_MODE*/)
                {
                    fs_comm_send_command(KC_BLUETOOTH_MODE, KC_EVENT_KEY_PRESSED);
#if (CONFIG_WAIT_CHANGE_MODE_VENICEX)
                    TimeoutSet(&tmrBlockingFunctionReplyVx, TIMEOUT_BLOCKING_FUNCTION_REPLY);
#endif
                }
                mainTaskState = TASK_STATE_RUN;
            }

            if (fs_comm_get_bt_status() == FS_BT_CONNECTED)
            {
                /** @removed: indicator */
            }
            else /** disconnected, discoverable*/
            {
                /** @removed: indicator */
            }
            pre_bt_state = fs_comm_get_bt_status();
            break;

        case TASK_STATE_RUN:
            /** there are some change in bt status */
            if ( pre_bt_state != fs_comm_get_bt_status() )
            {
                mainTaskState = TASK_STATE_INIT;
            }
            break;

        case TASK_STATE_PAUSE:
            /** resume task*/
            break;

        case TASK_STATE_STOP:
            /** do deinit task */
            mainTaskState = TASK_STATE_IDLE;
            break;
        default: break;
    }

    task_common(ev);
}



/**             
 * @brief do spotify task 
*/
void task_spotify_connect(void *arg)
{
    EventContext *ev = (EventContext *)(arg);

    do_change_task(&mainTaskState, ev);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            
            /** check network state, if no any wifi connection so
             * it will blink green till it connected sucessfully
            */
            if (fs_comm_get_wifi_status() == FS_WIFI_STATE_DISCONNECTED || \
                fs_comm_get_wifi_status() == FS_WIFI_SETUP_MODE)
            {
                system_config.current_function = SYS_MODE_NETWORK_CONFIG;
                mainTaskState = TASK_STATE_STOP;    /* exit */
            }
            else /* network (stay) connected */
            {
                mainTaskState = TASK_STATE_INIT;
            }
            break;

        case TASK_STATE_INIT:
            if (fs_comm_get_wifi_status() == FS_WIFI_STATE_CONNECTED)
            {
                /** @removed: indicator */

#if (CONFIG_ROLLING_MODE)
                if ( IsTimeout(&tmrWaitChangeMode) )
#else
                if ( 1 )
#endif
                {
                    /** check is change funtion trigger from module fs or not 
                     * @note: in setup mode, dont change to spotify, this will caused SSID dont appear!!
                    */
                    if ( !GET_FLAG(FLAG_IS_MODE_FROM_FS) && fs_comm_get_wifi_status() != FS_WIFI_SETUP_MODE)
                    {
                        fs_comm_send_command(KC_SPOTIFY_MODE, KC_EVENT_KEY_PRESSED);
#if (CONFIG_WAIT_CHANGE_MODE_VENICEX)
                        TimeoutSet(&tmrBlockingFunctionReplyVx, TIMEOUT_BLOCKING_FUNCTION_REPLY);
#endif
                    }
                    mainTaskState = TASK_STATE_RUN;
                }
            }
            break;

        case TASK_STATE_RUN:
            if (fs_comm_get_wifi_status() == FS_WIFI_STATE_CONNECTED)
            {
                TimeoutSet(&tmrWaitConn, TIMEOUT_SIMULASI_WIFI_CONN);
            }
            else /** disconnect or setup mode */
            {
                /** check timeout for 2 S */
                if (IsTimeout(&tmrWaitConn))
                {
                    system_config.current_function = SYS_MODE_NETWORK_CONFIG;
                    mainTaskState = TASK_STATE_STOP;    /* exit */
                }
            }
            break;

        case TASK_STATE_PAUSE:
            /** pause task */
            break;

        case TASK_STATE_STOP:
            /** do deinit task */
            mainTaskState = TASK_STATE_IDLE;
            break;
        default: break;
    }

    task_common(ev);

}


/** State Machine 
 *  @note run in main_looping
*/
void (*MainTaskRunState[SYS_MODE_NUM])(void *arg) = {
    task_idle,
    task_standby,
    task_booting,
    task_bluetooth_a2dp,
    task_spotify_connect,
    task_factory,
    task_network_configuration,
};
/**********************************************************/


/**
 * @brief init main task 
*/
void main_task_init(void)
{
    
    /** Communication Init */
    communication_iface_init();

    system_app_init();

}

void main_task_run(void *arg)
{
    
    /** handle communication for wifi and bluetooth module */
    communication_fs_handler(&msgSend);

    /** running state **/
    (*MainTaskRunState[system_config.current_function])((void*)&msgSend);
}


