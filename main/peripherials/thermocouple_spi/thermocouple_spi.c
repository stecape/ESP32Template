#include "esp_log.h"
#include <math.h> // Include math.h for NAN definition
#include "thermocouple_spi.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define TAG "PT100"
#define PIN_NUM_CS 5
#define PIN_NUM_MISO 21
#define PIN_NUM_MOSI 19
#define PIN_NUM_CLK 33

// Define constants for PT100 sensor
#define RREF 430.0  // Reference resistor value
#define RNOMINAL 100.0  // Nominal resistance of PT100

static spi_device_handle_t spi;

// Function to calculate temperature from resistance
static float calculate_temperature(float resistance) {
    if (resistance < 0) {
        ESP_LOGE(TAG, "Invalid resistance value: %f", resistance);
        return NAN;
    }
    return (resistance - RNOMINAL) / 0.385; // PT100 standard formula
}

void pt100_init() {
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000, // Reduced SPI clock speed to 500 kHz
        .mode = 1, // SPI mode 1
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 1
    };

    ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &buscfg, 1));
    ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &devcfg, &spi));
    ESP_LOGI(TAG, "SPI initialized for MAX31865 with reduced clock speed");

    // Configure MAX31865 for 3-wire PT100
    uint8_t config_data[] = {0x80, 0xD2}; // Write to config register
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = config_data
    };
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));

    // Read back the configuration register to verify
    uint8_t read_config_cmd = 0x00; // Read config register
    uint8_t config_value = 0;
    t.length = 16;
    t.tx_buffer = &read_config_cmd;
    t.rx_buffer = &config_value;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));
    ESP_LOGI(TAG, "MAX31865 configuration register: 0x%02X", config_value);

    // Set low threshold to 0x0000 and high threshold to 0x7FFF
    uint8_t low_threshold[] = {0x02, 0x00, 0x00};
    t.length = 24;
    t.tx_buffer = low_threshold;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));

    uint8_t high_threshold[] = {0x03, 0x7F, 0xFF};
    t.tx_buffer = high_threshold;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));

    ESP_LOGI(TAG, "MAX31865 thresholds configured: Low=0x0000, High=0x7FFF");
}

float pt100_read_temperature() {
    uint8_t tx_data[2] = {0x01, 0x00}; // Read RTD MSB
    uint8_t rx_data[2] = {0};

    // Read status register to check for faults
    uint8_t status_cmd = 0x07; // Read fault status register
    uint8_t status_value = 0;
    spi_transaction_t status_t = {
        .length = 16,
        .tx_buffer = &status_cmd,
        .rx_buffer = &status_value
    };
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &status_t));
    ESP_LOGI(TAG, "MAX31865 status register: 0x%02X", status_value);

    // Add a longer delay to ensure the MAX31865 completes the conversion
    vTaskDelay(pdMS_TO_TICKS(1000));

    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };

    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));

    // Debug SPI communication
    ESP_LOGI(TAG, "Debugging SPI communication");
    ESP_LOGI(TAG, "SPI TX: 0x%02X 0x%02X", tx_data[0], tx_data[1]);
    ESP_LOGI(TAG, "SPI RX: 0x%02X 0x%02X", rx_data[0], rx_data[1]);

    // Check if RX data is consistent
    if (rx_data[0] == 0x00 && rx_data[1] == 0x00) {
        ESP_LOGW(TAG, "Received all zeros from SPI. Possible communication issue.");
    } else if (rx_data[0] == 0xFF && rx_data[1] == 0xFF) {
        ESP_LOGW(TAG, "Received all ones from SPI. Possible misconfiguration or noise.");
    } else {
        ESP_LOGI(TAG, "SPI communication appears normal.");
    }

    int16_t raw_rtd = ((rx_data[0] << 8) | rx_data[1]) >> 1;
    if (raw_rtd & 0x8000) { // Check for fault
        ESP_LOGE(TAG, "MAX31865 fault detected");
        return NAN;
    }

    float resistance = raw_rtd * RREF / 32768.0;
    ESP_LOGI(TAG, "Raw RTD: %d, Resistance: %f", raw_rtd, resistance);

    return calculate_temperature(resistance);
}
