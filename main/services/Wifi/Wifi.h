#ifndef Wifi_h
#define Wifi_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// typedef struct {
//     bool provisioned;
// } Wifi_d;
// Wifi_d _Wifi;
// Wifi_d *WifiD = &_Wifi;

// Funzioni di setup, loop ed interrupt
void Wifi_setup(void);
void Wifi_loop(void);


#ifdef __cplusplus
}
#endif

#endif