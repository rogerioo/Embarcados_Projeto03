#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <message.h>
#include <dht11.h>
#include <tasks.h>
#include <mqtt.h>

extern xSemaphoreHandle conexaoMQTTSemaphore;

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