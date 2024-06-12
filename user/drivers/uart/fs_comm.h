/**
 * @file fs_comm.h
 * @author c_e (cosmas.eric.polytron.co.id)
 * @brief communcation driver for frontier silicon Venice X (FS4340)
 * @version 0.1
 * @date 2023-05-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef FS_COMM_H
#define FS_COMM_H

#include <stdint.h>
#include "utility/circular_buffer.h"
#include "utility/timeout.h"

/** FS packet format 
 * [0XFF] + ['F'] + ['S'] + [len] + [D0] + [D1] 
*/
#define FS_HEADER1  (0xFF)
#define FS_HEADER2  ('F')   //0x46
#define FS_HEADER3  ('S')   //0x53

#define FS_KEY_COMMAND_DATA_LEN     (2) /* currenly limited to max 2*/
#define FS_KEY_COMMAND_PACKET_LEN   (6)

#define FS_SYSTEM_STATUS_DATA_LEN   (4) /** curently limited to 4 data (byte)*/

/**
 * The data sent from Polytron MCU to VeniceX module are named ??key command??. The data length of
 * the key command is limited to 2 bytes.
 * The data sent from VeniceX module to Polytron MCU are named ??system status??. The data length of
 * the system status is limited to 4 bytes
*/
/** key command data_0*/
#define KC_SYSTEM_STATUS_REQ    (0x00)
#define KC_CLEAR_ERROR_FLAGS    (0x01)
#define KC_UNMUTE               (0x02)
#define KC_MUTE                 (0x03)
#define KC_SPOTIFY_MODE         (0x04)
#define KC_BLUETOOTH_MODE       (0x05)
#define KC_SET_VOLUME           (0x06)
#define KC_VOLUME_UP            (0x07)
#define KC_VOLUME_DOWN          (0x08)
#define KC_PLAY_PAUSE           (0x09)
#define KC_SKIP_NEXT            (0x0A)
#define KC_SKIP_PREVIOUS        (0x0B)
#define KC_RESET_NETWORK        (0x0C)
#define KC_FACTORY_RESET        (0x0D)
#define KC_STANDBY              (0x0E)

/** key command data_1 */
#define KC_EVENT_KEY_PRESSED    (0x80)

#define FS_SYSTEM_REQ_TIME      (1000)   /** set to 1000 ms */

/**
 * !!!
 * option to enable or disable control from venice-x
 * 0: enable
 * 1: disable
*/
#define FS_CONTROL_MODE             (1)
#define FS_CONTROL_VOLUME           (1)
#define FS_CONTROL_SPOTIFY_STATUS   (1)

/** prevent transition effect volume reply from 
 * VX module
*/
#define FS_VOLUME_TRICK             (1)
/** end of venice-x control*/

typedef union
{
    struct
    {
        uint8_t wifi_status : 4;    // 0x0: Wi-Fi disconnected
                                    // 0x1: Wi-Fi connected
                                    // 0x2: Wi-Fi setup mode
                                    // 0x3-0xF: Reserved

        uint8_t bt_status : 4;      // 0x0: Bluetooth disconnected
                                    // 0x1: Bluetooth connected
                                    // 0x2: Bluetooth re-connecting
                                    // 0x3: Discoverable mode
                                    // 0x4-0xF: Reserved
    }bit;
    uint8_t byte;
}Data0_t;

typedef union
{
    struct
    {
        uint8_t mode : 4;           // 0x0: No mode
                                    // 0x1: Bluetooth mode
                                    // 0x2: Spotify mode
                                    // 0x3: Standby Mode
                                    // 0x4-0xF: Reserved

        /**
         * b[4-7] are only set in Spotify mode. These bits are 0 when in BT mode
        */
        uint8_t spotify_status : 4; // 0x0: Spotify account logged out
                                    // 0x1: Spotify account logged in,
                                    //      waiting for user to press the play
                                    //      key to pull-context.
                                    // 0x2: Spotify paused
                                    // 0x3: Spotify playing
                                    // 0x4-0xF: Reserved
    }bit;
    uint8_t byte;
}Data1_t;

