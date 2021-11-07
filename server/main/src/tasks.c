#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>

#include <message.h>
#include <dht11.h>
#include <tasks.h>
#include <mqtt.h>

extern xSemaphoreHandle conexaoMQTTSemaphore;
extern xQueueHandle buttonMQ;

extern void IRAM_ATTR gpio_isr_handler(void *args);

void temperature_task(void *params)
{
    if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
    {
        struct dht11_reading data;

        float mean_temperature = 0.0f;
        float mean_humidity = 0.0f;
        int counter = 0;

        while (true)
        {
            data = DHT11_read();
            vTaskDelay(2000 / portTICK_PERIOD_MS);

            if (data.status != DHT11_OK)
                continue;

            ESP_LOGI("TEMP_TASK", "Temperature: %d", data.temperature);
            ESP_LOGI("TEMP_TASK", "Humidity: %d", data.humidity);

            mean_temperature += (float)data.temperature;
            mean_humidity += (float)data.humidity;

            if (counter < 5)
            {
                counter++;
                continue;
            }

            char temperature[12];
            char humidity[12];

            mean_temperature /= 5.0f;
            mean_humidity /= 5.0f;

            sprintf(temperature, "%02.0f", mean_temperature);
            sprintf(humidity, "%02.0f", mean_humidity);

            message temperature_message = {"data", temperature};
            message humidity_message = {"data", humidity};

            mqtt_send_message("temperatura", message_to_json(temperature_message), 1);
            mqtt_send_message("umidade", message_to_json(humidity_message), 1);

            ESP_LOGI("TEMP_TASK", "Sent temperature: %02.0f", mean_temperature);
            ESP_LOGI("TEMP_TASK", "Sent humidity: %02.0f", mean_humidity);

            counter = 0;
            mean_temperature = 0.0f;
            mean_humidity = 0.0f;
        }
    }
}

void button_task(void *params)
{
    int pin;

    while (true)
    {
        if (xQueueReceive(buttonMQ, &pin, portMAX_DELAY))
        {
            int state = gpio_get_level(pin);

            ESP_LOGI("BUTTON_TASK", "Button pressed");

            if (state == 1)
            {
                gpio_isr_handler_remove(pin);

                while (gpio_get_level(pin) == state)
                {
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }

                message button_message = {"button", "Activate"};

                mqtt_send_message("estado", message_to_json(button_message), 1);

                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_isr_handler_add(pin, gpio_isr_handler, (void *)pin);

                continue;
            }
        }
    }
}