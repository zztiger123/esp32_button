#ifndef __CUS_GPIO_H__
#define __CUS_GPIO_H__

#include "esp_err.h"
#include "esp_log.h"

#ifdef  __cplusplus
extern "C" {
#endif


#define ESP_INTR_FLAG_DEFAULT 0

typedef enum {
    KEY_SHORT_PRESS = 0,                 
    KEY_LONG_PRESS,                      
    KEY_ACCIDENTALLY_TRIGGERED
}GPIO_BUTTON_STATE;

uint32_t get_button_state(TickType_t ticks_to_wait);
void button_init(gpio_num_t button_gpio_num);

#ifdef __cplusplus
}
#endif

#endif/*!< __CUS_GPIO_H__ */