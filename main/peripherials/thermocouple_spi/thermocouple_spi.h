#ifndef THERMOCOUPLE_SPI_H
#define THERMOCOUPLE_SPI_H

#include <stdint.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <math.h>

#define TAG "PT100"
#define PIN_NUM_MISO 21
#define PIN_NUM_MOSI 19
#define PIN_NUM_CLK 33
#define PIN_NUM_CS 5

/**
 * @brief Initialize the PT100 SPI interface.
 */
void pt100_init();

/**
 * @brief Read the temperature from the PT100 sensor.
 *
 * @return Temperature in degrees Celsius.
 */
float pt100_read_temperature();

#endif // THERMOCOUPLE_SPI_H
