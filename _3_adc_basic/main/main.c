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
#include "driver/adc.h"
#include "esp_adc_cal.h"
void app_main(void)
{
    int read_adc;
    adc2_config_channel_atten(ADC2_CHANNEL_7,ADC_ATTEN_0db);
    esp_err_t r= adc2_get_raw(ADC2_CHANNEL_7,ADC_WIDTH_12Bit,&read_adc);
    //still function 
    if(r==ESP_OK)
    {
        printf("%d \n ",read_adc);

    } 
    else if(r == ESP_ERR_TIMEOUT)
    {
        printf("adc 2 used by wifi\n");
    }
    
}
