#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "../../HMI.h"
#include "../mqtt/mqtt.h"
#include "esp_log.h"

#define VBAT_PIN ADC_CHANNEL_7

static const char *TAG = "battery";
static adc_oneshot_unit_handle_t adc1_handle;
static int64_t prevTime = 0;

float readBatteryVoltage() {
  int raw;
  esp_err_t err = adc_oneshot_read(adc1_handle, VBAT_PIN, &raw);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read ADC: %s", esp_err_to_name(err));
    return -1.0; // Return an invalid voltage
  }
  ESP_LOGI(TAG, "ADC reading: %d V", raw); // Log the voltage
  float voltage = raw * 2 * 3.3 / 4096;  // Compensa il partitore di tensione e converte in volt
  return voltage;
}

float calculateBatteryPercentage(float voltage) {
  float minVoltage = 3.0;
  float maxVoltage = 4.2;
  float percentage = (voltage - minVoltage) / (maxVoltage - minVoltage) * 100;
  if (percentage > 100.0) {
    percentage = 100.0;
  } else if (percentage < 0.0) {
    percentage = 0.0;
  }
  return percentage;
}

// Funzione di configurazione dei GPIO della batteria
void battery_setup() {
  adc_oneshot_unit_init_cfg_t init_config = {
    .unit_id = ADC_UNIT_1,
  };
  esp_err_t err = adc_oneshot_new_unit(&init_config, &adc1_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize ADC unit: %s", esp_err_to_name(err));
    return;
  }

  adc_oneshot_chan_cfg_t adc_configuration = {
    .bitwidth = ADC_BITWIDTH_12,
    .atten = ADC_ATTEN_DB_12,
  };
  err = adc_oneshot_config_channel(adc1_handle, VBAT_PIN, &adc_configuration);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to configure ADC channel: %s", esp_err_to_name(err));
  }
}

// Funzione di loop dei GPIO della batteria
void battery_loop(float *value, float *min, float *max){
  int64_t currentTime = time(NULL); // Get time in seconds
  if (currentTime - prevTime < 5) { // Update every 5 seconds
    return;
  }
  prevTime = currentTime;
  float voltage = readBatteryVoltage();
  if (voltage < 0) {
    ESP_LOGE(TAG, "Invalid battery voltage");
    return;
  }
  ESP_LOGI(TAG, "Battery voltage: %.2f V", voltage); // Log the voltage
  float percentage = calculateBatteryPercentage(voltage);
  float max_val = 100.0;
  float min_val = 0.0;
  mqtt_updHMI(value, &percentage);
  mqtt_updHMI(max, &max_val);
  mqtt_updHMI(min, &min_val);
}