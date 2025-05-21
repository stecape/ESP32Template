#include "temperature.h"
#include "sclib/hmi_tools/hmi_tools.h"
#include "sclib/control/PID/pid.h"
#include "peripherials/thermocouple_spi/thermocouple_spi.h"
#include "HMI.h"

// Parametri PID di esempio per temperatura
static PID_Params temperature_pid_params = {
    .Kp = 10.0f, // [% output / °C]
    .Gp = 1.0f,  // adimensionale
    .Ti = 120.0f,  // [s] tempo di integrazione
    .Td = 0.0f,  // [s] tempo di derivazione
    .Taw = 0.25f, // [s] tempo anti-windup (più rapido)
    .dt = 0.25f, // 250 ms
    .out_min = 0.0f,
    .out_max = 100.0f,
    .ref_min = 0.0f,
    .ref_max = 100.0f,
    .gradiente = 10.0f, // 1 unità/s
    .output_gradient = 10.0f // rampa output PID
};
static PID_Handle temperaturePID;

void temperature_setup(void) {
    PID_Init(&temperaturePID, &temperature_pid_params);
    thermocouple_init();
}

void temperature_loop(void) {
    // Aggiorna la temperatura letta dalla termocoppia e scrivila su PLC.Temperature
    float temperature = thermocouple_get_temperature_cached();
    sclib_writeSetAct(&PLC.Temperature, temperature);
    // Aggiorna HMI con il valore attuale della batteria (output PID)
    sclib_writeAct(&PLC.BatteryLevel, PLC.BatteryLevel.Act.Value);
    sclib_writeSetAct(&PLC.Pressure, PLC.Pressure.Act.Value);
}

void temperature_interrupt(void) {
    // Esegui la regolazione PID ogni 250ms
    float setpoint = PLC.Temperature.Set.Value;
    float actual   = PLC.Temperature.Act.Value;
    float reference = 0.0f;
    bool stop = (PLC.Light.Status != 2);
    float pid_output = PID_Compute(&temperaturePID, setpoint, actual, reference, stop);
    PLC.BatteryLevel.Act.Value = pid_output;
    PLC.Pressure.Act.Value = temperaturePID.state.integrale;
}
