/**
 * @file fs_comm.c
 * @author c_e (cosmas.eric.septian@polytron.co.id)
 * @brief communcation driver for frontier silicon Venice X (FS4340)
 * @version 0.1
 * @date 2023-05-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdint.h>
#include "fs_comm.h"
#include "main.h"
#include "app_config.h"

/** uart instance*/
#define FS_UART     (USART1)

FrontierSilicon_t FS;

TIMER tmrSysReq;

void fs_comm_init(void)
{
    FS.uart_handler = FS_UART;
    
    /** init circular buffer */
    MCUCircular_Config(&FS.cbCtx, FS.circular_buffer, FS_CIRCULAR_BUFF_LEN);

    LL_USART_EnableIT_RXNE(FS.uart_handler);
}

/**
 * @brief uart2 receive handler
 * 
 * @note    called on USART2_IRQHandler() function on file stm32f1xx_it.c 
*/
void FS_USART_IRQ_Handler(void)
{
	uint8_t temp;

    /** if any data received */
    if (LL_USART_IsActiveFlag_RXNE(FS.uart_handler))
    {
    	temp = LL_USART_ReceiveData8(FS.uart_handler);
        MCUCircular_PutData(&FS.cbCtx, &temp, 1);
    }

    /** if any error occur */
    if (LL_USART_IsActiveFlag_ORE(FS.uart_handler))
    {

    }
}

static void fs_uart_send_byte(uint8_t data)
{
    uint32_t timeout = 10;

    while(!LL_USART_IsActiveFlag_TXE(FS.uart_handler))
    {
        /* Check Systick counter flag to decrement the time-out value */
        if (LL_SYSTICK_IsActiveCounterFlag()) 
        { 
            if(timeout-- == 0)
            {
                break;
            }
        } 
    }

    LL_USART_ClearFlag_TC(FS.uart_handler); 
    LL_USART_TransmitData8(FS.uart_handler, data);
}

/**
 * The data sent from Polytron MCU to VeniceX module are named key command. 
 * The data length of the key command is limited to 2 bytes.
*/
int fs_comm_send_command(uint8_t command, uint8_t data)
{
    uint8_t tx_data[FS_KEY_COMMAND_PACKET_LEN];
    uint8_t i = 0;

    tx_data[0] = FS_HEADER1;
    tx_data[1] = FS_HEADER2;
    tx_data[2] = FS_HEADER3;
    tx_data[3] = FS_KEY_COMMAND_DATA_LEN;
    tx_data[4] = command;
    tx_data[5] = data;

    /** send uart data */
    for(i = 0; i < FS_KEY_COMMAND_PACKET_LEN; i += 1)
    {
        fs_uart_send_byte(tx_data[i]);
    }

    return 0; // dummy return
}


/**
 * @brief process fs uart data 
 * 
 * @note    handler circular buffer content
 *          call at loop
*/
uint8_t fs_comm_scan_data(void)
{
	int i, len;
    static int rx_index = -3;
    uint8_t data_out = 0;

    TimeoutSet(&FS.timeout, 100);

    while( (len = MCUCircular_GetDataLen(&FS.cbCtx)) > 0 )
    {
        if (IsTimeout(&FS.timeout))
        {
            return 0;
        }
        /** get uart data */
        if (MCUCircular_GetData(&FS.cbCtx, &data_out, 1) == 0)
        {}
        switch(rx_index)
        {
            case -3:
                if (data_out == FS_HEADER1) rx_index = -2;
                break;
            case -2:
                if (data_out == FS_HEADER2) rx_index = -1;
                break;
            case -1:
                if (data_out == FS_HEADER3) rx_index = 0;
                break;

            default:
                FS.rx_buffer[rx_index++] = data_out;
                if ( FS.rx_buffer[0] > FS_SYSTEM_STATUS_DATA_LEN )
                {
                    /** not valid length */
                    rx_index = -3;
                    return 0;
                }
                /** check len on buffer[0] */
                if(rx_index >= FS_SYSTEM_STATUS_DATA_LEN + 1)
                {
                    rx_index = -3;
                    return 1;
                }
                break;
        }
    }

    return 0;
}

uint8_t fs_comm_get_wifi_status(void)
{
    return (FS.config.data0.bit.wifi_status);
}

uint8_t fs_comm_get_bt_status(void)
{
    return (FS.config.data0.bit.bt_status);
}

uint8_t fs_comm_get_mode(void)
{
    return (FS.config.data1.bit.mode);
}

uint8_t fs_comm_get_spotify_status(void)
{
    return (FS.config.data1.bit.spotify_status);
}

uint8_t fs_comm_get_volume(void)
{
    return (FS.config.data2.bit.volume);
}

uint8_t fs_comm_get_mute_state(void)
{
    return (FS.config.data2.bit.mute);
}

uint8_t fs_comm_get_error(void)
{
    return (FS.config.data3.byte);
}

/**
 * @brief get error related to network reconection fail
 *        Data3 - b[0] : Wifi Reconection failed
 *        this error indicate that previous access point not found within 
 *        timeout so this error indication show
*/
uint8_t fs_comm_get_err_wifi_reconnection(void)
{
    return (FS.config.data3.bit.wifi_reconnect_fail);
}

/***
 * @brief convert volume with mute status
 * @param   mute: (0=unmute 1=mute)
 * @param   vol: volume value (min:0 max: 32)
 * @return byte: volume with 7th bit as mute bit
*/
uint8_t fs_comm_mute_volume_convert(uint8_t mute, uint8_t vol)
{
    uint8_t temp = 0;

    temp = ( (((mute) ? 1 : 0) << 7) | vol & 0x7F);
    return (temp);
}

/***
 * @brief get factory reset status 
 * @note    todo: check frontier bit allocation !!!
*/
uint8_t fs_comm_get_factory_status(void)
{
    return (FS.config.data3.bit.factory_reset_status);
}

/***
 * @brief   reset FS data
*/
void fs_reset_default(void)
{
    FS.config.data0.byte = 0;
    FS.config.data1.byte = 0;
    FS.config.data2.byte = 0;
    FS.config.data3.byte = 0;
}


/**
 * send system request status every n milisecond
*/
void fs_system_req(void)
{
    if (IsTimeout(&tmrSysReq))
    {
        TimeoutSet(&tmrSysReq, FS_SYSTEM_REQ_TIME);
        fs_comm_send_command(KC_SYSTEM_STATUS_REQ, 0x00);
    }
}
