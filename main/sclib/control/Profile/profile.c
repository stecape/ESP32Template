#include "profile.h"
#include <stdbool.h>
#include <stddef.h>
#include "esp_timer.h"
#include "HMI.h"
#include "../../hmi_tools/hmi_tools.h"

static float get_time_s() {
    // Restituisce il tempo dal boot in secondi (float)
    return (float)(esp_timer_get_time() / 1000000ULL);
}

// Funzione di utilità: restituisce il valore interpolato dato un tempo elapsed
static float interpolate_profile(const Interpolation *profile, float elapsed, int *out_idx) {
    // Assumiamo che profile->Times e profile->Values siano array paralleli di Set (max 6 punti)
    // Il tempo di ciascun punto è la durata cumulativa
    float t_prev = 0.0f;
    float v_prev = profile->Values._0.Set.Value;
    for (int i = 0; i < 6; ++i) {
        float t = 0.0f;
        float v = 0.0f;
        switch(i) {
            case 0: t = profile->Times._0.Set.Value; v = profile->Values._0.Set.Value; break;
            case 1: t = profile->Times._1.Set.Value; v = profile->Values._1.Set.Value; break;
            case 2: t = profile->Times._2.Set.Value; v = profile->Values._2.Set.Value; break;
            case 3: t = profile->Times._3.Set.Value; v = profile->Values._3.Set.Value; break;
            case 4: t = profile->Times._4.Set.Value; v = profile->Values._4.Set.Value; break;
            case 5: t = profile->Times._5.Set.Value; v = profile->Values._5.Set.Value; break;
        }
        // Forza monotonia crescente (o uguale) dei tempi
        if (t < t_prev) t = t_prev;
        if (elapsed < t) {
            float dt = t - t_prev;
            float frac = (dt > 0) ? (elapsed - t_prev) / dt : 0.0f;
            if (frac < 0) frac = 0;
            if (frac > 1) frac = 1;
            if (out_idx) *out_idx = i-1;
            return v_prev + frac * (v - v_prev);
        }
        t_prev = t;
        v_prev = v;
    }
    if (out_idx) *out_idx = 5;
    return v_prev;
}

ProfileReturn sclib_profile(Interpolation *profile, ProfileState *state, bool start, bool pause, bool stop) {
    ProfileReturn ret = {0};
    if (!state || !profile) {
        ret.error = true;
        return ret;
    }

    // Imposta i limiti minimi per garantire monotonia crescente dei tempi
    profile->Times._0.Limit.Min = 0.0f;
    profile->Times._1.Limit.Min = profile->Times._0.Set.Value;
    profile->Times._2.Limit.Min = profile->Times._1.Set.Value;
    profile->Times._3.Limit.Min = profile->Times._2.Set.Value;
    profile->Times._4.Limit.Min = profile->Times._3.Set.Value;
    profile->Times._5.Limit.Min = profile->Times._4.Set.Value;
    // Imposta i limiti massimi a 24h (86400 secondi)
    profile->Times._0.Limit.Max = 86400.0f;
    profile->Times._1.Limit.Max = 86400.0f;
    profile->Times._2.Limit.Max = 86400.0f;
    profile->Times._3.Limit.Max = 86400.0f;
    profile->Times._4.Limit.Max = 86400.0f;
    profile->Times._5.Limit.Max = 86400.0f;

    sclib_Set(&profile->Times._0, 0, 0.0f, 0);
    sclib_Set(&profile->Times._1, 0, 0.0f, 0);
    sclib_Set(&profile->Times._2, 0, 0.0f, 0);
    sclib_Set(&profile->Times._3, 0, 0.0f, 0);
    sclib_Set(&profile->Times._4, 0, 0.0f, 0);
    sclib_Set(&profile->Times._5, 0, 0.0f, 0);
    sclib_Set(&profile->Values._0, 0, 0.0f, 0);
    sclib_Set(&profile->Values._1, 0, 0.0f, 0);
    sclib_Set(&profile->Values._2, 0, 0.0f, 0);
    sclib_Set(&profile->Values._3, 0, 0.0f, 0);
    sclib_Set(&profile->Values._4, 0, 0.0f, 0);
    sclib_Set(&profile->Values._5, 0, 0.0f, 0);

    if (stop) {
        state->running = false;
        state->elapsed = 0.0f;
        state->init = false;
        ret.done = false;
        ret.error = false;
        ret.busy = false;
        ret.value = 0.0f;
        ret.time = 0.0f;
        return ret;
    }

    if (pause) {
        state->running = false;
        ret.busy = false;
        ret.value = 0.0f;
        ret.time = state->elapsed;
        return ret;
    }

    if (start) {
        state->running = true;
        if (!state->init) {
            state->elapsed = 0.0f;
            state->start_time = get_time_s();
            state->prev_time = state->start_time;
            state->init = true;
        }
    }

    if (state->running) {
        float now = get_time_s();
        float dt = now - state->prev_time;
        if (dt < 0) dt = 0;
        state->elapsed += dt;
        state->prev_time = now;

        // Calcola il tempo totale del profilo (ultimo time)
        float t_end = profile->Times._5.Set.Value;
        for (int i = 4; i >= 0; --i) {
            float t = 0.0f;
            switch(i) {
                case 4: t = profile->Times._4.Set.Value; break;
                case 3: t = profile->Times._3.Set.Value; break;
                case 2: t = profile->Times._2.Set.Value; break;
                case 1: t = profile->Times._1.Set.Value; break;
                case 0: t = profile->Times._0.Set.Value; break;
            }
            if (t_end < t) t_end = t;
        }

        if (state->elapsed >= t_end) {
            ret.done = true;
            ret.busy = false;
            ret.value = interpolate_profile(profile, t_end, NULL);
            ret.time = t_end;
            state->running = false;
        } else {
            int idx = 0;
            ret.value = interpolate_profile(profile, state->elapsed, &idx);
            ret.time = state->elapsed;
            ret.busy = true;
        }
    } else {
        // Se non running, aggiorna prev_time per evitare salti di tempo quando si riparte
        state->prev_time = get_time_s();
    }
    return ret;
}
