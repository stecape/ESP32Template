#include "pt100_spi.h"
#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/task.h>
#include <math.h>
#include <string.h>
#include <limits.h>

static const char *TAG = "Max31865";

const char *max31865_error_to_string(Max31865Error error) {
    switch (error) {
        case MAX31865_NO_ERROR:
            return "No error";
        case MAX31865_VOLTAGE:
            return "Over/under voltage fault";
        case MAX31865_RTDIN_LOW:
            return "RTDIN- < 0.85*VBIAS (FORCE- open)";
        case MAX31865_REF_LOW:
            return "REFIN- < 0.85*VBIAS (FORCE- open)";
        case MAX31865_REF_HIGH:
            return "REFIN- > 0.85*VBIAS";
        case MAX31865_RTD_LOW:
            return "RTD below low threshold";
        case MAX31865_RTD_HIGH:
            return "RTD above high threshold";
    }
    return "";
}

static void IRAM_ATTR max31865_drdy_interrupt_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR((SemaphoreHandle_t)arg, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
    // NON mettere log qui! (ISR)
}

Max31865 *max31865_create(int miso, int mosi, int sck, int cs, int drdy, spi_host_device_t host) {
    Max31865 *dev = calloc(1, sizeof(Max31865));
    if (!dev) return NULL;
    dev->miso = miso;
    dev->mosi = mosi;
    dev->sck = sck;
    dev->cs = cs;
    dev->drdy = drdy;
    dev->hostDevice = host;
    dev->deviceHandle = NULL;
    dev->drdySemaphore = NULL;
    return dev;
}

void max31865_destroy(Max31865 *dev) {
    if (!dev) return;
    if (dev->deviceHandle) spi_bus_remove_device(dev->deviceHandle);
    esp_err_t err = spi_bus_free(dev->hostDevice);
    if (err == ESP_OK) {
        gpio_uninstall_isr_service();
    } else if (err == ESP_ERR_INVALID_STATE) {
        ESP_LOGD(TAG, "Devices still attached; not freeing the bus");
    } else {
        ESP_LOGE(TAG, "Error freeing bus: %s", esp_err_to_name(err));
    }
    free(dev);
}

esp_err_t max31865_begin(Max31865 *dev, max31865_config_t config) {
    gpio_config_t gpioConfig = {0};
    gpioConfig.intr_type = GPIO_INTR_DISABLE;
    gpioConfig.mode = GPIO_MODE_OUTPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioConfig.pin_bit_mask = 1ULL << dev->cs;
    gpio_config(&gpioConfig);

    if (dev->drdy > -1) {
        gpio_config_t gpioConfig = {0};
        gpioConfig.intr_type = GPIO_INTR_NEGEDGE;
        gpioConfig.mode = GPIO_MODE_INPUT;
        gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
        gpioConfig.pin_bit_mask = 1ULL << dev->drdy;
        gpio_config(&gpioConfig);

        dev->drdySemaphore = xSemaphoreCreateBinary();
        if (gpio_get_level((gpio_num_t)dev->drdy) == 0) {
            xSemaphoreGive(dev->drdySemaphore);
        }
        // ISR service deve essere installato dal main!
        gpio_isr_handler_add((gpio_num_t)dev->drdy, max31865_drdy_interrupt_handler, dev->drdySemaphore);
    }

    spi_bus_config_t busConfig = {0};
    busConfig.miso_io_num = dev->miso;
    busConfig.mosi_io_num = dev->mosi;
    busConfig.sclk_io_num = dev->sck;
    busConfig.quadhd_io_num = -1;
    busConfig.quadwp_io_num = -1;
    esp_err_t err = spi_bus_initialize(dev->hostDevice, &busConfig, 0);
    if (err == ESP_ERR_INVALID_STATE) {
        ESP_LOGD(TAG, "SPI bus already initialized");
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error initialising SPI bus: %s", esp_err_to_name(err));
        return err;
    }

    spi_device_interface_config_t deviceConfig = {0};
    deviceConfig.spics_io_num = -1;
    deviceConfig.clock_speed_hz = 3000000;
    deviceConfig.mode = 1;
    deviceConfig.address_bits = CHAR_BIT;
    deviceConfig.command_bits = 0;
    deviceConfig.flags = SPI_DEVICE_HALFDUPLEX;
    deviceConfig.queue_size = 1;
    err = spi_bus_add_device(dev->hostDevice, &deviceConfig, &dev->deviceHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error adding SPI device: %s", esp_err_to_name(err));
        return err;
    }
    return max31865_set_config(dev, config);
}

