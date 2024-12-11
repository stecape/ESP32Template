#ifndef mqtt_h
#define mqtt_h

#include "esp_log.h"
#include "mqtt_client.h"

typedef struct {
    bool place_holder;
} mqtt_d;
mqtt_d _mqtt;
mqtt_d *mqtt = &_mqtt;

// Funzioni di setup, loop e send
void mqtt_setup(void);
void mqtt_send(void);

#endif