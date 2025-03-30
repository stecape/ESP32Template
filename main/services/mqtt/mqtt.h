/*
  I topic sono "/command/CONFIG_MQTT_DEVICE_ID" per ciò che arriva all'ESP32, "/feedback/CONFIG_MQTT_DEVICE_ID" per ciò che viene spedito dall'ESP32.
*/

#ifndef mqtt_h
#define mqtt_h

#include "esp_log.h"
#include "mqtt_client.h"

// Funzioni di setup e di scrittura delle variabili HMI dall'interno del software ESP32
void mqtt_setup(void);
void mqtt_updHMI(bool force);

#endif