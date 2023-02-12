#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "cJSON.h"

#include "fire_detector.h"
#include "mqtt.h"

int analogic_output;
int digital_output;

void init_fire_detector() {
  config_fire_detector();
  xTaskCreate(&handle_fire_detector, "Detector de chama", 2048, NULL, 1, NULL);
  mqtt_send_message_to_dashboard_about_flame_detector(0);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
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

    if (digital_output != 0) {
      ESP_LOGI("Modulo detector de fogo", "Fogo detectado!!!");
      mqtt_send_message_to_dashboard_about_flame_detector(digital_output);

      vTaskDelay(2000 / portTICK_PERIOD_MS);
    } else {
      analogic_output = 0;
      mqtt_send_message_to_dashboard_about_flame_detector(digital_output);
    }
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void mqtt_send_message_to_dashboard_about_flame_detector(int is_activated) {
  cJSON* attributes_response_body = cJSON_CreateObject();
  cJSON* telemetry_response_body = cJSON_CreateObject();
  
  if (attributes_response_body == NULL) {
    ESP_LOGE("Fire detector", "Nao foi possivel criar o attributes_response_body do detector de fogo!");
  }

  if (telemetry_response_body == NULL) {
    ESP_LOGE("Fire detector", "Nao foi possivel criar o telemetry_response_body do detector de fogo!");
  }

  cJSON_AddItemToObject(telemetry_response_body, "fire_detector_state_telemetry", cJSON_CreateNumber(analogic_output));
  mqtt_envia_mensagem("v1/devices/me/telemetry", cJSON_Print(telemetry_response_body));

  cJSON_AddItemToObject(attributes_response_body, "fire_detector_state", cJSON_CreateNumber(is_activated));
  mqtt_envia_mensagem("v1/devices/me/attributes", cJSON_Print(attributes_response_body));
  
  vTaskDelay(3000 / portTICK_PERIOD_MS);
}