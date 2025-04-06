#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include <esp_err.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Inizializza la NVS
esp_err_t nvs_manager_init(void);

// Legge un valore intero dalla NVS
esp_err_t nvs_manager_get_int(int key, int *value);

// Salva un valore intero nella NVS
esp_err_t nvs_manager_set_int(int key, int value);

// Legge un valore booleano dalla NVS
esp_err_t nvs_manager_get_bool(int key, bool *value);

// Salva un valore booleano nella NVS
esp_err_t nvs_manager_set_bool(int key, bool value);

// Legge un valore float dalla NVS
esp_err_t nvs_manager_get_float(int key, float *value);

// Salva un valore float nella NVS
esp_err_t nvs_manager_set_float(int key, float value);

// Legge un valore timestamp dalla NVS
esp_err_t nvs_manager_get_timestamp(int key, time_t *value);

// Salva un valore timestamp nella NVS
esp_err_t nvs_manager_set_timestamp(int key, time_t value);

// Legge un valore string nella NVS
esp_err_t nvs_manager_get_string(int key, char *value, size_t max_len);

// Salva un valore string nella NVS
esp_err_t nvs_manager_set_string(int key, const char *value);

// Cancella tutti i dati nella NVS
esp_err_t nvs_manager_erase_all(void);

#endif // NVS_MANAGER_H