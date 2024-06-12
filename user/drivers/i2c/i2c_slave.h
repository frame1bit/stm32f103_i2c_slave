/**
 * @file i2c_slave.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <stdint.h>
#include "main.h"

#define RX_SIZE 2


/** data mapping struct */
typedef struct __attribute__((packed))
{
    uint8_t reg;
    uint16_t data;
} I2C_Data_t;

typedef
struct _i2c_slave
{
    uint8_t bytes_transmitted;
    uint8_t bytes_received;
    uint8_t tx_count;
    uint8_t rx_count;
    uint8_t start_position;
    uint8_t rx_data[RX_SIZE];
    uint32_t errcode;
    void (*process_callback)(I2C_Data_t *data);

} I2C_Slave_t;


typedef void (*process_callback)(I2C_Data_t *);

/** prototype function */
void i2c_slave_init(process_callback cb, uint8_t *pReg);
/** end of prototype function  */

#endif /** end of I2C_SLAVE_H */