#ifndef Led_h
#define Led_h

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/gpio.h"
#include "driver/touch_pad.h"
#include "esp_log.h"

// Funzioni di setup, loop ed interrupt
void led_setup(void);
void led_loop(void);
void led_interrupt(void);

#ifdef __cplusplus
}
#endif

#endif