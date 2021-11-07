#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include <esp_log.h>

#define DUTY_FORCE 8191
#define LED_GPIO 2
#define LED_TAG "LED"

void led_init()
{
    ESP_LOGI(LED_TAG, "Initializing LED GPIO %d", LED_GPIO);

    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .gpio_num = LED_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&channel_config);
}

void toggle_led(double percentage)
{
    ESP_LOGI(LED_TAG, "Toggle LED with %.0lf%%", percentage * 100);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, percentage * DUTY_FORCE);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}