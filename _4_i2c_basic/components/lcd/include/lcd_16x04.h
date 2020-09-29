#ifndef _LCD_16X04_H_
#define  _LCD_16X04_H_
#ifdef __cplusplus
extern "C" {
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
#define LCD_ADDRESS 0x27
BYTE* lcd_set_command(BYTE command);
BYTE* lcd_set_char (BYTE character);
void i2c_send_command(BYTE command);
void i2c_send_char(BYTE character);
void i2c_send_string(char* string_data);
void LCD_INIT();
void LCD_goto_XY(int X, int Y);
#ifdef __cplusplus
}
#endif

#endif /* RS485_TRANSPORT_H_ */