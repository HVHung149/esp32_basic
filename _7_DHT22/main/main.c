#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "rom/ets_sys.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
static char *TAG = "viethung";
typedef uint8_t BYTE;
#define PIN_OUT 5
#define PIN_TEST_OUTPUT (1ULL << PIN_OUT)
#define MAX_INTERVAL 2000
#define MAX_BYTE_READ 5
#define DHT_TIMEOUT_ERROR -1
#define mode_ouput(x) gpio_set_direction(x, GPIO_MODE_OUTPUT)
#define mode_input(x) gpio_set_direction(x, GPIO_MODE_INPUT)
#define low(x) gpio_set_level(x, 0)
#define high(x) gpio_set_level(x, 1)
#define get(x) gpio_get_level(x)
#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2
float humidity=0;
float temperature=0;
void errorHandler(int response)
{
    switch (response)
    {

    case DHT_TIMEOUT_ERROR:
        ESP_LOGE(TAG, "Sensor Timeout\n");
        break;

    case DHT_CHECKSUM_ERROR:
        ESP_LOGE(TAG, "CheckSum error\n");
        break;

    case DHT_OK:
        break;

    default:
        ESP_LOGE(TAG, "Unknown error\n");
    }
}

int getSignalLevel(int usTimeOut, bool state)
{

    int uSec = 0;
    while (gpio_get_level(5) == state)
    {

        if (uSec > usTimeOut)
            return -1;

        ++uSec;
        ets_delay_us(1); // uSec delay
    }

    return uSec;
}


int readDHT()
{
    int uSec = 0;

    uint8_t dhtData[MAX_BYTE_READ];
    uint8_t byteInx = 0;
    uint8_t bitInx = 7;

    for (int k = 0; k < MAX_BYTE_READ; k++)
        dhtData[k] = 0;

    // == Send start signal to DHT sensor ===========

    gpio_set_direction(PIN_OUT, GPIO_MODE_OUTPUT);

    // pull down for 3 ms for a smooth and nice wake up
    gpio_set_level(PIN_OUT, 0);
    ets_delay_us(3000);

    // pull up for 25 us for a gentile asking for data
    gpio_set_level(PIN_OUT, 1);
    ets_delay_us(25);

    gpio_set_direction(PIN_OUT, GPIO_MODE_INPUT); // change to input mode

    // == DHT will keep the line low for 80 us and then high for 80us ====

    uSec = getSignalLevel(85, 0);
    ESP_LOGD(TAG, "Response = %d", uSec);
    if (uSec < 0)
        return DHT_TIMEOUT_ERROR;

    // -- 80us up ------------------------

    uSec = getSignalLevel(85, 1);
    ESP_LOGD(TAG, "Response = %d", uSec);
    if (uSec < 0)
        return DHT_TIMEOUT_ERROR;

    // == No errors, read the 40 data bits ================

    for (int k = 0; k < 40; k++)
    {

        // -- starts new data transmission with >50us low signal

        uSec = getSignalLevel(56, 0);
        if (uSec < 0)
            return DHT_TIMEOUT_ERROR;

        // -- check to see if after >70us rx data is a 0 or a 1

        uSec = getSignalLevel(75, 1);
        if (uSec < 0)
            return DHT_TIMEOUT_ERROR;

        // add the current read to the output data
        // since all dhtData array where set to 0 at the start,
        // only look for "1" (>28us us)

        if (uSec > 40)
        {
            dhtData[byteInx] |= (1 << bitInx);
        }

        // index to next byte

        if (bitInx == 0)
        {
            bitInx = 7;
            ++byteInx;
        }
        else
            bitInx--;
    }

    // == get humidity from Data[0] and Data[1] ==========================

    humidity = dhtData[0];
    humidity *= 0x100; // >> 8
    humidity += dhtData[1];
    humidity /= (float)10; // get the decimal

    // == get temp from Data[2] and Data[3]

    temperature = dhtData[2] & 0x7F;
    temperature *= 0x100; // >> 8
    temperature += dhtData[3];
    temperature /= (float)10;

    if (dhtData[2] & 0x80) // negative temp, brrr it's freezing
        temperature *= -1;

    // == verify if checksum is ok ===========================================
    // Checksum is the sum of Data 8 bits masked out 0xFF

    if (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF))
        return DHT_OK;

    else
        return DHT_CHECKSUM_ERROR;
}
void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    //gpio_set_direction(GPIO_NUM_14,GPIO_MODE_OUTPUT);
    // while (1)
    // {
    //     high(5);
    //     vTaskDelay(100 / portTICK_PERIOD_MS);
    //     low(5);
    //     vTaskDelay(100 / portTICK_PERIOD_MS);
    // }
    // gpio_set_pull_mode(GPIO_NUM_5, GPIO_PULLUP_ENABLE);
    // gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT_OUTPUT);
    // while(1)
    // {
    //     cnt++;
    //     ESP_LOGI(TAG,"cnt: %d",cnt);
    //     ets_delay_us(1000);
    // }
    ESP_LOGI(TAG, "starting");
    while(1)
    {
    int test=readDHT();
    errorHandler(test);
    printf("temperature: %f and huminity: %f \n ",temperature,humidity);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}