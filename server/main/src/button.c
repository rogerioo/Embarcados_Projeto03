#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <esp_log.h>
#include <time.h>

#include <message.h>
#include <button.h>
#include <mqtt.h>
#include <nvs.h>

extern xQueueHandle buttonMQ;

static void IRAM_ATTR gpio_isr_handler(void *args)
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

void button_task(void *params)
{
    clock_t begin = clock();
    int pin;

    while (true)
    {
        if (xQueueReceive(buttonMQ, &pin, portMAX_DELAY))
        {
            int state = gpio_get_level(pin);

            if (state == 0)
            {
                begin = clock();

                message button_message = {"button", "Activate"};

                mqtt_send_message("estado", message_to_json(button_message), 1);

                continue;
            }

            clock_t pressed_time = clock() - begin;

            int diference_seconds = pressed_time / CLOCKS_PER_SEC;

            message button_message = {"button", "Deactivate"};

            mqtt_send_message("estado", message_to_json(button_message), 1);

            if (diference_seconds >= 3)

            {
                clean_nvs_partition();
                esp_restart();
            }
        }
    }
}