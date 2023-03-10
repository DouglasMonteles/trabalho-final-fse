#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"

#include "flash_memory_nvs.h"

int32_t le_valor_nvs(char* key) {
  // Inicia o acesso à partição padrão nvs
  ESP_ERROR_CHECK(nvs_flash_init());  

  // Inicia o acesso à partição personalizada
  // ESP_ERROR_CHECK(nvs_flash_init_partition(NVS_FIRE_STATE_KEY));
  // ESP_ERROR_CHECK(nvs_flash_init_partition(NVS_ANALOGIC_KEY));

  int32_t valor = 0;
  nvs_handle particao_padrao_handle;

  // Abre o acesso à partição nvs
  esp_err_t res_nvs = nvs_open(NVS_STORAGE, NVS_READONLY, &particao_padrao_handle);

  // Abre o acesso à partição DadosNVS
  // esp_err_t res_nvs = nvs_open_from_partition(key, "armazenamento", NVS_READONLY, &particao_padrao_handle);

  if (res_nvs == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGE(NVS_TAG, "Namespace: %s, não encontrado", key);
  } else {
    esp_err_t res = nvs_get_i32(particao_padrao_handle, key, &valor);

    switch (res) {
    case ESP_OK:
      printf("Valor armazenado em %s: %d\n", key, (int)valor);
      break;
    case ESP_ERR_NOT_FOUND:
      ESP_LOGE(NVS_TAG, "Valor não encontrado");
      return -1;
    default:
      ESP_LOGE(NVS_TAG, "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
      return -1;
      break;
    }

    nvs_close(particao_padrao_handle);
  }
  return valor;
}

void grava_valor_nvs(char* key, int32_t valor) {
  ESP_ERROR_CHECK(nvs_flash_init());
  // ESP_ERROR_CHECK(nvs_flash_init_partition(key));

  nvs_handle particao_padrao_handle;

  esp_err_t res_nvs = nvs_open(NVS_STORAGE, NVS_READWRITE, &particao_padrao_handle);
  // esp_err_t res_nvs = nvs_open_from_partition(key, "armazenamento", NVS_READWRITE, &particao_padrao_handle);

  if (res_nvs == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGE(NVS_TAG, "Namespace: %s, não encontrado", key);
  }
  
  esp_err_t res = nvs_set_i32(particao_padrao_handle, key, valor);
  
  if (res != ESP_OK) {
    ESP_LOGE(NVS_TAG, "Não foi possível escrever no NVS (%s)", esp_err_to_name(res));
  }
  
  nvs_commit(particao_padrao_handle);
  nvs_close(particao_padrao_handle);
}
