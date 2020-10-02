
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#define MOSI_IO 22
#define MISO_IO 19
#define CLK_IO 21
#define CS_IO 5
#define BYTE_SYNC_COMMAND 0xF8
#define BYTE_SYNC_DATA 0xFA
typedef uint8_t BYTE;
static const char *SPI_TAG = "spi_test";

void SPI_LCD_INIT(spi_device_handle_t handle)
{
    ESP_LOGD(SPI_TAG, ">> test_spi_task");
    spi_bus_config_t bus_config;
    bus_config.sclk_io_num = CLK_IO;  // CLK (LCD'S EN PIN)
    bus_config.mosi_io_num = MOSI_IO; // MOSI (LCD'S RW PIN)
    bus_config.miso_io_num = MISO_IO; // MISO (NOT USE)
    bus_config.quadwp_io_num = -1;    // Not used
    bus_config.quadhd_io_num = -1;    // Not used
    bus_config.intr_flags = 0;
    ESP_LOGI(SPI_TAG, "... Initializing bus.");
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus_config, 1));

    spi_device_interface_config_t dev_config;
    dev_config.address_bits = 0;
    dev_config.command_bits = 0;
    dev_config.dummy_bits = 0;
    dev_config.mode = 1;
    dev_config.duty_cycle_pos = 0;
    dev_config.cs_ena_posttrans = 0;
    dev_config.cs_ena_pretrans = 0;
    dev_config.clock_speed_hz = 10000;
    dev_config.spics_io_num = CS_IO;
    dev_config.flags = 0;
    dev_config.queue_size = 1;
    dev_config.pre_cb = NULL;
    dev_config.post_cb = NULL;
    ESP_LOGI(SPI_TAG, "... Adding device bus.");
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev_config, &handle));
}
void SPI_send_byte(BYTE *data, spi_device_handle_t handle)
{
    spi_transaction_t trans_desc;
    trans_desc.addr = 0;
    trans_desc.cmd = 0;
    trans_desc.flags = 0;
    trans_desc.length = 8 * 3; //sync upper lower
    trans_desc.rxlength = 0;
    trans_desc.tx_buffer = data;
    trans_desc.rx_buffer = data;
    ESP_LOGI(SPI_TAG, "... Transmitting.");
    ESP_ERROR_CHECK(spi_device_transmit(handle, &trans_desc));

    // ESP_LOGI(SPI_TAG, "... Removing device.");
    // ESP_ERROR_CHECK(spi_bus_remove_device(handle));

    // ESP_LOGI(SPI_TAG, "... Freeing bus.");
    // ESP_ERROR_CHECK(spi_bus_free(HSPI_HOST));
    ESP_LOGI(SPI_TAG, "tranmissted");
}
void send_command(BYTE sync_command, BYTE command, spi_device_handle_t handle)
{
    BYTE *data = (BYTE *)malloc(3);
    gpio_set_level(GPIO_NUM_14,1);
    data[0] = sync_command;
    data[1] = command & 0xF0;
    data[2] = (command << 4) & 0xF0;
    SPI_send_byte(data, handle);
    gpio_set_level(GPIO_NUM_14,0);
}
void send_data(BYTE sync, BYTE data_send, spi_device_handle_t handle)
{
    BYTE *data = (BYTE *)malloc(3);
    gpio_set_level(GPIO_NUM_14,1);
    data[0] = sync ;
    data[1] = data_send & 0xF0;
    data[2] = (data_send << 4) & 0xF0;
    SPI_send_byte(data, handle);
    gpio_set_level(GPIO_NUM_14,0);
}
void send_string(char * str,spi_device_handle_t handle)
{
    while(*str!='\0')
    {
        send_data(BYTE_SYNC_DATA,(BYTE)*str,handle);
        str++;  
    }
}
void LCD_INIT(spi_device_handle_t handle)
{
    ESP_LOGI(SPI_TAG, "lcd wait");
    vTaskDelay(40 / portTICK_PERIOD_MS);

    ESP_LOGI(SPI_TAG, "lcd 0x30");
    send_command(BYTE_SYNC_COMMAND, 0x30, handle);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ESP_LOGI(SPI_TAG, "lcd 0x30");
    send_command(BYTE_SYNC_COMMAND, 0x30, handle);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ESP_LOGI(SPI_TAG, "lcd 0x01");
    send_command(BYTE_SYNC_COMMAND, 0x01, handle);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ESP_LOGI(SPI_TAG, "lcd 0x06");
    send_command(BYTE_SYNC_COMMAND, 0x06, handle);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ESP_LOGI(SPI_TAG, "lcd 0x0E");
    send_command(BYTE_SYNC_COMMAND, 0x0E, handle);
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
static void test_spi_task(spi_device_handle_t handle)
{

    vTaskDelete(NULL);
}
void app_main(void)
{
    spi_device_handle_t handle;
    esp_log_level_set(SPI_TAG, ESP_LOG_VERBOSE);
    ESP_LOGD(SPI_TAG, ">> test_spi_task");
    spi_bus_config_t bus_config = {
        .sclk_io_num = CLK_IO,  // CLK (LCD'S EN PIN)
        .mosi_io_num = MOSI_IO, // MOSI (LCD'S RW PIN)
        .miso_io_num = MISO_IO, // MISO (NOT USE)
        .quadwp_io_num = -1,    // Not used
        .quadhd_io_num = -1     // Not used
    };

    ESP_LOGI(SPI_TAG, "... Initializing bus.");
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus_config, 1));

    spi_device_interface_config_t dev_config = {
        .address_bits = 0,
        .command_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_posttrans = 0,
        .cs_ena_pretrans = 0,
        .clock_speed_hz = 10000,
        .spics_io_num = CS_IO,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL};

    ESP_LOGI(SPI_TAG, "... Adding device bus.");
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev_config, &handle));
    // char data[3];
    // spi_transaction_t trans_desc;
    // trans_desc.addr = 0;
    // trans_desc.cmd = 0;
    // trans_desc.flags = 0;
    // trans_desc.length = 3 * 8;
    // trans_desc.rxlength = 0;
    // trans_desc.tx_buffer = data;
    // trans_desc.rx_buffer = data;
    // data[0] = 0x12;
    // data[1] = 0x34;
    // data[2] = 0x56;

    // ESP_LOGI(SPI_TAG, "... Transmitting.");
    // ESP_ERROR_CHECK(spi_device_transmit(handle, &trans_desc));

    // ESP_LOGI(SPI_TAG, "... Removing device.");
    // ESP_ERROR_CHECK(spi_bus_remove_device(handle));

    // ESP_LOGI(SPI_TAG, "... Freeing bus.");
    // ESP_ERROR_CHECK(spi_bus_free(HSPI_HOST));
    // ESP_LOGD(SPI_TAG, "<< test_spi_task");
    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT);
    //gpio_set_level(GPIO_NUM_12, 0);
    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
    //gpio_set_level(GPIO_NUM_14, 1);
    gpio_set_level(GPIO_NUM_14,0);
    LCD_INIT(handle);
    // send_data(BYTE_SYNC_DATA, 0x41, handle);
    send_string("viet hung",handle);
}