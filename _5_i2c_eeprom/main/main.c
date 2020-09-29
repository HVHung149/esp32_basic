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
#define SLAY_ADDRESS      0x50
#define MASTER_FREQUENTLY 100000
#define MASTER_SDA_IO    4
#define MASTER_SCL_IO    19
#define I2C_PORT_NUM     1
#define I2C_MASTER_BUFFER_TX 0
#define I2C_MASTER_BUFFER_RX 0
#define ACK_EN 1
#define ACK_DIS 0
typedef union 
{
    int data_int;
    BYTE data_byte[4];
}data_int_byte;

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
static esp_err_t master_write_byte(BYTE* page_upper,BYTE* page_lower ,uint8_t* data, int data_size)
{
    printf("write to eeprom\n");
    i2c_cmd_handle_t cmd;
    //step 1: linl
    cmd=i2c_cmd_link_create();
    //step 2: start send
    i2c_master_start(cmd);
    //step 3: master_write_byte 
    i2c_master_write_byte(cmd,(SLAY_ADDRESS << 1) | I2C_MASTER_WRITE, ACK_EN);// write
    //step 4: master write char 
    i2c_master_write(cmd, page_upper, 1,ACK_EN);printf(" page upper : %02X ",*page_upper);
    i2c_master_write(cmd, page_lower, 1,ACK_EN);printf(" page lower : %02X ",*page_lower);
    i2c_master_write(cmd, data, data_size,ACK_EN);printf(" data_send : %02X ",*data);
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
static esp_err_t master_read_byte(BYTE* page_upper,BYTE* page_lower ,uint8_t* data_read, int data_size)
{
    i2c_cmd_handle_t cmd;
    //step 1: linl
    cmd = i2c_cmd_link_create();
    //step 2: start send
    i2c_master_start(cmd);
    //step 3: master_write_byte 
    i2c_master_write_byte(cmd,(SLAY_ADDRESS << 1) | I2C_MASTER_WRITE, ACK_EN);// write
    //step 4: master write char 
    i2c_master_write(cmd, page_upper, 1,ACK_EN);printf(" page upper : %02X ",*page_upper);
    i2c_master_write(cmd, page_lower, 1,ACK_EN);printf(" page lower : %02X ",*page_lower);
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
    vTaskDelay(30 / portTICK_RATE_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SLAY_ADDRESS << 1 | I2C_MASTER_READ, ACK_EN);
    i2c_master_read_byte(cmd, data_read, 0);
    printf("byte received %02X",*data_read);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
static void i2c_send_eeprom(uint16_t page, int data)
{
    for(int i=0;i<4;i++)
    {
        BYTE page_upper = page >> 8;
        BYTE page_lower = page & 0xFF;
        data_int_byte temp;
        temp.data_int=data;
        master_write_byte(&page_upper,&page_lower,temp.data_byte+i,1);
        page++;
    }   
}
static void i2c_read_eeprom(uint16_t page )
{
   data_int_byte read;
   for(int i=0;i<4;i++)
    {
        BYTE page_upper = page >> 8;
        BYTE page_lower = page & 0xFF;
        master_read_byte(&page_upper,&page_lower,read.data_byte+i,1);
        printf(" data: %02X \n",read.data_byte[i]);
        page++;
    }   
    printf("%d\n",read.data_int);
}
void app_main(void)
{
  i2c_master_init();
  i2c_send_eeprom(1,12);
  i2c_read_eeprom(1);

}
