#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "cJSON.h"

#include "main.h"
#include "wifi.h"
#include "mqtt.h"
#include "dht11.h"
#include "fire_detector.h"
#include "low_power.h"

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

void conectadoWifi(void * params) {
  while (true) {
    if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)) {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void app_main(void) {
  // Inicializa o NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  
  conexaoWifiSemaphore = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore = xSemaphoreCreateBinary();
  wifi_start();

  xTaskCreate(&conectadoWifi,  "Iniciar conexão ao MQTT via WI-FI", 4096, NULL, 1, NULL);
  
  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
    if (LOW_POWER_MODE == 0) {
      mqtt_send_low_power_mode(false);

      init_fire_detector();
      dht11_init();
    } else if(LOW_POWER_MODE == LOW_POWER) {
      mqtt_send_low_power_mode(true);
      handle_low_power_mode();     
    }
  }
}

void mqtt_send_low_power_mode(int is_low_power_enabled) {
  ESP_LOGW("LOW POWER", "Enviando estado %s do sistema", (is_low_power_enabled) ? "LOWER POWER" : "NORMAL");
  
  cJSON* response = cJSON_CreateObject();
  
  if (response == NULL){
    ESP_LOGE("LOW POWER", "Erro ao criar o json");
  }

  cJSON_AddItemToObject(response, "is_low_power_enabled", cJSON_CreateNumber(is_low_power_enabled));

  mqtt_envia_mensagem("v1/devices/me/attributes", cJSON_Print(response));
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
