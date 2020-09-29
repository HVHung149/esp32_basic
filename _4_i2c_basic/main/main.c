/* 
    The ESP32 chip features 40 physical GPIO pads
    NOTES:
        - Note that GPIO6-11 are usually used for SPI flash  
        - GPIO34-39 can only be set as input mode and do not have 
        software pullup or pulldown functions.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
typedef uint8_t BYTE;
#define SLAY_ADDRESS      0x27
#define MASTER_FREQUENTLY 100000
#define MASTER_SDA_IO    4
#define MASTER_SCL_IO    19
#define I2C_PORT_NUM     1
#define I2C_MASTER_BUFFER_TX 0
#define I2C_MASTER_BUFFER_RX 0
#define ACK_EN 1
#define ACK_DIS 0


static esp_err_t i2c_master_init()
{
    i2c_config_t config;
    config.master.clk_speed = MASTER_FREQUENTLY;
    config.mode = I2C_MODE_MASTER;
    config.scl_io_num = MASTER_SCL_IO;
    config.sda_io_num = MASTER_SDA_IO;
    config.sda_pullup_en = 1;
    config.scl_pullup_en = 1;
    i2c_param_config(I2C_PORT_NUM,&config);
    return i2c_driver_install(I2C_PORT_NUM,I2C_MODE_MASTER,I2C_MASTER_BUFFER_RX,I2C_MASTER_BUFFER_TX,0);
}
static esp_err_t master_write_byte(uint8_t* data, int data_size)
{
    i2c_cmd_handle_t cmd;
    //step 1: linl
    cmd=i2c_cmd_link_create();
    //step 2: start send
    i2c_master_start(cmd);
    //step 3: master_write_byte 
        i2c_master_write_byte(cmd,(SLAY_ADDRESS << 1) | I2C_MASTER_WRITE, ACK_EN);// write
    //step 4: master write char 
    i2c_master_write(cmd, data, data_size,ACK_EN);
    //step 5: stop i2c link
    i2c_master_stop(cmd);
    //step 6 send 
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT_NUM,cmd,portMAX_DELAY);
    if(ret == ESP_ERR_TIMEOUT)
    {
        printf("SEND TimeOut ERROR CODE: %03X \n",ESP_ERR_TIMEOUT);
    }
    else if(ret == ESP_OK)
    {
        printf("SEND OKE \n");
    }
    else if(ret == ESP_FAIL)
    {
        printf("SEND FAIL ERROR CODE: %03X \n",ESP_FAIL);
    }
    else
    {
        printf("NO ACK, SENSOR, ERROR CODE: %s \n",esp_err_to_name(ret));
    }
    //step 7 delete link
    i2c_cmd_link_delete(cmd);
    return ret;
}
static BYTE* lcd_set_command(BYTE command)
{
    BYTE* arr_command = (BYTE *)malloc(2 * sizeof(BYTE));
    arr_command[0]=command & 0xF0;
    arr_command[1]= (command<<4) & 0xF0;
    return arr_command;
} 

static BYTE* lcd_set_char (BYTE character)
{
    BYTE* arr_command = (BYTE *)malloc(2 * sizeof(BYTE));
    arr_command[0]= character & 0xF0;
    arr_command[1]= (character<<4) & 0xF0;
    return arr_command;
}

static void i2c_send_command(BYTE command)
{
    BYTE* data = lcd_set_command(command);
    BYTE data_upper= data[0];
   // printf("data upper %x \n ",data_upper);
    BYTE data_lower =data[1];
   // printf("data lower %x \n ",data_lower);
    //send upper
    BYTE data_send[4];
    data_send[0]=data_upper| 0x04;
    data_send[1]=data_upper;
    data_send[2]=data_lower| 0x04;
    data_send[3]=data_lower| 0x08;
    for(int i=0;i<4;i++)
    {
        int ret = master_write_byte(data_send+i,1);
    }
}
static void i2c_send_char(BYTE character)
{
    BYTE* data = lcd_set_char(character);
    BYTE data_upper= data[0];
    BYTE data_lower =data[1];
    //send upper
    BYTE data_send[4];
    data_send[0]=data_upper| 0x0D;
    data_send[1]=data_upper| 0x09;
    data_send[2]=data_lower| 0x0D;
    data_send[3]=data_lower| 0x09;
    for(int i=0;i<4;i++)
    {
        master_write_byte(data_send+i,1);
    }
}
static void i2c_send_string(char* string_data)
{
    while(*string_data!= '\0')
    {
        i2c_send_char(*string_data);
        string_data ++;
    }
}
static void LCD_goto_XY(int X, int Y)
{

}
static void LCD_INIT()
{
   i2c_send_command (0x33);
   vTaskDelay(10/portTICK_PERIOD_MS);

   i2c_send_command (0x32);
   vTaskDelay(50/portTICK_PERIOD_MS);

   i2c_send_command (0x28);
   vTaskDelay(50/portTICK_PERIOD_MS);

   i2c_send_command (0x01);
   vTaskDelay(50/portTICK_PERIOD_MS);

   i2c_send_command(0x06);
   vTaskDelay(50/portTICK_PERIOD_MS);

   i2c_send_command(0x0C);
   vTaskDelay(50/portTICK_PERIOD_MS);

   i2c_send_command(0x02);
   vTaskDelay(50/portTICK_PERIOD_MS);
}

void app_main(void)
{
  i2c_master_init();
  printf("i2c_master_init success \n");
  LCD_INIT();
  printf("lcd_init success \n");
  i2c_send_string("Hung Quynh");
  printf("lcd_string success \n");

}
