#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "cJSON.h"

#include "led.h"

void handle_led_event(char* event_data) {
  cJSON* json = cJSON_Parse(event_data);
  cJSON* response = cJSON_GetObjectItem(json, "method");

  if (strcmp(LED_STATUS_METHOD, response->valuestring) == 0){
    response = cJSON_GetObjectItem(json, "params");
    config_led();
    turn_led_on_or_off();
  }
}

void config_led() {
  esp_rom_gpio_pad_select_gpio(LED);
  gpio_set_direction(LED, GPIO_MODE_INPUT_OUTPUT);
}

int led_status(int led_pin) {
  return gpio_get_level(led_pin);
}

void turn_led_on_or_off() {
  ESP_LOGI(LED_TAG, "Alterando o estado do led");

  if (led_status(LED) == LOW) {
    gpio_set_level(LED, HIGH);
  } else {
    gpio_set_level(LED, LOW);
  }
}