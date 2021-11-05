#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include <tasks.h>
#include "mqtt.h"

extern xSemaphoreHandle conexaoMQTTSemaphore;

void temperature_task(void *params)
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