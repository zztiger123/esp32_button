/* GPIO BUTTON example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "cus_gpio.h"


#define GPIO_BUTTON     0

static const char *TAG = "button_main";

static void gpio_button_example(void* arg)
{
    uint32_t button_state = 0;

    while (1) {
        button_state = get_button_state(portMAX_DELAY);
        if (-1 == button_state) {
            vTaskDelete(NULL);
        }

        switch (button_state) {

        case KEY_SHORT_PRESS:
            ESP_LOGI(TAG, "KEY_SHORT_PRESS triggered");
            break;
        
        case KEY_LONG_PRESS:
            ESP_LOGI(TAG, "KEY_LONG_PRESS triggered");
            break;

        case KEY_ACCIDENTALLY_TRIGGERED:
            ESP_LOGW(TAG, "KEY_ACCIDENTALLY_TRIGGERED");
            break;

        default:
            ESP_LOGW(TAG, "CAN'T GET BUTTON STATUS");
        }
    }

    vTaskDelete(NULL);
}

void app_main()
{
    button_init(GPIO_BUTTON);
    //start gpio task
    xTaskCreate(gpio_button_example, "gpio_button_example", 2048, NULL, 10, NULL);


    
}

