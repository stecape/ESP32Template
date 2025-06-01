#ifndef PROFILE_H
#define PROFILE_H

#include "HMI.h"

typedef struct {
	float start_time; // Tempo di inizio del profilo
  float prev_time; // Tempo precedente per il calcolo del delta
  bool running;   // Stato di esecuzione del profilo
  float elapsed;  // Tempo trascorso dall'inizio del profilo
  bool init;     // Flag di inizializzazione del profilo
} ProfileState;

typedef struct {
  bool done;
  bool error;
  bool busy;
  float value;
  float time;
} ProfileReturn;

ProfileReturn sclib_profile(Interpolation *profile, ProfileState *state, bool start, bool pause, bool stop);

#endif // PROFILE_H