static esp_err_t max31865_write_spi(Max31865 *dev, uint8_t addr, uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "[DEBUG] max31865_write_spi: addr=0x%02X, size=%d", addr, (int)size);
    assert(size <= 4);
    spi_transaction_t transaction = {0};
    transaction.length = CHAR_BIT * size;
    transaction.rxlength = 0;
    transaction.addr = addr | MAX31865_REG_WRITE_OFFSET;
    transaction.flags = SPI_TRANS_USE_TXDATA;
    memcpy(transaction.tx_data, data, size);
    gpio_set_level((gpio_num_t)dev->cs, 0);
    esp_err_t err = spi_device_polling_transmit(dev->deviceHandle, &transaction);
    gpio_set_level((gpio_num_t)dev->cs, 1);
    ESP_LOGI(TAG, "[DEBUG] max31865_write_spi: spi_device_polling_transmit err=%d", err);
    return err;
}

static esp_err_t max31865_read_spi(Max31865 *dev, uint8_t addr, uint8_t *result, size_t size) {
    ESP_LOGI(TAG, "[DEBUG] max31865_read_spi: addr=0x%02X, size=%d", addr, (int)size);
    assert(size <= 4);
    spi_transaction_t transaction = {0};
    transaction.length = 0;
    transaction.rxlength = CHAR_BIT * size;
    transaction.addr = addr & (MAX31865_REG_WRITE_OFFSET - 1);
    transaction.flags = SPI_TRANS_USE_RXDATA;
    gpio_set_level((gpio_num_t)dev->cs, 0);
    esp_err_t err = spi_device_polling_transmit(dev->deviceHandle, &transaction);
    gpio_set_level((gpio_num_t)dev->cs, 1);
    ESP_LOGI(TAG, "[DEBUG] max31865_read_spi: spi_device_polling_transmit err=%d", err);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error sending SPI transaction: %s", esp_err_to_name(err));
        return err;
    }
    memcpy(result, transaction.rx_data, size);
    return ESP_OK;
}

esp_err_t max31865_set_config(Max31865 *dev, max31865_config_t config) {
    dev->chipConfig = config;
    uint8_t configByte = 0;
    if (config.vbias) configByte |= 1UL << MAX31865_CONFIG_VBIAS_BIT;
    if (config.autoConversion) configByte |= 1UL << MAX31865_CONFIG_CONVERSIONMODE_BIT;
    if (config.nWires == MAX31865_NWIRES_THREE) configByte |= 1UL << MAX31865_CONFIG_NWIRES_BIT;
    if (config.faultDetection != MAX31865_FAULT_NO_ACTION) configByte |= ((uint8_t)config.faultDetection) << MAX31865_CONFIG_FAULTDETECTION_BIT;
    if (config.filter != MAX31865_FILTER_60HZ) configByte |= 1UL << MAX31865_CONFIG_MAINSFILTER_BIT;
    return max31865_write_spi(dev, MAX31865_CONFIG_REG, &configByte, 1);
}

esp_err_t max31865_get_config(Max31865 *dev, max31865_config_t *config) {
    uint8_t configByte = 0;
    esp_err_t err = max31865_read_spi(dev, MAX31865_CONFIG_REG, &configByte, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading config: %s", esp_err_to_name(err));
        return err;
    }
    config->vbias = ((configByte >> MAX31865_CONFIG_VBIAS_BIT) & 1U) != 0;
    config->autoConversion = ((configByte >> MAX31865_CONFIG_CONVERSIONMODE_BIT) & 1U) != 0;
    config->nWires = (Max31865NWires)((configByte >> MAX31865_CONFIG_NWIRES_BIT) & 1U);
    config->faultDetection = (Max31865FaultDetection)((configByte >> MAX31865_CONFIG_FAULTDETECTION_BIT) & 1U);
    config->filter = (Max31865Filter)((configByte >> MAX31865_CONFIG_MAINSFILTER_BIT) & 1U);
    return ESP_OK;
}

esp_err_t max31865_set_rtd_thresholds(Max31865 *dev, uint16_t min, uint16_t max) {
    assert((min < (1 << 15)) && (max < (1 << 15)));
    uint8_t thresholds[4];
    thresholds[0] = (uint8_t)((max << 1) >> CHAR_BIT);
    thresholds[1] = (uint8_t)(max << 1);
    thresholds[2] = (uint8_t)((min << 1) >> CHAR_BIT);
    thresholds[3] = (uint8_t)(min << 1);
    return max31865_write_spi(dev, MAX31865_HIGH_FAULT_REG, thresholds, sizeof(thresholds));
}

esp_err_t max31865_clear_fault(Max31865 *dev) {
    uint8_t configByte = 0;
    esp_err_t err = max31865_read_spi(dev, MAX31865_CONFIG_REG, &configByte, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading config: %s", esp_err_to_name(err));
        return err;
    }
    configByte |= 1U << MAX31865_CONFIG_FAULTSTATUS_BIT;
    return max31865_write_spi(dev, MAX31865_CONFIG_REG, &configByte, 1);
}

