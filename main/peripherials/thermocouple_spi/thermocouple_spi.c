#include "thermocouple_spi.h"
#include "esp_log.h"
#include "max6675.h"

#define TAG "THERMOCOUPLE"
#define THERMOCOUPLE_SCK   14
#define THERMOCOUPLE_CS    27
#define THERMOCOUPLE_MISO  12
#define THERMOCOUPLE_COEFF 0.25f

static MAX6675_structure thermocouple_cfg = {
    .MAX6675_SCK = THERMOCOUPLE_SCK,
    .MAX6675_CS = THERMOCOUPLE_CS,
    .MAX6675_MISO = THERMOCOUPLE_MISO,
    .TEMPERATURE_CALIBRATION_COEFFICIENT = THERMOCOUPLE_COEFF
};

static float thermocouple_temperature = 0.0f;

void thermocouple_temperature_task(void *arg) {
    while (1) {
        thermocouple_temperature = thermocouple_read_temperature();
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void thermocouple_init() {
    MAX6675_init(thermocouple_cfg);
    xTaskCreate(thermocouple_temperature_task, "thermocouple_temp_task", 2048, NULL, 5, NULL);
}

float thermocouple_read_temperature() {
    double temp = readCelsius();
    return (float)temp;
}

float thermocouple_get_temperature_cached() {
    return thermocouple_temperature;
}
