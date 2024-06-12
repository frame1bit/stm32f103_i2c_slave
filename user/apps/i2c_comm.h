/**
 * @file i2c_comm.h
 * @author cosmas e.s
 * @brief   header file for i2c communication
 * @version 0.1
 * @date 2024-06-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef I2C_COMM_H
#define I2C_COMM_H


#include "drivers/i2c/i2c_slave.h"


/** register list based on frontier silicon */
#define REG_SYSTEM_REQ      0x00
#define REG_ERROR_FLAG      0x01
#define REG_KEY_COMMAND     0x02
/** volume setting
 * 0x00 - 0x20
 */
#define REG_VOLUME_SET      0x03


#define REG_AUXILIARY1      0x0A
#define REG_AUXILIARY2      0x0B

/** key command definition */
#define KEY_CMD_UNMUTE          0x01
#define KEY_CMD_MUTE            0x02
#define KEY_CMD_SPOTIFY_MODE    0x03
#define KEY_CMD_BLUETOOTH_MODE  0x04
#define KEY_CMD_VOLUME_UP       0x05
#define KEY_CMD_VOLUME_DOWN     0x06
#define KEY_CMD_PLAY_PAUSE      0x07
#define KEY_CMD_NEXT            0x08
#define KEY_CMD_PREVIOUS        0x09
#define KEY_CMD_RESET_NETWORK   0x0A
#define KEY_CMD_FACTORY_RESET   0x0B
/** end of key command definition  */


typedef union
{
    struct
    {
        uint8_t ready : 1;
        uint8_t rebooting : 1;
        uint8_t resetting : 1;
        uint8_t reserved : 4;
    } bit;
    uint8_t byte;
} BootInfo_t;

typedef union
{
    struct
    {
        uint8_t volume: 7;
        uint8_t mute_state: 1;
    } bit;
    uint8_t byte;
} VolumeInfo_t;

typedef
struct
{
    // reg 0
    uint8_t id;                     // firmware ID

    // reg 1
    BootInfo_t boot_info;           // boot information
                                    // readiness status

    // reg 2
    uint8_t wifi_status;            // 0x0: Wi-Fi disconnected
                                    // 0x1: Wi-Fi connected
                                    // 0x2: Wi-Fi setup mode
                                    // 0x3-0xF: Reserved

    // reg 3
    uint8_t bt_status;              // 0x0: Bluetooth disconnected
                                    // 0x1: Bluetooth connected
                                    // 0x2: Discoverable mode
                                    // 0x3-0xF: Reserved

    // reg 4
    uint8_t mode;                   // 0x0: No mode
                                    // 0x1: Bluetooth mode
                                    // 0x2: Spotify mode
                                    // 0x3: Standby mode
                                    // 0x4-0xF: Reserved

    // reg 5
    uint8_t spotify_status;         // 0x0: Spotify account logged out
                                    // 0x1: Spotify account logged in,
                                    //      waiting for user to press the play
                                    //      key to pull-context.
                                    // 0x2: Spotify paused
                                    // 0x3: Spotify playing
                                    // 0x4-0xF: Reserved

    // reg 6
    VolumeInfo_t volume;            // b[0-6]   0 ~ 32
                                    // b[7]:    mute status 
                                    //          0: unmute
                                    //          1: mute

    // reg 7
    uint8_t error_status;           // b[0] = 1: wifi reconnection failed
                                    // b[1] = 1: Webpage upgrade failed
                                    // b[2] = 1: factory reset will start
                                    // b[3-7] : reserved

    // reg 8
    uint8_t aux1;                   // auxiliary data1
    
    // reg 9
    uint8_t aux2;                   // auxiliary data2

} ReadRegister_t;

/** prototype function */

/** end of prototype function  */

#endif  /** end of I2C_COMM_H */
