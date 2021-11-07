#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <sdkconfig.h>
#include <esp_log.h>

#include <button.h>
#include <tasks.h>
#include <dht11.h>
#include <wifi.h>
#include <mqtt.h>
#include <nvs.h>
#include <led.h>

xSemaphoreHandle mutex_wifi_connection;
xSemaphoreHandle conexaoMQTTSemaphore;
xQueueHandle buttonMQ;

void app_main()
{
    DHT11_init(GPIO_NUM_4);
    button_init();
    led_init();

    start_nvs();

    mutex_wifi_connection = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    buttonMQ = xQueueCreate(10, sizeof(int));

    wifi_start();

    if (xSemaphoreTake(mutex_wifi_connection, portMAX_DELAY))
    {
        ESP_LOGI("MAIN", "Wi-fi conected");
        mqtt_start();
    }

    xTaskCreate(temperature_task, "Temperature Deamon", 2048, NULL, 1, NULL);
    xTaskCreate(button_task, "Button Task", 2048, NULL, 1, NULL);
}