#include "temperature.h"
#include "sclib/hmi_tools/hmi_tools.h"
#include "sclib/control/PID/pid.h"
#include "peripherials/thermocouple_spi/thermocouple_spi.h"
#include "HMI.h"
#include "driver/gpio.h"

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

#define SSR_GPIO 5
#define SSR_BURST_PERIOD_MS 250
#define SSR_BURST_STEPS 25

static volatile uint32_t ssr_tick_10ms = 0;

static void ssr_pwm_task(void *arg) {
    while (1) {
        bool ssr_on = PID_SSR_Burst(&temperaturePID, ssr_tick_10ms);
        gpio_set_level(SSR_GPIO, ssr_on ? 1 : 0);
        ssr_tick_10ms = (ssr_tick_10ms + 1) % SSR_BURST_STEPS;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void temperature_ssr_setup(void);

void temperature_setup(void) {
    PID_Init(&temperaturePID, &temperature_pid_params);
    thermocouple_init();
    temperature_ssr_setup();
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
    // Sincronizza il burst PWM SSR con il nuovo ciclo PID
    ssr_tick_10ms = 0;
}

void temperature_ssr_setup(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SSR_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(SSR_GPIO, 0);
    xTaskCreatePinnedToCore(ssr_pwm_task, "ssr_pwm_task", 2048, NULL, 5, NULL, 1);
}
