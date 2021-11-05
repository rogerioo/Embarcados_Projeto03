#include "nvs_flash.h"
#include "esp_log.h"

#include "nvs.h"

void start_nvs()
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
}

void write_value(const char *variable_name, const char *value)
{
    nvs_handle mqq_data_handle;
    esp_err_t res_nvs = nvs_open_from_partition("mqtt_data", "mqtt", NVS_READWRITE, &mqq_data_handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Couldn't open NVS handler");
    }

    esp_err_t res = nvs_set_i32(mqq_data_handle, variable_name, value);

    if (res != ESP_OK)
    {
        ESP_LOGE("NVS", "Could't write on NVS: (%s)", esp_err_to_name(res));
    }

    nvs_commit(mqq_data_handle);
    nvs_close(mqq_data_handle);
}

const char *read_value(const char *variable_name)
{
    nvs_handle mqq_data_handle;
    esp_err_t res_nvs = nvs_open_from_partition("mqtt_data", "mqtt", NVS_READWRITE, &mqq_data_handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
        return "";
    }

    size_t required_size;
    esp_err_t res = nvs_get_str(mqq_data_handle, variable_name, NULL, &required_size);

    switch (res)
    {
    case ESP_OK:
        ESP_LOGI("NVS", "Find string with size: %d\n", required_size);
        break;
    case ESP_ERR_NOT_FOUND:
        ESP_LOGE("NVS", "Couldn't find value");
        return "";
    default:
        ESP_LOGE("NVS", "Couldn't access NVS: (%s)", esp_err_to_name(res));
        return "";
        break;
    }

    char *value = malloc(required_size);
    res = nvs_get_str(mqq_data_handle, variable_name, &value, &required_size);

    nvs_close(mqq_data_handle);

    return value;
}