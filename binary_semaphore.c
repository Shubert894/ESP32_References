#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/semphr.h"

static const char *TAG = "Message";

SemaphoreHandle_t bSem;
int mesCount;

void sender(void *parameter)
{
    while (1)
    {
        mesCount++;
        ESP_LOGI(TAG, "Sent");
        xSemaphoreGive(bSem);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void receiver(void *parameter)
{
    while (1)
    {
        xSemaphoreTake(bSem, portMAX_DELAY);
        ESP_LOGI(TAG, "Received %d", mesCount);
    }
}

void app_main()
{
    bSem = xSemaphoreCreateBinary();
    xTaskCreate(sender, "Sender", 2048, NULL, 2, NULL);
    xTaskCreate(receiver, "Sender", 2048, NULL, 2, NULL);
}