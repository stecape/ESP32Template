#include "alarms.h"
#include "esp_log.h"
#include "esp_timer.h"

bool alarmsAck = false;
int64_t alarmsAckTime = 0;

static const char *TAG = "ALARMS";

// Funzione per riconoscere gli allarmi. Deve partire un task che scrive a true alarmsAck per un certo tempo (1 secondo)
// e poi lo riporta a false.
void alarms_ack(void) {
    alarmsAck = true;
    alarmsAckTime = esp_timer_get_time();
    ESP_LOGI(TAG, "Alarms acknowledged");
 }
// Funzione per controllare gli allarmi. Se sono stati riconosciuti e sono passati più di 1 secondo, li resetta.
void check_alarms(void) {
  if (alarmsAck && (esp_timer_get_time() - alarmsAckTime > 1000000)) { // 1 secondo in microsecondi
      alarmsAck = false;
  }
}

void alarm (Alarm *alarm, bool execute, AlarmReaction reaction) {
  if (alarm->Reaction == ALARM_REACTION_NO_REACTION) {
    alarm->Reaction = reaction;
    alarm->Ts = esp_timer_get_time()/1000;
  }
  if (alarm->Status == ALARM_STATUS_INACTIVE && execute) {
    alarm->Status = ALARM_STATUS_ACTIVE;
    alarm->Ts = esp_timer_get_time()/1000;
    // Inviare un messaggio MQTT o eseguire altre azioni
  } else if (alarm->Status == ALARM_STATUS_ACTIVE && alarmsAck) {
    alarm->Status = ALARM_STATUS_ACKNOWLEDGED;
  } else if (alarm->Status == ALARM_STATUS_ACTIVE && !execute) {
    alarm->Status = ALARM_STATUS_GONE;
  } else if (alarm->Status == ALARM_STATUS_ACKNOWLEDGED && !execute) {
    alarm->Status = ALARM_STATUS_INACTIVE;
    alarm->Ts = esp_timer_get_time()/1000; // Convert to milliseconds
  } else if (alarm->Status == ALARM_STATUS_GONE && execute) {
    alarm->Status = ALARM_STATUS_ACTIVE;
    alarm->Ts = esp_timer_get_time()/1000; // Convert to milliseconds
  } else if (alarm->Status == ALARM_STATUS_GONE && alarmsAck) {
    alarm->Status = ALARM_STATUS_INACTIVE;
    alarm->Ts = esp_timer_get_time()/1000; // Convert to milliseconds
  }

  if (alarm->Status == ALARM_STATUS_INACTIVE) {
    alarm->Q = false;
  } else {
    alarm->Q = true;
  }
}