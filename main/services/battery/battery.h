#ifndef battery_h
#define battery_h

#include "esp_err.h"

// Funzioni di setup e di scrittura delle variabili HMI dall'interno del software ESP32
void battery_setup(void);
void battery_loop(Act *act);

// Enum for low-power modes
typedef enum {
    LOW_POWER_DEEP_SLEEP,
    LOW_POWER_LIGHT_SLEEP,
    LOW_POWER_HIBERNATE
} low_power_mode_t;

// Function to detect if the ESP32 is connected to external power
bool is_connected_to_power(void);

// Function to get the configured low-power mode
low_power_mode_t get_low_power_mode(void);

#endif