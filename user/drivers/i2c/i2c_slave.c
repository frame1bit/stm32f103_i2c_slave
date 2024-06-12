/**
 * @file i2c_slave.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "i2c_slave.h"

I2C_Slave_t i2c_slave;
I2C_Data_t *i2c_data;

/** this pointer must be point to received buffer in upper layer */
static uint8_t *pRegister;

/**
 * @brief   I2C Slave init
 * @param   cb      function callback to process received data from master I2C
 * @param   *pReg   array buffer to store data that receive from master
 * 
 * @return  none
 */
void i2c_slave_init(process_callback cb, uint8_t *pReg)
{
    i2c_slave.process_callback = cb;
    i2c_data = (I2C_Data_t*) i2c_slave.rx_data;
    pRegister = pReg;
}

/**
 * @brief
 *
 * @param hi2c
 */
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_EnableListen_IT(hi2c);
}

/**
 * @brief The Address Callback is called when the address sent by master matches with the slave address.
 *
 * @param hi2c
 * @param TransferDirection
 * @param AddrMatchCode
 */
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    /** transmit direction, from master to slave  */
    if (TransferDirection == I2C_DIRECTION_TRANSMIT)
    {
        i2c_slave.rx_data[0] = 0;       // reset buffer receiver
        i2c_slave.rx_count = 0;
        HAL_I2C_Slave_Sequential_Receive_IT(hi2c,
        									i2c_slave.rx_data+i2c_slave.rx_count,
											1,
											I2C_FIRST_FRAME);
    }
    else /* I2C_DIRECTION_RECEIVE receive direction, from slave to master  */
    {
        i2c_slave.tx_count = 0;
        i2c_slave.start_position = i2c_slave.rx_data[0];
        i2c_slave.rx_data[0] = 0;
        HAL_I2C_Slave_Seq_Transmit_IT(hi2c, 
                                        pRegister + i2c_slave.start_position + i2c_slave.tx_count,
                                        2,                  /** len of data receive */
                                        I2C_FIRST_FRAME     /** just for first frame after i2c start */
                                    );
    }
}

/**
 * @brief completed callback 
 *          write format:
 *          [I2C Address]   ->  [register]  ->  [   data    ]
 *          |    byte   |       |  byte  |      |    Word   |
 * 
 *          read format
 *          []
 * @param hi2c 
 */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{

    i2c_slave.rx_count += 1;
	if (i2c_slave.rx_count < RX_SIZE)
	{
		if (i2c_slave.rx_count == RX_SIZE-1)
		{
            /** send NACK if in last size */
			HAL_I2C_Slave_Sequential_Receive_IT(hi2c, 
                                                i2c_slave.rx_data+i2c_slave.rx_count, 
                                                1, 
                                                I2C_LAST_FRAME);
		}
		else
		{
            /** send ACK till size of data */
			HAL_I2C_Slave_Sequential_Receive_IT(hi2c, 
                                                i2c_slave.rx_data+i2c_slave.rx_count,  
                                                1, 
                                                I2C_NEXT_FRAME);
		}
	}

    /** process data if count fullfiled */
	if (i2c_slave.rx_count == RX_SIZE)
	{
		i2c_slave.process_callback(i2c_data);
	}
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    i2c_slave.errcode = HAL_I2C_GetError(hi2c);
    if (i2c_slave.errcode == 0x04) // AF error
    {
        if (i2c_slave.tx_count == 0) // error is while slave is receiving
        {
            i2c_slave.bytes_received = i2c_slave.rx_count - 1;
            i2c_slave.rx_count = 0; 
            i2c_slave.process_callback(i2c_data);
        }
        else // error while slave is transmitting
        {
            i2c_slave.bytes_transmitted = i2c_slave.tx_count - 1;   
            i2c_slave.tx_count = 0;     // reset tx count for next operation
        }
    }
    /* BERR Error commonly occurs during the Direction switch
     * Here we the software reset bit is set by the HAL error handler
     * Before resetting this bit, we make sure the I2C lines are released and the bus is free
     * I am simply reinitializing the I2C to do so
     */
    else if (i2c_slave.errcode == 0x01) // BERR Error
    {
        HAL_I2C_DeInit(hi2c);
        HAL_I2C_Init(hi2c);
        memset(i2c_slave.rx_data, '\0', RX_SIZE);
        i2c_slave.rx_count = 0;
    }
    HAL_I2C_EnableListen_IT(hi2c);
}


/***
 * @brief   i2c slave tx completed callback
 * @param   hi2c    i2c handler
 */
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    i2c_slave.tx_count += 1;
    HAL_I2C_Slave_Seq_Transmit_IT(hi2c, 
                                pRegister + i2c_slave.start_position + i2c_slave.tx_count, 
                                1, 
                                I2C_NEXT_FRAME);
}
