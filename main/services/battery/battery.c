#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "../../HMI.h"
#include "../../sclib/hmi_tools/hmi_tools.h"
#include "../mqtt/mqtt.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define VBAT_PIN ADC_CHANNEL_7
#define POWER_DETECT_PIN GPIO_NUM_35 // Example GPIO pin for power detection

static const char *TAG = "battery";
static adc_oneshot_unit_handle_t adc1_handle;

// Function to detect if the ESP32 is connected to external power
bool is_connected_to_power(void) {
    gpio_set_direction(POWER_DETECT_PIN, GPIO_MODE_INPUT);
    return gpio_get_level(POWER_DETECT_PIN) == 1; // HIGH means connected to power
}

// Function to get the configured low-power mode
low_power_mode_t get_low_power_mode(void) {
    #if CONFIG_LOW_POWER_MODE_DEEP_SLEEP
        return LOW_POWER_DEEP_SLEEP;
    #elif CONFIG_LOW_POWER_MODE_LIGHT_SLEEP
        return LOW_POWER_LIGHT_SLEEP;
    #elif CONFIG_LOW_POWER_MODE_HIBERNATE
        return LOW_POWER_HIBERNATE;
    #else
        return LOW_POWER_DEEP_SLEEP; // Default to Deep Sleep
    #endif
}

float readBatteryVoltage() {
  int raw;
  esp_err_t err = adc_oneshot_read(adc1_handle, VBAT_PIN, &raw);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read ADC: %s", esp_err_to_name(err));
    return -1.0; // Return an invalid voltage
  }
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
void battery_loop(Act *act){
  float voltage = readBatteryVoltage();
  if (voltage < 0) {
    ESP_LOGE(TAG, "Invalid battery voltage");
    return;
  }
  float percentage = calculateBatteryPercentage(voltage);
  sclib_writeAct(act, percentage);
}