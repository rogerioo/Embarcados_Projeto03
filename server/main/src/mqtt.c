#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_netif.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

#include <lwip/sockets.h>
#include <lwip/dns.h>
#include <lwip/netdb.h>

#include <cJSON.h>

#include <esp_log.h>
#include <mqtt_client.h>

#include <message.h>
#include <mqtt.h>
#include <nvs.h>
#include <led.h>

extern xSemaphoreHandle conexaoMQTTSemaphore;
esp_mqtt_client_handle_t client;

char *allocated_room;

void handle_mqtt_register()
{
    const char *saved_room = nvs_read_value("allocated_room");

    uint8_t baseMac[6] = {0};
    esp_efuse_mac_get_default(baseMac);

    char *mac_address = calloc(18, sizeof(char));
    sprintf(mac_address, "%02X:%02X:%02X:%02X:%02X:%02X",
            baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);

    char *topic = calloc(100, sizeof(char));
    sprintf(topic, "fse2021/%s/dispositivos/%s", student_id, mac_address);

    if (strcmp(saved_room, "") != 0)
    {
        ESP_LOGI(MQTT_TAG, "Skipping register procedure");

        allocated_room = saved_room;
        xSemaphoreGive(conexaoMQTTSemaphore);

        esp_mqtt_client_subscribe(client, topic, 0);

        return;
    }

    message request = {"register", mac_address};
    const char *json = message_to_json(request);

    mqtt_send_message(topic, json, 0);

    esp_mqtt_client_subscribe(client, topic, 0);

    ESP_LOGI(MQTT_TAG, "Send register request to topic %s", topic);
}

void handle_receive_data(const char *data)
{
    message request = json_to_message(data);

    if (strcmp(request.command, "register") == 0)
    {
        nvs_write_value("allocated_room", request.data);

        allocated_room = calloc(strlen(request.data), sizeof(char));
        strcpy(allocated_room, request.data);

        xSemaphoreGive(conexaoMQTTSemaphore);
    }
    else if (strcmp(request.command, "led") == 0)
    {
        toggle_led(atof(request.data));
    }
    else if (strcmp(request.command, "reset") == 0)
    {
        clean_nvs_partition();
        esp_restart();
    }
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED");

        handle_mqtt_register();
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");

        ESP_LOGI(MQTT_TAG, "Topic: %.*s\r", event->topic_len, event->topic);
        ESP_LOGI(MQTT_TAG, "Data: %.*s\r", event->data_len, event->data);

        handle_receive_data(event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(MQTT_TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_start()
{
    esp_mqtt_client_config_t mqtt_config = {
        .uri = "mqtt://test.mosquitto.org",
    };
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void mqtt_send_message(char *topic, const char *message, int adjust_topic)
{
    char *full_topic;

    if (adjust_topic == 0)
        full_topic = topic;
    else if (adjust_topic == 1)
    {
        full_topic = calloc(100, sizeof(char));
        sprintf(full_topic, "fse2021/%s/%s/%s", student_id, allocated_room, topic);
    }
    else
    {
        ESP_LOGE(MQTT_TAG, "Invalid option");
        return;
    }

    int message_id = esp_mqtt_client_publish(client, full_topic, message, 0, 1, 0);

    ESP_LOGI(MQTT_TAG, "Sent message, ID: %d", message_id);
}
