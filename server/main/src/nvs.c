#include <nvs_flash.h>
#include <esp_log.h>

#include <nvs.h>

void start_nvs()
{
    ESP_LOGI(NVS_TAG, "Intializing NVS");

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(nvs_flash_init_partition("mqtt_data"));
}

void nvs_write_value(const char *variable_name, const char *value)
{
    ESP_LOGI(NVS_TAG, "Writing variable %s value %s on NVS", variable_name, value);

    nvs_handle mqq_data_handle;

    ESP_ERROR_CHECK(nvs_open_from_partition("mqtt_data", "mqtt", NVS_READWRITE, &mqq_data_handle));

    ESP_ERROR_CHECK(nvs_set_str(mqq_data_handle, variable_name, value));

    nvs_commit(mqq_data_handle);
    nvs_close(mqq_data_handle);
}

const char *nvs_read_value(const char *variable_name)
{
    ESP_LOGI(NVS_TAG, "Reading variable %s from NVS", variable_name);

    nvs_handle mqq_data_handle;
    esp_err_t res_nvs = nvs_open_from_partition("mqtt_data", "mqtt", NVS_READONLY, &mqq_data_handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE(NVS_TAG, "Namespace empty: (%s)", esp_err_to_name(res_nvs));
        return "";
    }

    ESP_ERROR_CHECK(res_nvs);

    size_t *required_size;
    required_size = calloc(1, sizeof(int));

    esp_err_t res = nvs_get_str(mqq_data_handle, variable_name, NULL, required_size);

    switch (res)
    {
    case ESP_OK:
        ESP_LOGI(NVS_TAG, "Find string with size: %zu", *required_size);
        break;
    case ESP_ERR_NOT_FOUND:
        ESP_LOGE(NVS_TAG, "Couldn't find value");
        return "";
    default:
        ESP_LOGE(NVS_TAG, "Couldn't access NVS: (%s)", esp_err_to_name(res));
        return "";
        break;
    }

    char *value = malloc(*required_size);
    res = nvs_get_str(mqq_data_handle, variable_name, value, required_size);

    switch (res)
    {
    case ESP_OK:
        ESP_LOGI(NVS_TAG, "Retrivied string: %s", value);
        break;
    case ESP_ERR_NOT_FOUND:
        ESP_LOGE(NVS_TAG, "Couldn't find value");
        return "";
    default:
        ESP_LOGE(NVS_TAG, "Couldn't access NVS: (%s)", esp_err_to_name(res));
        return "";
        break;
    }

    nvs_close(mqq_data_handle);

    return value;
}

void clean_nvs_partition()
{
    nvs_flash_erase_partition("mqtt_data");
}