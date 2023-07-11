#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

const static char *TAG = "Message";

#define ADC1_CH1 ADC_CHANNEL_2 // GPIO2
#define ADC1_CH2 ADC_CHANNEL_3 // GPIO3

#define ADC_ATTEN ADC_ATTEN_DB_11 // 150mV - 2450mV

adc_oneshot_unit_handle_t adc1;

void adc_configure()
{
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1, ADC1_CH1, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1, ADC1_CH2, &config));
}

float to_mv(int x)
{
    const int dMax = 3889;
    const int dMin = 2288;
    const float range = dMax - dMin;
    const float vMax = 1589;
    return (x - dMin) / range * vMax;
}

void adc_sample(void *parameter)
{
    int val1 = 0;
    int val2 = 0;

    while (1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1, ADC1_CH1, &val1));
        ESP_ERROR_CHECK(adc_oneshot_read(adc1, ADC1_CH2, &val2));

        ESP_LOGI(TAG, "ADC1 --- Ch1Raw : [%d], Ch1mv : [%f]mV", val1, to_mv(val1));

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    adc_configure();

    xTaskCreate(adc_sample, "ADC Sampling", 2048, NULL, 2, NULL);
}