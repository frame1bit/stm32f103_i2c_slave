/**
 * @file communication_iface.c
 * @author c.e.s
 * @brief 
 * @version 0.1
 * @date 2024-06-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "communication_iface.h"

#define I2C_REGISTER_LEN    12

uint8_t I2C_Registers[I2C_REGISTER_LEN];
ReadRegister_t *read_reg;

/** extern variable */
extern TIMER tmrVolumeSync;
/** end of extern variable */

/** extern function */
/** end of extern function */

/***
 * @brief   update register for read
 */
void i2c_update_register_data(uint8_t *src_reg)
{
    int i = 0;

    for (i = 0; i < I2C_REGISTER_LEN; i++) {
        I2C_Registers[i] = src_reg[i];
    }
}

/***
 * @brief   parsing key command 
 * @param   none
 */
static void parsing_key_command(uint8_t data)
{
    switch( data ) {
        case KEY_CMD_UNMUTE:
            fs_comm_send_command(KC_UNMUTE, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_MUTE:
            fs_comm_send_command(KC_MUTE, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_SPOTIFY_MODE:
            fs_comm_send_command(KC_SPOTIFY_MODE, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_BLUETOOTH_MODE:
            fs_comm_send_command(KC_BLUETOOTH_MODE, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_VOLUME_UP:
            fs_comm_send_command(KC_VOLUME_UP, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_VOLUME_DOWN:
            fs_comm_send_command(KC_VOLUME_DOWN, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_PLAY_PAUSE:
            fs_comm_send_command(KC_PLAY_PAUSE, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_NEXT:
            fs_comm_send_command(KC_SKIP_NEXT, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_PREVIOUS:
            fs_comm_send_command(KC_SKIP_PREVIOUS, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_RESET_NETWORK:
            fs_comm_send_command(KC_RESET_NETWORK, KC_EVENT_KEY_PRESSED);
            break;

        case KEY_CMD_FACTORY_RESET:
            fs_comm_send_command(KC_FACTORY_RESET, KC_EVENT_KEY_PRESSED);
            break;

        default: break;
    }
}

/***
 * @brief   parsing volume set
 */
static void parsing_volume_command( uint8_t data )
{
    /** check value  */
    if (data > 32) return;
    fs_comm_send_command(KC_SET_VOLUME, data);
}


/***
 * @brief   process and parsing data received from i2c master
 * @param   void
 */
static void i2c_communication_process(I2C_Data_t *p)
{
    if (!p) return;

    /** check register pointer  */
    if (p->reg > /*I2C_REGISTER_LEN-1*/ 0x0F)
        return;

#if 0   /** this routine used when write -> store -> read back */
    /** store data based on register value */
    I2C_Registers[p->reg] = p->data;
#endif
    /** test received data */
    switch ( p->reg ) {
        case REG_SYSTEM_REQ:

            break;

        case REG_KEY_COMMAND:
            parsing_key_command( p->data );
            break;

        case REG_VOLUME_SET:
            parsing_volume_command( p->data );
            break;

        case REG_AUXILIARY1:
            if (p->data == 0x00) {
                // turn off the led
                HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, 0);
            }
            else if (p->data == 0x01) {
                // turn on the led
                HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, 1);
            }
            break;

        default: break;
    }
}


/***
 * @brief communication interface init
 * @param   none
*/
void communication_iface_init(void)
{
    /** init FS communication init */
    fs_comm_init();

    /** init I2C Slave */
    i2c_slave_init( i2c_communication_process , I2C_Registers);

    /** remap register, on future use read_reg */
    read_reg = I2C_Registers;
}


/**
 * @brief FS Venice X - ST Communication handler
 * call at main loop, waiting data
*/
int communication_fs_handler(EventContext *ev)
{
    /** send periodically to request system status
     * to venice x module 
    */
    fs_system_req();


    /** scan incomming data */
    if (!fs_comm_scan_data())
    {
        return;
    }

    /** process received data */
    FS.config.data0.byte = FS.rx_buffer[1];
    FS.config.data1.byte = FS.rx_buffer[2];
    FS.config.data2.byte = FS.rx_buffer[3];
    FS.config.data3.byte = FS.rx_buffer[4];

    system_config.venicex_state = VENICEX_STATE_READY;

    /** change detection */
    if (FS.config.data0.byte != FS.preConfig.data0.byte)
    {
        FS.preConfig.data0.byte = FS.config.data0.byte;
        /** mode status update, wifi and bluetooth 
         * update on task run state 
         * */  
        read_reg->wifi_status = FS.config.data0.bit.wifi_status;
        read_reg->bt_status = FS.config.data0.bit.bt_status;
    }
    else if (FS.config.data1.byte != FS.preConfig.data1.byte)
    {

        read_reg->spotify_status = FS.config.data1.bit.spotify_status;
        read_reg->mode = FS.config.data1.bit.mode;


        /** there is mode change */
        if (FS.config.data1.bit.mode != FS.preConfig.data1.bit.mode)
        {
            FS.preConfig.data1.bit.mode = FS.config.data1.bit.mode;

#if (FS_CONTROL_MODE)
            /***
             * @note: if (fs_comm_get_wifi_status() != FS_WIFI_SETUP_MODE)
             * used to prevent function change by FS module in setup mode
             * so default after power on will on spotify --> network setup mode
             * if this conditional removed: 
             * change function system_config.current_function = SYS_MODE_SPOTIFY_CONNECT in task_standby will 
             * override by FS state
            */
            if (fs_comm_get_wifi_status() != FS_WIFI_SETUP_MODE)
            {
                ev->eventId = MSG_SET_MODE;
            }
#endif
        }

        FS.preConfig.data1.byte = FS.config.data1.byte;
    }
    else if (FS.config.data2.byte != FS.preConfig.data2.byte)
    {
        read_reg->volume.byte = FS.config.data2.byte;

#if (FS_CONTROL_VOLUME)
        if (FS.preConfig.data2.bit.volume != FS.config.data2.bit.volume)
        {
            FS.preConfig.data2.bit.volume = FS.config.data2.bit.volume;

#if (FS_VOLUME_TRICK)
            if ( IsTimeout(&tmrVolumeSync) )  
#endif
            {
                system_config.system_volume = FS.config.data2.bit.volume;
                if ( GET_FLAG(FLAG_FS_ALLOWED_VOLUME_CHANGE)  )
                {
                    ev->eventId = MSG_FS_VOL_SET;
                }
                SET_FLAG( FLAG_FS_ALLOWED_VOLUME_CHANGE );
            }
        }
#endif     
        FS.preConfig.data2.byte = FS.config.data2.byte;
    }
    else if (FS.config.data3.byte != FS.preConfig.data3.byte)
    {
        FS.preConfig.data3.byte = FS.config.data3.byte;
        read_reg->error_status = FS.config.data3.byte;
        /* status update */
    }

    /** end of receive data process */

    return 0;

}



/**
 * @brief   universal function to send command from ST to Action / FS Module
 * @param   
*/