esp_err_t max31865_read_fault_status(Max31865 *dev, Max31865Error *fault) {
    *fault = MAX31865_NO_ERROR;
    uint8_t faultByte = 0;
    esp_err_t err = max31865_read_spi(dev, MAX31865_FAULT_STATUS_REG, &faultByte, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading fault status: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "MAX31865 fault status byte: 0x%02X", faultByte);
    if (faultByte != 0) {
        *fault = (Max31865Error)(CHAR_BIT * sizeof(unsigned int) - 1 - __builtin_clz(faultByte));
        ESP_LOGW(TAG, "MAX31865 FAULT: %s", max31865_error_to_string(*fault));
    }
    return max31865_clear_fault(dev);
}

esp_err_t max31865_get_rtd(Max31865 *dev, uint16_t *rtd, Max31865Error *fault) {
    ESP_LOGI(TAG, "[DEBUG] Inizio max31865_get_rtd");
    max31865_config_t oldConfig = dev->chipConfig;
    bool restoreConfig = false;
    if (!dev->chipConfig.vbias) {
        restoreConfig = true;
        dev->chipConfig.vbias = true;
        esp_err_t err = max31865_set_config(dev, dev->chipConfig);
        ESP_LOGI(TAG, "[DEBUG] set_config (vbias) err=%d", err);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error setting config: %s", esp_err_to_name(err));
            return err;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!dev->chipConfig.autoConversion) {
        restoreConfig = true;
        uint8_t configByte = 0;
        esp_err_t err = max31865_read_spi(dev, MAX31865_CONFIG_REG, &configByte, 1);
        ESP_LOGI(TAG, "[DEBUG] read_spi (config) err=%d", err);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error reading config: %s", esp_err_to_name(err));
            return err;
        }
        configByte |= 1U << MAX31865_CONFIG_1SHOT_BIT;
        err = max31865_write_spi(dev, MAX31865_CONFIG_REG, &configByte, 1);
        ESP_LOGI(TAG, "[DEBUG] write_spi (1shot) err=%d", err);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error writing config: %s", esp_err_to_name(err));
            return err;
        }
        vTaskDelay(pdMS_TO_TICKS(65));
    } else if (dev->drdy > -1) {
        ESP_LOGI(TAG, "[DEBUG] Prima di xSemaphoreTake");
        if (xSemaphoreTake(dev->drdySemaphore, pdMS_TO_TICKS(1000)) != pdTRUE) {
            ESP_LOGE(TAG, "[DEBUG] Timeout su xSemaphoreTake!");
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "[DEBUG] Dopo xSemaphoreTake");
    }

    uint8_t rtdBytes[2];
    ESP_LOGI(TAG, "[DEBUG] Prima di max31865_read_spi (RTD)");
    esp_err_t err = max31865_read_spi(dev, MAX31865_RTD_REG, rtdBytes, 2);
    ESP_LOGI(TAG, "[DEBUG] Dopo max31865_read_spi (RTD) err=%d", err);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading RTD: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "MAX31865 RTD raw bytes: 0x%02X 0x%02X", rtdBytes[0], rtdBytes[1]);

    if ((rtdBytes[1] & 1U) != 0) {
        *rtd = 0;
        Max31865Error tmp = MAX31865_NO_ERROR;
        if (fault == NULL) fault = &tmp;
        max31865_read_fault_status(dev, fault);
        ESP_LOGW(TAG, "Sensor fault detected: %s", max31865_error_to_string(*fault));
        return ESP_ERR_INVALID_RESPONSE;
    }

    *rtd = rtdBytes[0] << CHAR_BIT;
    *rtd |= rtdBytes[1];
    *rtd >>= 1U;
    ESP_LOGI(TAG, "MAX31865 RTD value: %u", *rtd);

    // ADAFRUIT: Disabilita VBias dopo lettura one-shot (conversione non continua)
    if (!oldConfig.autoConversion) {
        max31865_config_t cfg = oldConfig;
        cfg.vbias = false;
        esp_err_t vbias_err = max31865_set_config(dev, cfg);
        ESP_LOGI(TAG, "[ADAFRUIT] VBias disabilitato dopo lettura one-shot, err=%d", vbias_err);
    }

    return restoreConfig ? max31865_set_config(dev, oldConfig) : ESP_OK;
}

