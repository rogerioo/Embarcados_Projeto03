#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include <button.h>

extern xQueueHandle buttonMQ;

void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pin = (int)args;

    xQueueSendFromISR(buttonMQ, &pin, NULL);
}

void button_init()
{

    gpio_pad_select_gpio(BUTTON_PIN);

    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);

    gpio_pulldown_en(BUTTON_PIN);

    gpio_pullup_dis(BUTTON_PIN);

    gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_ANYEDGE);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, (void *)BUTTON_PIN);
}