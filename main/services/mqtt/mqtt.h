/*
I topic a cui ci si deve registrare sono "command" per ciò che arriva, "feedback" per ciò che viene spedito.
Nella fattispecie:

*/

#ifndef mqtt_h
#define mqtt_h

#include "esp_log.h"
#include "mqtt_client.h"

// Funzioni di setup, loop e send
void mqtt_setup(void);
void mqtt_updHMI(void *ptrToHMIVar, void *ptrToValue);

#endif