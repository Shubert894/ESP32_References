#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/queue.h"

#define BUTT_LEFT 11
#define BUTT_RIGHT 10

#define BLINK_GPIO 8

static const char *TAG = "Message";

static const int qLen = 10;

QueueHandle_t qPeriod;

int led_state;

static void configure_led(void)
{
    ESP_LOGI(TAG, "Configuring LED");
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

static void configure_buttons(void)
{
    ESP_LOGI(TAG, "Configuring Buttons");
    gpio_reset_pin(BUTT_LEFT);
    gpio_set_direction(BUTT_LEFT, GPIO_MODE_INPUT);
    gpio_reset_pin(BUTT_RIGHT);
    gpio_set_direction(BUTT_RIGHT, GPIO_MODE_INPUT);
}

void blink_led(void *parameter)
{
    int period = 1000;
    while (1)
    {
        if (xQueueReceive(qPeriod, &period, 0) == pdTRUE)
            ESP_LOGI(TAG, "Period is %d", period);
        // else
        //     ESP_LOGI(TAG, "Queue is Full");

        led_state = !led_state;
        gpio_set_level(BLINK_GPIO, led_state);
        vTaskDelay(period / portTICK_PERIOD_MS);
    }
}

void button_press(void *parameter)
{
    int temp_period = 1000;
    const int increment = 100;

    while (1)
    {
        int leftPressed = gpio_get_level(BUTT_LEFT);
        int rightPressed = gpio_get_level(BUTT_RIGHT);
        if (leftPressed == 0)
            temp_period -= increment;
        // ESP_LOGI(TAG, "Left Button Pressed");
        if (rightPressed == 0)
            temp_period += increment;
        // ESP_LOGI(TAG, "Right Button Pressed");

        if (temp_period < 100)
            temp_period = 100;
        else if (temp_period > 2000)
            temp_period = 2000;

        if (leftPressed == 0 || rightPressed == 0)
        {
            if (xQueueSend(qPeriod, &temp_period, 10) == pdTRUE)
                ESP_LOGI(TAG, "Sent");
            else
                ESP_LOGI(TAG, "Full");
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{

    configure_led();
    configure_buttons();

    qPeriod = xQueueCreate(qLen, sizeof(int));

    xTaskCreate(blink_led, "LED Blink", 2048, NULL, 1, NULL);
    xTaskCreate(button_press, "Buttons", 2048, NULL, 2, NULL);
}
