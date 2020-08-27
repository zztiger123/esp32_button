#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "esp_log.h"

#include "cus_gpio.h"

static const char *TAG = "CUS_GPIO";

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

uint32_t get_button_state(TickType_t ticks_to_wait) 
{
    uint32_t io_num;
    uint32_t current_time = 0;
    bool press_key = false;
    bool lift_key  = false;

    while(1) {
        if(xQueueReceive(gpio_evt_queue, &io_num, ticks_to_wait)) {
            ESP_LOGI(TAG, "GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));

            if (gpio_get_level(io_num) == 0) {
                press_key = true;
                current_time = esp_timer_get_time();
            } else if (press_key) {
                lift_key = true;
                current_time = esp_timer_get_time() - current_time;
            }

            if (press_key && lift_key) {
                press_key = false;
                lift_key = false;
            
                if (current_time > 3000000) {
                    return KEY_LONG_PRESS;
                } else if (current_time < 50000) {
                    return KEY_ACCIDENTALLY_TRIGGERED;
                } else {
                    return KEY_SHORT_PRESS;
                }
            }
        }
    }

    return -1;
}

void button_init(gpio_num_t button_gpio_num)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_ANYEDGE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1 << button_gpio_num;
    //disable pull-down mode
    io_conf.pull_down_en = 1;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);


    //change gpio intrrupt type for one pin
    gpio_set_intr_type(button_gpio_num, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(button_gpio_num, gpio_isr_handler, (void*) button_gpio_num);
}