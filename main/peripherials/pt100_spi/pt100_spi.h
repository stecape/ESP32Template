#ifndef PT100_SPI_H
#define PT100_SPI_H

#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX31865_CONFIG_REG 0x00
#define MAX31865_RTD_REG 0x01
#define MAX31865_HIGH_FAULT_REG 0x03
#define MAX31865_LOW_FAULT_REG 0x05
#define MAX31865_FAULT_STATUS_REG 0x07

#define MAX31865_REG_WRITE_OFFSET 0x80

#define MAX31865_CONFIG_VBIAS_BIT 7
#define MAX31865_CONFIG_CONVERSIONMODE_BIT 6
#define MAX31865_CONFIG_1SHOT_BIT 5
#define MAX31865_CONFIG_NWIRES_BIT 4
#define MAX31865_CONFIG_FAULTDETECTION_BIT 2
#define MAX31865_CONFIG_FAULTSTATUS_BIT 1
#define MAX31865_CONFIG_MAINSFILTER_BIT 0

typedef enum {
    MAX31865_NWIRES_THREE = 1,
    MAX31865_NWIRES_TWO = 0,
    MAX31865_NWIRES_FOUR = 0
} Max31865NWires;

typedef enum {
    MAX31865_FAULT_NO_ACTION = 0b00,
    MAX31865_FAULT_AUTO_DELAY = 0b01,
    MAX31865_FAULT_MANUAL_DELAY_CYCLE1 = 0b10,
    MAX31865_FAULT_MANUAL_DELAY_CYCLE2 = 0b11
} Max31865FaultDetection;

typedef enum {
    MAX31865_FILTER_50HZ = 1,
    MAX31865_FILTER_60HZ = 0
} Max31865Filter;

typedef enum {
    MAX31865_NO_ERROR = 0,
    MAX31865_VOLTAGE = 2,
    MAX31865_RTDIN_LOW,
    MAX31865_REF_LOW,
    MAX31865_REF_HIGH,
    MAX31865_RTD_LOW,
    MAX31865_RTD_HIGH
} Max31865Error;

typedef struct {
    bool vbias;
    bool autoConversion;
    Max31865NWires nWires;
    Max31865FaultDetection faultDetection;
    Max31865Filter filter;
} max31865_config_t;

typedef struct {
    float ref;
    float nominal;
} max31865_rtd_config_t;

typedef struct {
    int miso;
    int mosi;
    int sck;
    int cs;
    int drdy;
    spi_host_device_t hostDevice;
    max31865_config_t chipConfig;
    spi_device_handle_t deviceHandle;
    SemaphoreHandle_t drdySemaphore;
} Max31865;

float max31865_rtd_to_temperature(uint16_t rtd, max31865_rtd_config_t rtdConfig);
uint16_t max31865_temperature_to_rtd(float temperature, max31865_rtd_config_t rtdConfig);
const char *max31865_error_to_string(Max31865Error error);

Max31865 *max31865_create(int miso, int mosi, int sck, int cs, int drdy, spi_host_device_t host);
void max31865_destroy(Max31865 *dev);

esp_err_t max31865_begin(Max31865 *dev, max31865_config_t config);
esp_err_t max31865_set_config(Max31865 *dev, max31865_config_t config);
esp_err_t max31865_get_config(Max31865 *dev, max31865_config_t *config);
esp_err_t max31865_set_rtd_thresholds(Max31865 *dev, uint16_t min, uint16_t max);
esp_err_t max31865_clear_fault(Max31865 *dev);
esp_err_t max31865_read_fault_status(Max31865 *dev, Max31865Error *fault);
esp_err_t max31865_get_rtd(Max31865 *dev, uint16_t *rtd, Max31865Error *fault);

esp_err_t setup_pt100_3wires(Max31865 **pdev, int miso, int mosi, int sck, int cs, int drdy, spi_host_device_t host);
float get_pt100_temperature_cached(void);

#endif  // PT100_SPI_H