#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht11.h"
#include "fire_detector.h"

void app_main() {
  init_fire_detector();
}