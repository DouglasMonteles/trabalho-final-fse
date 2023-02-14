#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp32/rom/uart.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "esp_sleep.h"

#include "mqtt.h"
#include "low_power.h"

void handle_low_power_mode() {
  config_low_power();

  esp_sleep_enable_gpio_wakeup();
  
  ESP_LOGI(LOW_POWER_TAG, "O sistema está dormindo...");
  uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
  esp_light_sleep_start();
  esp_restart();
}

void config_low_power() {
  esp_rom_gpio_pad_select_gpio(BTN);
  gpio_set_direction(BTN, GPIO_MODE_INPUT);

  gpio_wakeup_enable(BTN, GPIO_INTR_LOW_LEVEL);
}