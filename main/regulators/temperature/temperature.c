#include "temperature.h"
#include "sclib/hmi_tools/hmi_tools.h"
#include "sclib/control/PID/pid.h"
#include "sclib/control/Profile/profile.h"
#include "peripherials/thermocouple_spi/thermocouple_spi.h"
#include "HMI.h"
#include "driver/gpio.h"
#include "esp_log.h"

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

static float profile_output = 0.0f;

#define SSR_GPIO 5
#define SSR_BURST_PERIOD_S 30
#define SSR_BURST_HYSTERESIS_S 5

void temperature_setup(void) {
    thermocouple_init();
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SSR_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

void temperature_loop(void) {
    // Aggiorna la temperatura letta dalla termocoppia e scrivila su PLC.Temperature
    float temperature = thermocouple_get_temperature_cached();
    sclib_writeAct(&PLC.ActualTemperature, temperature);

    //Gestione logiche ed analogiche
    sclib_logic(&PLC.Heating);
    sclib_logic(&PLC.Mode);
    sclib_Set(&PLC.TemperatureReference, 0, 0.0, 0);
    sclib_Set(&PLC.PowerReference, 0, 0.0, 0);
    sclib_Set(&PLC.PID.kP, 0, 0.0, 0);
    sclib_Set(&PLC.PID.Gp, 0, 0.0, 0);
    sclib_Set(&PLC.PID.Ti, 0, 0.0, 0);
    sclib_Set(&PLC.PID.Td, 0, 0.0, 0);
    sclib_Set(&PLC.PID.Taw, 0, 0.0, 0);
    sclib_Set(&PLC.PID.PidMin, 0, 0.0, 0);
    sclib_Set(&PLC.PID.PidMax, 0, 0.0, 0);
    sclib_Set(&PLC.PID.OutMin, 0, 0.0, 0);
    sclib_Set(&PLC.PID.OutMax, 0, 0.0, 0);
    sclib_Set(&PLC.PID.SetpointGradient, 0, 0.0, 0);
    sclib_Set(&PLC.PID.OutGradient, 0, 0.0, 0);

    //gestione del profilo
    static ProfileState profile_state = {0};
    ProfileReturn profile_ret = sclib_profile(&PLC.Profile, &profile_state, PLC.Heating.Status == 2 && PLC.Mode.Status == 4, false, PLC.Heating.Status == 1);
    profile_output = profile_ret.value;

    // Scrivi il valore del profilo in PLC.Profile.Output
    sclib_writeAct(&PLC.Profile.Output, profile_output);
}

void temperature_interrupt(void) {
    // Test del PID. Cancellare una volta debuggato (se vuoi)
    temperaturePID.params.gradiente = PLC.PID.SetpointGradient.Set.Value;
    temperaturePID.params.output_gradient = PLC.PID.OutGradient.Set.Value;
    temperaturePID.params.Kp = PLC.PID.kP.Set.Value;
    temperaturePID.params.Ti = PLC.PID.Ti.Set.Value;
    temperaturePID.params.Td = PLC.PID.Ti.Set.Value;
    temperaturePID.params.Gp = PLC.PID.Gp.Set.Value;
    temperaturePID.params.Taw = PLC.PID.Taw.Set.Value;
    temperaturePID.params.out_min = PLC.PID.PidMin.Set.Value;
    temperaturePID.params.out_max = PLC.PID.PidMax.Set.Value;
    temperaturePID.params.ref_min = PLC.PID.OutMin.Set.Value;
    temperaturePID.params.ref_max = PLC.PID.OutMax.Set.Value;
    temperaturePID.params.dt = 0.25f; // 250 ms
    // Test del PID. Cancellare una volta debuggato (se vuoi)


    // Esegui la regolazione PID ogni 250ms
    float setpoint = 0.0f;
    if (PLC.Mode.Status == 1) {
        // Se la modalità è manuale, usa il valore dell'actual di temperature, così da congelare il PID
        setpoint = PLC.ActualTemperature.Act.Value;
    } else if (PLC.Mode.Status == 2) {
        // Se la modalità è automatica, usa il setpoint di temperature
        setpoint = PLC.TemperatureReference.Set.Value;
    } else if (PLC.Mode.Status == 4) {
        // Se la modalità è profile, usa il setpoint generato dal profilo
        setpoint = PLC.Profile.Output.Act.Value;
    } else {
        // Modalità sconosciuta, non fare nulla
        return;
    }

    float pid_output = PID_Mngt(
        /* pid */           &temperaturePID,
        /* setpoint */      setpoint,
        /* actual */        PLC.ActualTemperature.Act.Value,
        /* reference */     0.0f,
        /* stop */          PLC.Heating.Status != 2,
        /* manual_mode */   PLC.Mode.Status == 1,
        /* deriv. enabled*/ false,
        /* AW enabled*/     false,
        /* manual_output */ PLC.PowerReference.Set.Value
    );
    
    // Test del PID. Cancellare una volta debuggato (se vuoi)
    PLC.PID.ManualMode = PLC.Mode.Status == 1;
    PLC.PID.ManualRef = PLC.PowerReference.Set.Value;
    PLC.PID.Stop = PLC.Heating.Status != 2;
    PLC.PID.Set = setpoint;
    PLC.PID.Act = PLC.ActualTemperature.Act.Value;
    PLC.PID.Error = temperaturePID.state.error;
    PLC.PID.kpError = temperaturePID.state.errore_prec;
    PLC.PID.ProportionalCorrection = temperaturePID.state.proportionalCorrection;
    PLC.PID.IntegralCorrection = temperaturePID.state.integralCorrection;
    PLC.PID.AntiWindupContribute = temperaturePID.state.antiWindupContribute;
    PLC.PID.DerivativeCorrection = temperaturePID.state.derivativeCorrection;
    PLC.PID.Correction = temperaturePID.state.totalCorrection;
    PLC.PID.PidOut = temperaturePID.state.out_pid;
    PLC.PID.RawOut = temperaturePID.state.rawOut;
    PLC.PID.OutSat = temperaturePID.state.out_tot;
    PLC.PID.Out = pid_output;
    // Test del PID. Cancellare una volta debuggato (se vuoi)

    // Gestione SSR con HeatingPWM
    static HeatingPWM_Instance ssr_pwm = {0};
    HeatingPWM_Return ssr_out = HeatingPWM(
        &ssr_pwm,
        pid_output, // duty in percentuale, positivo=riscalda, negativo=raffredda
        SSR_BURST_PERIOD_S, // durata ciclo burst (es. 30s)
        temperaturePID.params.dt, // tempo ciclo (es. 0.25s)
        SSR_BURST_HYSTERESIS_S, // isteresi (es. 5s)
        PLC.Heating.Status == 2 // enable solo se heating attivo
    );
    // Attiva/disattiva SSR fisico
    gpio_set_level(SSR_GPIO, ssr_out.positive_out ? 1 : 0);
    // (Se hai anche raffreddamento, gestisci qui ssr_out.negative_out)
    

    sclib_writeAct(&PLC.ActualPower, ssr_pwm.duty_avg); // Uscita totale del PID
}
