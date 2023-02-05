#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

#include "fire_detector.h"

int analogic_output;
int digital_output;

void init_fire_detector() {
  config_fire_detector();
  xTaskCreate(&handle_fire_detector, "Detector de chama", 4096, NULL, 1, NULL);
}

void config_fire_detector() {
  esp_rom_gpio_pad_select_gpio(GPIO_ANALOGIC_OUTPUT);
  gpio_set_direction(GPIO_ANALOGIC_OUTPUT, GPIO_MODE_INPUT);
  adc1_config_width(ADC_WIDTH_MAX);
  adc1_config_channel_atten(ANALOGIC_CHANEL, ADC_ATTEN_DB_6);

  esp_rom_gpio_pad_select_gpio(GPIO_DIGITAL_OUTPUT);
  gpio_set_direction(GPIO_DIGITAL_OUTPUT, GPIO_MODE_INPUT);
}

void handle_fire_detector(void* params) {
  analogic_output = 0;
  digital_output = 0;

  while (true) {
    analogic_output = adc1_get_raw(ANALOGIC_CHANEL);
    digital_output = gpio_get_level(GPIO_DIGITAL_OUTPUT);

    ESP_LOGI("DETECTOR DE FOGO", "Saida analogica: %d - Saida digital: %d", analogic_output, digital_output);

    if (digital_output != 0){
      ESP_LOGI("Modulo detector de fogo", "Fogo detectado!!!");
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}