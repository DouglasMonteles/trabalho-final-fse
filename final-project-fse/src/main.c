#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "mqtt.h"
#include "fire_detector.h"
#include "temperature_sensor.h"

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

void handle_fire_detector_server_connection(void * params) {
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
    while(true) {
      init_fire_detector();
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }
}

void handle_temperature_sensor_server_connection(void * params) {
  ESP_LOGI("SENSOR DE TEMPERATURA", "Iniciado...");
    while(true) {
      //init_temperature_sensor();
      vTaskDelay(3000 / portTICK_PERIOD_MS);
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

  xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&handle_fire_detector_server_connection, "Comunicação com Broker - Fire Detector", 4096, NULL, 1, NULL);
  //xTaskCreate(&handle_temperature_sensor_server_connection, "Comunicação com Broker - Fire Detector", 4096, NULL, 1, NULL);
}
