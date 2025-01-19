#include "home_assistant.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "HMI.h"
#include <math.h>

#ifndef CONFIG_MQTT_HOMEASSISTANT_LWT_RETAIN
#define CONFIG_MQTT_HOMEASSISTANT_LWT_RETAIN 0
#endif

#ifndef CONFIG_MQTT_HOMEASSISTANT_BIRTH_RETAIN
#define CONFIG_MQTT_HOMEASSISTANT_BIRTH_RETAIN 0
#endif

#ifndef CONFIG_MQTT_HOMEASSISTANT_DISABLE_CLEAN_SESSION
#define CONFIG_MQTT_HOMEASSISTANT_DISABLE_CLEAN_SESSION 0
#endif

static const char *TAG = "MQTT";
static char command_topic[256];
static char feedback_topic[256];

static esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = CONFIG_MQTT_HOMEASSISTANT_BROKER_URL,
    .broker.address.port = CONFIG_MQTT_HOMEASSISTANT_BROKER_PORT,
    .credentials.username = CONFIG_MQTT_HOMEASSISTANT_USERNAME,
    .credentials.authentication.password = CONFIG_MQTT_HOMEASSISTANT_PASSWORD,
    .session.keepalive = CONFIG_MQTT_HOMEASSISTANT_KEEP_ALIVE,
    .session.disable_clean_session = CONFIG_MQTT_HOMEASSISTANT_DISABLE_CLEAN_SESSION,
    .network.reconnect_timeout_ms = CONFIG_MQTT_HOMEASSISTANT_RECONNECT_TIMEOUT * 1000,
    .session.last_will.topic = CONFIG_MQTT_HOMEASSISTANT_LWT_TOPIC,
    .session.last_will.msg = CONFIG_MQTT_HOMEASSISTANT_LWT_MSG,
    .session.last_will.qos = CONFIG_MQTT_HOMEASSISTANT_LWT_QOS,
    .session.last_will.retain = CONFIG_MQTT_HOMEASSISTANT_LWT_RETAIN,
};
static esp_mqtt_client_handle_t client;

//Event handler che va a gestire gli eventi legati all'MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    //Compongo il command topic: /CONFIG_MQTT_HOMEASSISTANT_COMMAND_TOPIC/CONFIG_MQTT_HOMEASSISTANT_CLIENT_ID
    snprintf(command_topic, sizeof(command_topic), "%s/%s", CONFIG_MQTT_HOMEASSISTANT_COMMAND_TOPIC, CONFIG_MQTT_HOMEASSISTANT_CLIENT_ID);
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            // ALLA CONNESSIONE, ESEGUE LE SUBSCRIPTIONS
            msg_id = esp_mqtt_client_subscribe(client, command_topic, CONFIG_MQTT_HOMEASSISTANT_QOS);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            msg_id = esp_mqtt_client_publish(client, CONFIG_MQTT_HOMEASSISTANT_BIRTH_TOPIC, CONFIG_MQTT_HOMEASSISTANT_BIRTH_MSG, 0, CONFIG_MQTT_HOMEASSISTANT_BIRTH_QOS, CONFIG_MQTT_HOMEASSISTANT_BIRTH_RETAIN);
            ESP_LOGI(TAG, "sent birth message, msg_id=%d", msg_id);
            /*
            //Autoconfiguration will be managed later if required.             
            msg_id = esp_mqtt_client_publish(
              client,
              "homeassistant/sensor/pot_battery_level/config",
              '{"name": "Pot Battery Level","state_topic": "home/pot/battery","unit_of_measurement": "/%","value_template": "{{ value_json.battery }}","device_class": "battery","unique_id": "pot_battery_level"}',
              0,
              CONFIG_MQTT_HOMEASSISTANT_QOS,
              CONFIG_MQTT_HOMEASSISTANT_BIRTH_RETAIN
            );
            */
            ESP_LOGI(TAG, "sent birth message, msg_id=%d", msg_id);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            //AGGIORNAMENTO DEI VALORI SULLA RECEZIONE DEI COMANDI DA HMI
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

// Funzione di configurazione della sessione MQTT
void home_assistant_setup(){
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

/*
    Funzione di aggiornamento dei valori su Home Assistant
*/
void home_assistant_update(int id, int _type, void *ptrToValue) {
  switch (_type) {
      case REAL: {
        double rounded_value = round(*(float *)ptrToValue * 10000) / 10000;
        char id_str[10];
        snprintf(id_str, sizeof(id_str), "%d", id);
        snprintf(feedback_topic, sizeof(feedback_topic), "%s/%s/%s/%s", CONFIG_MQTT_HOMEASSISTANT_FEEDBACK_TOPIC, CONFIG_MQTT_HOMEASSISTANT_CLIENT_ID, "sensor", id_str);
        char value_str[32];
        snprintf(value_str, sizeof(value_str), "%.4f", rounded_value);
        esp_mqtt_client_publish(client, feedback_topic, value_str, 0, CONFIG_MQTT_HOMEASSISTANT_QOS, 0);
        break;
    }
    case INT: {
      //Compongo il feedback topic: /CONFIG_MQTT_HOMEASSISTANT_FEEDBACK_TOPIC/CONFIG_MQTT_HOMEASSISTANT_CLIENT_ID
      //"homeassistant/Pot/sensor/3";
      char id_str[10];
      snprintf(id_str, sizeof(id_str), "%d", id);
      snprintf(feedback_topic, sizeof(feedback_topic), "%s/%s/%s/%s", CONFIG_MQTT_HOMEASSISTANT_FEEDBACK_TOPIC, CONFIG_MQTT_HOMEASSISTANT_CLIENT_ID, "select", id_str);
      char value_str[32];
      snprintf(value_str, sizeof(value_str), "%f", (double)*(int *)ptrToValue);
      esp_mqtt_client_publish(client, feedback_topic, value_str, 0, CONFIG_MQTT_HOMEASSISTANT_BIRTH_QOS, 0);
      break;
    }
    case BOOL: {
      char id_str[10];
      snprintf(id_str, sizeof(id_str), "%d", id);
      snprintf(feedback_topic, sizeof(feedback_topic), "%s/%s/%s/%s", CONFIG_MQTT_HOMEASSISTANT_FEEDBACK_TOPIC, CONFIG_MQTT_HOMEASSISTANT_CLIENT_ID, "switch", id_str);
      //"homeassistant/Pot/sensor/3";
      char value_str[32];
      snprintf(value_str, sizeof(value_str), "%d", (*(int *)ptrToValue != 0));
      esp_mqtt_client_publish(client, feedback_topic, value_str, 0, CONFIG_MQTT_HOMEASSISTANT_BIRTH_QOS, 0);
      break;
    }
    case STRING:
      // Gestione delle stringhe se necessario
      break;
    case TIMESTAMP:
      // Gestione dei timestamp se necessario
      break;
    default:
      ESP_LOGE(TAG, "Unknown type");
      return;
  }
}