#ifndef __FLASH_MEMORY_NVS__
#define __FLASH_MEMORY_NVS__

#include "nvs.h"

#define NVS_TAG "NVS"
#define NVS_FIRE_STATE_KEY "fireState"
#define NVS_ANALOGIC_KEY "analogic"
#define NVS_TEMPERATURE_KEY "temperature"
#define NVS_HUMIDITY_KEY "humidity"
#define NVS_STORAGE "armazenamento"

int32_t le_valor_nvs(char* key);
void grava_valor_nvs(char* key, int32_t valor);

#endif