typedef union
{
    struct
    {
        uint8_t volume : 7;         // 0x00-0x20: Volume step (0-32)
                                    // 0x21-0xFF: Reserved
        uint8_t mute : 1;           // mute status 
                                    // 0: unmute
                                    // 1: mute
    }bit;
    uint8_t byte;
}Data2_t;

/**
 * When error happens, the error bit of Data 3 will keep 1, until 
 * a) Clear Error Flags keycommand with the error bit is received, or 
 * b) Factory reset command is received, or c) The error is handled.
*/
typedef union
{
    struct
    {
        uint8_t wifi_reconnect_fail : 1;    // 1: Wifi reconection failed 
        uint8_t webpage_upgrade_fail : 1;   // 1: webpage upgrade failed
        uint8_t factory_reset_status : 1;   // 1: factory reset start
        uint8_t resv_3_3 : 1;               // reserved
        uint8_t resv_3_4 : 1;               // reserved
        uint8_t resv_3_5 : 1;               // reserved
        uint8_t resv_3_6 : 1;               // reserved
        uint8_t resv_3_7 : 1;               // reserved
    }bit;
    uint8_t byte;
}Data3_t;

typedef struct
{
    Data0_t data0;         // system status data 0
    Data1_t data1;         // system status data 1
    Data2_t data2;         // system status data 2
    Data3_t data3;         // system status data 3
}FS_SystemStatus;

/********************** FS state ***************************/
/* wifi state */
#define FS_WIFI_STATE_DISCONNECTED  0x00
#define FS_WIFI_STATE_CONNECTED     0x01
#define FS_WIFI_SETUP_MODE          0x02

/** bluetooth state */
#define FS_BT_DISCONNECTED          0x00
#define FS_BT_CONNECTED             0x01
#define FS_BT_DISCOVERABLE          0x02

/** FS mode */
#define FS_NO_MODE                  0x00
#define FS_MODE_BLUETOOTH           0x01
#define FS_MODE_SPOTIFY             0x02
#define FS_MODE_STANDBY             0x03

/* spotify status */
#define FS_SPOTIFY_ACCOUNT_LOGOUT   0x00
#define FS_SPOTIFY_ACCOUNT_LOGIN    0x01
#define FS_SPOTIFY_PAUSED           0x02
#define FS_SPOTIFY_PLAYING          0x03

/** fs error status */
#define FS_ERR_WIFI_RECONNECTION    (1<<0)
#define FS_ERR_WEBPAGE_UPG_FAIL     (1<<1)
/** update for venicex firmware upgrade */
#define FS_ERR_STATUS_UPGRADE       (1<<2)


/******************** Comm Resource **********************/
#define FS_SYSTEM_STATUS_BUFF_LEN   6
#define FS_CIRCULAR_BUFF_LEN        12*2

typedef struct
{
    uint32_t *uart_handler;
    uint8_t circular_buffer[FS_CIRCULAR_BUFF_LEN];
    MCU_CIRCULAR_CONTEXT cbCtx;
    uint8_t rx_buffer[FS_SYSTEM_STATUS_BUFF_LEN];
    TIMER timeout;
    FS_SystemStatus config, preConfig;
}FrontierSilicon_t;

/** prototype function */
void fs_comm_init(void);
void FS_USART_IRQ_Handler(void);
int fs_comm_send_command(uint8_t command, uint8_t data);
uint8_t fs_comm_scan_data(void);

uint8_t fs_comm_get_wifi_status(void);
uint8_t fs_comm_get_bt_status(void);
uint8_t fs_comm_get_mode(void);
uint8_t fs_comm_get_spotify_status(void);
uint8_t fs_comm_get_volume(void);
uint8_t fs_comm_get_error(void);
uint8_t fs_comm_get_err_wifi_reconnection(void);
uint8_t fs_comm_get_factory_status(void);
void fs_reset_default(void);
void fs_system_req(void);
/* end of prototype function */

/** extern resource */
//extern FS_SystemStatus fs_system_config;
extern FrontierSilicon_t FS;
/* end of extern resource */

#endif /*FS_COMM_H*/
