/* 
    each time occures interrupt pull up then print hey i see you 
    NOTES: use delay for button that makes smooth 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#define PIN_IN 19
#define PIN_TEST_INPUT (1ULL<<PIN_IN)
#define PIN_OUT  2
#define PIN_TEST_OUTPUT (1ULL<<PIN_OUT)
#define ESP_INTR_FLAG_DEFAULT 0
static xSemaphoreHandle sema_handle = NULL;
uint8_t i =0;
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    gpio_set_level(PIN_OUT,i%2);
    i++;
    static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(sema_handle,xHigherPriorityTaskWoken);  
}
static void TaskForSayHey()
{
    while(1)
    {
    if(sema_handle!=NULL)
    {
        if(xSemaphoreTake(sema_handle,portMAX_DELAY)==pdTRUE)
        {
            printf("hey i see you \n");
            //xSemaphoreGive(sema_handle);
        }
        else
        {
            printf("cant go to this taskForSayHey \n");
        }
    }
    }
}

void app_main(void)
{
   sema_handle=xSemaphoreCreateBinary();
   if(sema_handle ==NULL)
   {
       printf("create semaphore fail \n");
   }
   else{
       printf("create semaphore successfully \n");
   }

    gpio_set_direction(GPIO_NUM_2,GPIO_MODE_OUTPUT);
   gpio_config_t gpio_cf;
    
   gpio_cf.pin_bit_mask = PIN_TEST_INPUT;
   gpio_cf.intr_type = GPIO_INTR_POSEDGE;
   gpio_cf.mode = GPIO_MODE_INPUT;
   gpio_config(&gpio_cf);
   //install gpio isr service
   gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
   //hook isr handler for specific gpio pin
   gpio_isr_handler_add(PIN_IN, gpio_isr_handler, (void*) PIN_IN);
   
   xTaskCreate(TaskForSayHey,"active_interrupt",1024,NULL,5,NULL);
}
