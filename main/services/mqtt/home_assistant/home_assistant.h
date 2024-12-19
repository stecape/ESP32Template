/*
  I topic sono "/command/CONFIG_MQTT_DEVICE_ID" per ciò che arriva all'ESP32, "/feedback/CONFIG_MQTT_DEVICE_ID" per ciò che viene spedito dall'ESP32.
*/

#ifndef home_assistant_h
#define home_assistant_h

#include "esp_log.h"
#include "mqtt_client.h"

// Funzioni di setup e di scrittura delle variabili HMI dall'interno del software ESP32
void home_assistant_setup(void);
void home_assistant_update(char *device_id, int _type, void *ptrToValue);

#endif