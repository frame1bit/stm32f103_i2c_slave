/**
 * file: communication_iface.h
 * 
*/
#ifndef COMMUNICATION_IFACE_H
#define COMMUNICATION_IFACE_H

#include "main.h"
#include "drivers/uart/fs_comm.h"
#include "i2c_comm.h"
#include "sys_app.h"
#include "app_event_message.h"

/** function prototype */
void communication_iface_init(void);
int communication_fs_handler(EventContext *ev);

void i2c_update_register_data( uint8_t *src_reg);
/** end of prototype function */

/** extern resource */
extern ReadRegister_t *read_reg;
/** end of extern resource  */

#endif /** COMMUNICATION_IFACE_H */
