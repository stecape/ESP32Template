#include "pid.h"
#include <string.h>
#include <math.h>

void PID_Init(PID_Handle *pid, const PID_Params *params) {
    if (!pid || !params) return;
    pid->params = *params;
    PID_Reset(pid);
    pid->manual_mode = false;
    pid->manual_output = 0.0f;
    pid->debounce_counter = 0;
    pid->debounce_threshold = PID_DEBOUNCE_DEFAULT;
    pid->pending_mode = false;
}

void PID_Reset(PID_Handle *pid) {
    if (!pid) return;
    memset(&pid->state, 0, sizeof(PID_State));
    pid->manual_mode = false;
    pid->manual_output = 0.0f;
    pid->debounce_counter = 0;
    pid->debounce_threshold = PID_DEBOUNCE_DEFAULT;
    pid->pending_mode = false;
    pid->state.setpoint_ramp = 0.0f;
    pid->state.ramp_initialized = false;
}

void PID_SetManual(PID_Handle *pid, bool manual, float manual_output) {
    if (!pid) return;
    if (manual != pid->manual_mode) {
        pid->pending_mode = manual;
        pid->debounce_counter = 0;
    }
    pid->manual_output = manual_output;
}

void PID_SyncRamp(PID_Handle *pid, float setpoint_target) {
    if (!pid) return;
    pid->state.setpoint_ramp = setpoint_target;
    pid->state.ramp_initialized = true;
}

static float saturate(float val, float min, float max) {
    if (val > max) return max;
    if (val < min) return min;
    return val;
}

float PID_Compute(PID_Handle *pid, float setpoint, float measure, float reference, bool stop) {
    if (!pid) return 0.0f;
    PID_Params *p = &pid->params;
    PID_State *s = &pid->state;

    // --- Gestione STOP con rampa di uscita ---
    if (stop) {
        // Inizializza la rampa di uscita al valore attuale solo la prima volta
        if (!s->output_ramp_initialized) {
            s->output_ramp = s->out_tot;
            s->output_ramp_initialized = true;
        }
        float delta = 0.0f - s->output_ramp;
        float max_step = (p->output_gradient > 0.0f && p->dt > 0.0f) ? (p->output_gradient * p->dt) : fabsf(delta);
        if (fabsf(delta) <= max_step) {
            s->output_ramp = 0.0f;
        } else {
            s->output_ramp += (delta > 0 ? max_step : -max_step);
        }
        // Reset contributi interni ogni ciclo
        s->integrale = 0.0f;
        s->errore_prec = 0.0f;
        s->setpoint_ramp = 0.0f;
        s->ramp_initialized = false;
        s->out_pid = 0.0f;
        s->out_tot = s->output_ramp;
        return s->output_ramp;
    }

    // --- Gestione debounce cambio modalità ---
    if (pid->pending_mode != pid->manual_mode) {
        pid->debounce_counter++;
        if (pid->debounce_counter >= pid->debounce_threshold) {
            pid->manual_mode = pid->pending_mode;
            pid->debounce_counter = 0;
        }
    } else {
        pid->debounce_counter = 0;
    }
    // --- Se in manuale, restituisci manual_output saturato ---
    if (pid->manual_mode) {
        float out = saturate(pid->manual_output, pid->params.ref_min, pid->params.ref_max);
        pid->state.out_tot = out;
        return out;
    }

    // --- Ramping setpoint per istanza PID ---
    if (!s->ramp_initialized) {
        s->setpoint_ramp = setpoint;
        s->ramp_initialized = true;
    }
    if (p->gradiente > 0.0f && p->dt > 0.0f) {
        float delta = setpoint - s->setpoint_ramp;
        float max_step = p->gradiente * p->dt;
        if (fabsf(delta) <= max_step) {
            s->setpoint_ramp = setpoint;
        } else {
            s->setpoint_ramp += (delta > 0 ? max_step : -max_step);
        }
    } else {
        s->setpoint_ramp = setpoint;
    }

    float errore = s->setpoint_ramp - measure;
    float errore_scalato = errore * p->Kp;
    // Proporzionale
    float out_p = p->Gp * errore_scalato;
    // Integrale (incremento corretto: (errore_scalato / Ti) * dt)
    if (p->Ti > 0.0f) {
        s->integrale += (errore_scalato / p->Ti) * p->dt;
    }
    float out_i = s->integrale;
    // Derivata (Td in secondi)
    float out_d = 0.0f;
    if (p->Td > 0.0f && p->dt > 0.0f) {
        out_d = p->Td * (errore_scalato - s->errore_prec) / p->dt;
    }
    float out_pid = out_p + out_i + out_d;
    // Saturazione correzione PID
    float out_pid_sat = saturate(out_pid, p->out_min, p->out_max);
    // Uscita totale: reference (feedforward) + correzione PID
    float out_tot = reference + out_pid_sat;
    float out_tot_sat = saturate(out_tot, p->ref_min, p->ref_max);
    // Anti-windup: se c'è saturazione su out_pid o out_tot, correggi integrale
    float aw_term = 0.0f;
    if (out_pid != out_pid_sat) {
        aw_term = (out_pid_sat - out_pid); // saturazione correzione
    } else if (out_tot != out_tot_sat) {
        aw_term = (out_tot_sat - out_tot); // saturazione uscita totale
    }
    // Aggiorna integrale con anti-windup (incremento corretto: (aw_term / Taw) * dt)
    if (p->Taw > 0.0f) {
        s->integrale += (aw_term / p->Taw) * p->dt;
    }
    // Aggiorna stato
    s->errore_prec = errore_scalato;
    s->out_pid = out_pid_sat;
    s->out_tot = out_tot_sat;
    s->output_ramp_initialized = false; // resetta la rampa di uscita se non in stop

    return out_tot_sat;
}