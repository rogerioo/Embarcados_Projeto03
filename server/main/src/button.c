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

void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pin = (int)args;

    xQueueSendFromISR(buttonMQ, &pin, NULL);
}

void button_init()
{
    ESP_LOGI(BUTTON_TAG, "Initalizing button GPIO");

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
    clock_t begin = 0;
    int pin;

    while (true)
    {
        if (xQueueReceive(buttonMQ, &pin, portMAX_DELAY))
        {
            int state = gpio_get_level(pin);

            if (state == 0)
            {
                ESP_LOGI(BUTTON_TAG, "Button pressed: starting clock");

                begin = clock();

                message button_message = {BUTTON_TAG, "Activate"};

                mqtt_send_message("estado", message_to_json(button_message), 1);

                continue;
            }

            if (begin == 0)
                continue;

            clock_t pressed_time = clock() - begin;

            int diference_seconds = pressed_time / CLOCKS_PER_SEC;

            message button_message = {BUTTON_TAG, "Deactivate"};

            mqtt_send_message("estado", message_to_json(button_message), 1);

            if (diference_seconds >= 3)

            {
                ESP_LOGI(BUTTON_TAG, "Button released: resetting ESP32");

                clean_nvs_partition();
                esp_restart();
            }

            ESP_LOGI(BUTTON_TAG, "Button released: nothing to do");
        }
    }
}