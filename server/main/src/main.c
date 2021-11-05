#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "dht11.h"
#include "wifi.h"
#include "mqtt.h"
#include "nvs.h"

xSemaphoreHandle mutex_wifi_connection;
xSemaphoreHandle conexaoMQTTSemaphore;

void send_messages(void *params)
{
    if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
    {
        while (true)
        {
            mqtt_send_message("temperatura", "Eita nóis!", 1);
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }
}

void app_main()
{
    DHT11_init(GPIO_NUM_4);
    start_nvs();

    mutex_wifi_connection = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();

    wifi_start();

    if (xSemaphoreTake(mutex_wifi_connection, portMAX_DELAY))
    {
        ESP_LOGI("MQTT", "Wi-fi conected");
        mqtt_start();
    }

    xTaskCreate(&send_messages, "Send messages", 2048, NULL, 1, NULL);
}