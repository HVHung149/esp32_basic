#ifndef _EPPROM_H_
#define _EPPROM_H_
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "i2c_protocol.h"
#define SLAY_ADDRESS 0x50
    typedef uint8_t BYTE;
    typedef union
    {
        int data_int;
        BYTE data_byte[4];
    } data_int_byte;

    void i2c_send_eeprom(uint16_t page, int data);
    void i2c_read_eeprom(uint16_t page);
#ifdef __cplusplus
}
#endif

#endif