float get_pt100_temperature(Max31865 *dev, max31865_rtd_config_t rtd_cfg) {
    ESP_LOGI(TAG, "[FUNC] get_pt100_temperature called");
    uint16_t rtd;
    Max31865Error fault;
    ESP_LOGI(TAG, "[DEBUG] Prima di max31865_get_rtd");
    esp_err_t ret = max31865_get_rtd(dev, &rtd, &fault);
    ESP_LOGI(TAG, "[DEBUG] Dopo max31865_get_rtd: ret=%d", ret);
    // Lettura e log del registro di fault
    Max31865Error fault_reg = MAX31865_NO_ERROR;
    esp_err_t fault_err = max31865_read_fault_status(dev, &fault_reg);
    ESP_LOGI(TAG, "[DEBUG] Registro di fault: err=%d, code=%d, descrizione=%s", fault_err, fault_reg, max31865_error_to_string(fault_reg));
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "[DEBUG] max31865_get_rtd OK, rtd=%u", rtd);
        // Sostituito max31865_rtd_to_temperature con adafruit_rtd_to_temperature
        return adafruit_rtd_to_temperature(rtd, rtd_cfg.ref, rtd_cfg.nominal);
    }
    ESP_LOGW(TAG, "[FUNC] max31865_get_rtd failed, returning -273.0");
    return -273.0f;
}

// Task per aggiornare la temperatura PT100 ogni 100ms
static volatile float pt100_temperature = -271.0f;

void pt100_temperature_task(void *arg) {
    ESP_LOGI(TAG, "pt100_temperature_task: avviato");
    Max31865 *dev = (Max31865 *)arg;
    max31865_rtd_config_t rtd_cfg = { .ref = 430.0f, .nominal = 100.0f };
    while (1) {
        float temp = get_pt100_temperature(dev, rtd_cfg);
        pt100_temperature = temp;
        if (temp == -273.0f) {
            ESP_LOGE(TAG, "Lettura PT100 fallita o sensore scollegato!");
        } else {
            ESP_LOGI(TAG, "Temperatura PT100 aggiornata: %.2f°C", temp);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Modifica la funzione di setup per creare il task
esp_err_t setup_pt100_3wires(Max31865 **pdev, int miso, int mosi, int sck, int cs, int drdy, spi_host_device_t host) {
    max31865_config_t config = {
        .vbias = true,
        .autoConversion = true, // conversione continua
        .nWires = MAX31865_NWIRES_THREE,
        .faultDetection = MAX31865_FAULT_NO_ACTION,
        .filter = MAX31865_FILTER_50HZ // o 60Hz se preferisci
    };
    *pdev = max31865_create(miso, mosi, sck, cs, drdy, host);
    if (!*pdev) return ESP_ERR_NO_MEM;
    esp_err_t err = max31865_begin(*pdev, config);
    ESP_LOGI(TAG, "setup_pt100_3wires: chiamata xTaskCreate...");
    // Disabilitato: non creare più il task di polling periodico
    // if (err == ESP_OK) {
    //     if (xTaskCreate(pt100_temperature_task, "pt100_temp_task", 4096, *pdev, 5, NULL) == pdPASS) {
    //         ESP_LOGI(TAG, "Task pt100_temp_task creato con successo");
    //     } else {
    //         ESP_LOGE(TAG, "Errore creazione task pt100_temp_task");
    //     }
    // }
    return err;
}

float get_pt100_temperature_cached() {
    return pt100_temperature;
}


// Conversione RTD -> Temperatura secondo Adafruit (Callendar–Van Dusen, fedele all'implementazione Adafruit)
// https://github.com/adafruit/Adafruit_MAX31865/blob/master/Adafruit_MAX31865.cpp
float adafruit_rtd_to_temperature(uint16_t rtd, float ref_resistor, float nominal_resistor) {
    // Rt = (rtd / 32768) * ref_resistor
    float Rt = (float)rtd;
    Rt /= 32768.0f;
    Rt *= ref_resistor;

    // Callendar–Van Dusen (T > 0°C)
    const float A = 3.9083e-3f;
    const float B = -5.775e-7f;
    const float C = -4.183e-12f; // Non usato per T > 0

    float Z1 = -A;
    float Z2 = A * A - (4.0f * B);
    float Z3 = 4.0f * B / nominal_resistor;
    float Z4 = 2.0f * B;

    float temp = Z1 + sqrtf(Z2 + Z3 * Rt);
    temp /= Z4;

    if (temp >= 0.0f) {
        return temp;
    }

    // Adafruit: se T < 0, usa la formula polinomiale (non supportata, restituisce NAN)
    // (opzionale: puoi restituire NAN o -273.15 per coerenza)
    return -273.15;
}

// Lettura RTD secondo Adafruit (bitmask, gestione fault)
// Restituisce true se fault, false se ok
bool adafruit_check_fault(uint8_t fault_byte) {
    return fault_byte != 0;
}