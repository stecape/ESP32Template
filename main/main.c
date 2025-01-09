/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "esp_event.h"
#include "HMI.h"
#include "peripherials/led/led.h"
#include "services/mqtt/mqtt.h"
#include "services/Wifi/Wifi.h"
#include "services/battery/battery.h"

//Librerie per test
#include <math.h>
//Librerie per test

#define CONFIG_INTERRUPT_CYCLE_TIME_S 5

void setup() {
  // Setup calls
  ESP_LOGI("HELLO", "Ciao Cacà, Ciao Tommy! =)");
  // Create default event loop
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  Wifi_setup();
  mqtt_setup();
  led_setup();
  battery_setup();
}


void loop() {
  // Loop calls
  led_loop();
  battery_loop(&HMI.BatteryLevel.Act.Value, &HMI.BatteryLevel.Limit.Min, &HMI.BatteryLevel.Limit.Max);

}


void interrupt() {
  // Interrupt calls
  led_interrupt();
}




static TaskHandle_t task_handle = NULL;

// Definizione della ISR del timer
bool IRAM_ATTR timer_isr_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // Segnala il task
    vTaskNotifyGiveFromISR(task_handle, &xHigherPriorityTaskWoken);
    // Forza il contesto di switch se necessario
    return xHigherPriorityTaskWoken == pdTRUE;
}

// Funzione per configurare il timer
void init_timer() {
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1 MHz per ottenere microsecondi
    };
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = CONFIG_INTERRUPT_CYCLE_TIME_S * 1000000, // 5 secondi (5 milioni di microsecondi)
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };

    // Creazione e configurazione del timer
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &(gptimer_event_callbacks_t){
        .on_alarm = timer_isr_callback,
    }, NULL));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

// Task dedicato per gestire l'operazione richiesta
void interrupt_task(void *arg) {
  while (true) {
    // Attende la notifica dalla ISR
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    interrupt();
  }
}

void app_main(void) {
  // Inizializza in timer
  init_timer();
  // Creazione del task dedicato
  xTaskCreatePinnedToCore(interrupt_task, "interrupt_task", 4096, NULL, 10, &task_handle, 1); // Pinning to core 1
  setup();

  while (true) {
    loop();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}