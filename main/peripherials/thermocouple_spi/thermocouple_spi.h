#ifndef THERMOCOUPLE_SPI_H
#define THERMOCOUPLE_SPI_H

#include <stdint.h>

/**
 * @brief Inizializza la termocoppia K (MAX6675) via SPI.
 */
void thermocouple_init();

/**
 * @brief Legge la temperatura dalla termocoppia K (MAX6675).
 *
 * @return Temperatura in gradi Celsius.
 */
float thermocouple_read_temperature();

/**
 * @brief Restituisce la temperatura dalla termocoppia K (MAX6675) dalla cache.
 *
 * @return Temperatura in gradi Celsius.
 */
float thermocouple_get_temperature_cached();

#endif // THERMOCOUPLE_SPI_H
