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
#define PIN_OUT 5
#define PIN_TEST_OUTPUT (1ULL<<PIN_OUT)
void app_main(void)
{
   gpio_config_t gpio_cf;
   gpio_cf.pin_bit_mask = PIN_TEST_OUTPUT;
   gpio_cf.intr_type = GPIO_INTR_DISABLE;
   gpio_cf.mode = GPIO_MODE_OUTPUT;
   gpio_cf.pull_down_en= GPIO_PULLDOWN_DISABLE;
   gpio_cf.pull_up_en= GPIO_PULLUP_ENABLE;
   gpio_config(&gpio_cf);
   while(1)
    {
        printf("Turning on the LED\n");
        gpio_set_level(PIN_OUT,1);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    	printf("Turning off the LED\n");
        gpio_set_level(PIN_OUT,0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
