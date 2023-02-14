#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "cJSON.h"

#include "fire_detector.h"
#include "mqtt.h"
#include "flash_memory_nvs.h" 
#include "rotaty.h"

int analogic_output;
int digital_output;

void init_fire_detector() {
  config_fire_detector();
  xTaskCreate(&handle_fire_detector, FIRE_DETECTOR_TAG, 2048, NULL, 1, NULL);
  
  int32_t fire_state = le_valor_nvs(NVS_FIRE_STATE_KEY);
  int32_t analogic_state = le_valor_nvs(NVS_ANALOGIC_KEY);

  mqtt_send_message_to_dashboard_about_flame_detector(fire_state, analogic_state);
  
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

  char mensagem[50];
  while (true) {
    analogic_output = adc1_get_raw(ANALOGIC_CHANEL);
    digital_output = gpio_get_level(GPIO_DIGITAL_OUTPUT);

    sprintf(mensagem, "{\"potencia_vermelho\": %d}", rotary_r);
    mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

    sprintf(mensagem, "{\"potencia_verde\": %d}", rotary_g);
    mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

    sprintf(mensagem, "{\"potencia_azul\": %d}", rotary_b);
    mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

    ESP_LOGI(FIRE_DETECTOR_TAG, "Saida analogica: %d - Saida digital: %d", analogic_output, digital_output);

    if (digital_output != 0) {
      ESP_LOGI(FIRE_DETECTOR_TAG, "Fogo detectado!!!");
      mqtt_send_message_to_dashboard_about_flame_detector(digital_output, analogic_output);

      vTaskDelay(2000 / portTICK_PERIOD_MS);
    } else {
      mqtt_send_message_to_dashboard_about_flame_detector(digital_output, analogic_output);
    }

    grava_valor_nvs(NVS_ANALOGIC_KEY, analogic_output);
    grava_valor_nvs(NVS_FIRE_STATE_KEY, digital_output);
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void mqtt_send_message_to_dashboard_about_flame_detector(int is_activated, int analogic_op) {
  cJSON* attributes_response_body = cJSON_CreateObject();
  cJSON* telemetry_response_body = cJSON_CreateObject();
  
  if (attributes_response_body == NULL) {
    ESP_LOGE(FIRE_DETECTOR_TAG, "Nao foi possivel criar o attributes_response_body do detector de fogo!");
  }

  if (telemetry_response_body == NULL) {
    ESP_LOGE(FIRE_DETECTOR_TAG, "Nao foi possivel criar o telemetry_response_body do detector de fogo!");
  }

  cJSON_AddItemToObject(telemetry_response_body, "fire_detector_state_telemetry", cJSON_CreateNumber(analogic_op));
  mqtt_envia_mensagem("v1/devices/me/telemetry", cJSON_Print(telemetry_response_body));

  cJSON_AddItemToObject(attributes_response_body, "fire_detector_state", cJSON_CreateNumber(is_activated));
  mqtt_envia_mensagem("v1/devices/me/attributes", cJSON_Print(attributes_response_body));
  
  vTaskDelay(3000 / portTICK_PERIOD_MS);
}