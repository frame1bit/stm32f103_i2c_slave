#ifndef SYS_APP_H
#define SYS_APP_H


typedef struct
{
	uint16_t		eventId;
}EventContext;


enum
{
	SYS_MODE_IDLE = 0x00,
    SYS_MODE_STANDBY,
    SYS_MODE_BOOTING,
	SYS_MODE_BT_A2DP,
	SYS_MODE_SPOTIFY_CONNECT,
    SYS_MODE_FACTORY_RESET,
    SYS_MODE_NETWORK_CONFIG,
	SYS_MODE_NUM,
};

#define SUB_MODE(cfunc)    ((cfunc==SYS_MODE_STANDBY) || (cfunc==SYS_MODE_BOOTING) || (cfunc==SYS_MODE_FACTORY_RESET) ||    \
                             (cfunc==SYS_MODE_FACTORY_RESET))
#define MAIN_MODE(cfunc)   ( (cfunc==SYS_MODE_BT_A2DP) || (cfunc==SYS_MODE_SPOTIFY_CONNECT))

enum
{
    TASK_STATE_IDLE = 0x00,
    TASK_STATE_INIT,
    TASK_STATE_RUN,
    TASK_STATE_WAIT,
    TASK_STATE_PAUSE,
    TASK_STATE_STOP,
};

/** bluetooth broadcast role */
enum
{
    BT_BROADCAST_OFF_ROLE_STANDALONE = 0x00,
    BT_BROADCAST_ON_ROLE_MASTER,
    BT_BROADCAST_ON_ROLE_SLAVE,
    BT_BROADCAST_ROLE_SUM,
};

/** factory reset state */
enum VENICEX_STATE
{
    VENICEX_STATE_NONE = 0,
    VENICEX_STATE_READY,
    VENICEX_STATE_FACTORY_START,
    VENICEX_STATE_BOOTING_PROCESS,
};


/**
 * speaker mode
*/
enum
{
    SPEAKER_MODE_MIX_LR = 0,
    SPEAKER_MODE_MONO_L,
    SPEAKER_MODE_MONO_R,
    SPEAKER_MODE_SUM,
};

typedef struct
{
    /**
     * bt broadacast state for 
     * 0: off
     * 1: master
     * 2: slave
    */
    uint8_t role : 3;
    
    /** state on/off !! obsolete !! Change to rolling state 140623 */
    uint8_t state : 1;
    /**
     * used to update state for free running function call
    */
    uint8_t update_state : 1;

    /** reserved */
    uint8_t resv : 3;
}BT_Broadcast_state;


enum
{
    BT_EVENT_IDLE = 0,
    BT_EVENT_PAIRING,
    BT_EVENT_CONNECTED,
    BT_EVENT_DISCONNECTED,
};

enum
{
    WIFI_EVENT_IDLE = 0,
    WIFI_EVENT_CONNECTING,
    WIFI_EVENT_CONNECTED,
    WIFI_EVENT_DISCONNECTED,
};


typedef struct sys_app
{
	uint8_t system_volume;
    uint8_t pre_system_volume;
	uint8_t current_function;
    uint8_t pre_function;
    BT_Broadcast_state bt_broadcast;

    /** state to indicate booting or factory reset prosess
     * see. enum FACTORY_STATE and BOOTING_STATE
    */
    uint8_t venicex_state;

    /**
     * speaker mode
    */
    uint8_t speaker_mode;

    uint16_t blink_periode;

    uint8_t bt_evt;
    uint8_t wifi_evt;

}SystemConfig_t;


/**
 * nvm parameter, mirror to SystemConfig_t
*/
typedef struct
{
    uint8_t system_volume;
    uint8_t current_function;
}SysNvmParam_t;

typedef union
{
    struct
    {
        uint32_t bit0 : 1;
        uint32_t bit1 : 1;
        uint32_t bit2 : 1;
        uint32_t bit3 : 1;
        uint32_t bit4 : 1;
        uint32_t bit5 : 1;
        uint32_t bit6 : 1;
        uint32_t bit7 : 1;
        uint32_t bit8 : 1;
        uint32_t bit9 : 1;
        uint32_t bit10 : 1;
        uint32_t bit11 : 1;
        uint32_t bit12 : 1;
        uint32_t bit13 : 1;
        uint32_t bit14 : 1;
        uint32_t bit15 : 1;
        uint32_t bit16 : 1;
        uint32_t bit17 : 1;
        uint32_t bit18 : 1;
        uint32_t bit19 : 1;
        uint32_t bit20 : 1;
        uint32_t bit21 : 1;
        uint32_t bit22 : 1;
        uint32_t bit23 : 1;
        uint32_t bit24 : 1;
        uint32_t bit25 : 1;
        uint32_t bit26 : 1;
        uint32_t bit27 : 1;
        uint32_t bit28 : 1;
        uint32_t bit29 : 1;
        uint32_t bit30 : 1;
        uint32_t bit31 : 1;
    };
    uint32_t mask;
} gFlag_t;

/** extern variable */
extern SystemConfig_t system_config;
extern gFlag_t gflag_sys;
/** end of extern variable */

/** flag for saving to nvm */
#define	FLAG_SAVE_NVM	        gflag_sys.bit0

/** flag for power on state */
#define FLAG_FIRST_POWERON      gflag_sys.bit1

/** flag to determine bt broadcaster ready
 * currently use counting timer methode
*/
#define FLAG_BT_BROADCASTER_RDY gflag_sys.bit2

/**
 * flag to determine function change source
 * is from FS or from local key (press button mode)
*/
#define FLAG_IS_MODE_FROM_FS    gflag_sys.bit3

/***
 * flag to determine fatory reset monitor status
 * from fs module to start and send change task to 
 * factory reset
*/
#define FLAG_FS_FACTORY_RESET_START gflag_sys.bit4


/***
 * flag used to prevent at first system status from venice not
 * trigger volume change (set indicator)
 * make it just set the current system volume
*/
#define FLAG_FS_ALLOWED_VOLUME_CHANGE gflag_sys.bit5

/**
 * flag to determine bluetooth CSB role change
 * this flag is mirror from system_config.bt_broadcast.update_state
 * state: 
 * 1 = CSB ON (role master or role slave)
 * 0 = CSB OFF (standalone)
*/
#define FLAG_CSB_PRE_STATE             gflag_sys.bit6


/** @brief flag to determine CSB scanning for device status 
 * both master role or slave role
 * 1: scanning progress
 * 0: scanning done
 * */
#define FLAG_CSB_SCANNING_STATUS        gflag_sys.bit7


/** flag to determine execution status of broadcast volume sync timeout */
#define FLAG_CSB_SYNC_VOL_TIMEOUT       gflag_sys.bit8


#define FLAG_POWER_AMP_AUTO_RECOVER     gflag_sys.bit9


#define FLAG_COUNT_DIRECTION            gflag_sys.bit10

#define	GET_FLAG(x)		(x)
#define	SET_FLAG(x)		(x = 1)
#define	CLR_FLAG(x)		(x = 0)

/**************************************/
#define	NVM_SAVE_PERIODE	(250)

/** protype function */
void system_app_dbg(uint8_t data);
void system_app_init(void);
void system_app_set_default(void);
/** end of prototype function */

#endif /*SYS_APP_H*/
