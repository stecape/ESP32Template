#ifndef PID_H
#define PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

/**
 * Macro per sezione critica (FreeRTOS, portatile tra task e ISR)
 * Esempio d'uso:
 * PID_ENTER_CRITICAL();
 * ...accesso dati condivisi...
 * PID_EXIT_CRITICAL();
 */
#define PID_ENTER_CRITICAL()  portENTER_CRITICAL()
#define PID_EXIT_CRITICAL()   portEXIT_CRITICAL()

// Struttura parametri PID
typedef struct {
    float Kp; // Guadagno principale [% output / °C]
    float Gp; // Guadagno proporzionale (default 1, adimensionale)
    float Ti; // Tempo di integrazione [s]
    float Td; // Tempo di derivazione [s]
    float Taw; // Tempo anti-windup [s]
    float dt; // Sample time [s]
    float out_min; // Limite correzione PID
    float out_max;
    float ref_min; // Limite uscita totale (reference+correzione)
    float ref_max;
    float gradiente; // Variazione massima setpoint al secondo (0 = disabilitato)
    float output_gradient; // Variazione massima uscita al secondo (rampa output, 0 = disabilitato)
} PID_Params;

// Stato interno PID
typedef struct {
    float integrale;
    float errore_prec;
    float out_pid; // correzione PID (pre-saturazione)
    float out_tot; // uscita totale (reference+correzione, post-saturazione)
    float setpoint_ramp; // setpoint rampato (per istanza PID)
    bool ramp_initialized; // flag inizializzazione ramp
    float output_ramp; // uscita rampata (per stop)
    bool output_ramp_initialized;
} PID_State;

// Handle PID
typedef struct {
    PID_Params params;
    PID_State state;
    // Modalità manuale
    bool manual_mode;
    float manual_output;
    int debounce_counter;
    int debounce_threshold;
    bool pending_mode; // richiesta di cambio modalità
} PID_Handle;

#define PID_DEBOUNCE_DEFAULT 4

void PID_Init(PID_Handle *pid, const PID_Params *params);
/**
 * Reset asincrono del PID: azzera lo stato interno (integrale, derivata, rampa, ecc.).
 * Da chiamare sempre in sezione critica se il PID è usato in ISR/task concorrenti.
 * Esempio:
 *   PID_ENTER_CRITICAL();
 *   PID_Reset(&pid);
 *   PID_EXIT_CRITICAL();
 */
void PID_Reset(PID_Handle *pid);
float PID_Compute(PID_Handle *pid, float setpoint, float measure, float reference, bool stop);
void PID_SetManual(PID_Handle *pid, bool manual, float manual_output);
/**
 * Forza il valore rampato del setpoint al target desiderato (bypassando la rampa).
 * Utile per cambio ricetta/reset/override.
 */
void PID_SyncRamp(PID_Handle *pid, float setpoint_target);
/**
 * Stop asincrono del PID: porta l'uscita a zero con rampa, resetta tutti i contributi.
 * Da chiamare in sezione critica se usato in ISR/task concorrenti.
 * Esempio:
 *   PID_ENTER_CRITICAL();
 *   PID_Stop(&pid);
 *   PID_EXIT_CRITICAL();
 */
void PID_Stop(PID_Handle *pid);
/**
 * PWM a treno di semionde per SSR zero crossing.
 * tick_10ms: contatore 0...24 (per periodo 250ms a 50Hz)
 * Restituisce true se la semionda corrente va accesa.
 */
bool PID_SSR_Burst(PID_Handle *pid, uint32_t tick_10ms);

#ifdef __cplusplus
}
#endif

#endif // PID_H