#ifndef battery_h
#define battery_h

#ifdef __cplusplus
extern "C" {
#endif

// Funzioni di setup e di scrittura delle variabili HMI dall'interno del software ESP32
void battery_setup(void);
void battery_loop(float *value, float *min, float *max);

#ifdef __cplusplus
}
#endif

#endif