#include "MQTT.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "mqtt_client.h"

#ifndef CONFIG_MQTT_LWT_RETAIN
#define CONFIG_MQTT_LWT_RETAIN 0
#endif

#ifndef CONFIG_MQTT_BIRTH_RETAIN
#define CONFIG_MQTT_BIRTH_RETAIN 0
#endif

#ifndef MQTT_DISABLE_CLEAN_SESSION
#define MQTT_DISABLE_CLEAN_SESSION 0
#endif

static const char *TAG = "MQTT";

//Event handler che va a gestire gli eventi legati all'MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, CONFIG_MQTT_SUBSCRIBE_TOPIC, CONFIG_MQTT_QOS);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            msg_id = esp_mqtt_client_publish(client, CONFIG_MQTT_BIRTH_TOPIC, CONFIG_MQTT_BIRTH_MSG, 0, CONFIG_MQTT_BIRTH_QOS, CONFIG_MQTT_BIRTH_RETAIN);
            ESP_LOGI(TAG, "sent birth message, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            mqtt_receive(event);
            ////////////////////////////QUI DEVO GESTIRE L'AGGIORNAMENTO DEI VALORI SULLA RECEZIONE DEI COMANDI DA HMI
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void mqtt_setup(){

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_MQTT_BROKER_URL,
        .broker.address.port = CONFIG_MQTT_BROKER_PORT,
        .credentials.username = CONFIG_MQTT_USERNAME,
        .credentials.authentication.password = CONFIG_MQTT_PASSWORD,
        .session.keepalive = CONFIG_MQTT_KEEP_ALIVE,
        .session.disable_clean_session = MQTT_DISABLE_CLEAN_SESSION,
        .network.reconnect_timeout_ms = CONFIG_MQTT_RECONNECT_TIMEOUT * 1000,
        .session.last_will.topic = CONFIG_MQTT_LWT_TOPIC,
        .session.last_will.msg = CONFIG_MQTT_LWT_MSG,
        .session.last_will.qos = CONFIG_MQTT_LWT_QOS,
        .session.last_will.retain = CONFIG_MQTT_LWT_RETAIN,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

static void mqtt_receive(esp_mqtt_event_handle_t event){
  ///va a leggere gli array e scrive i le variabili
}

void mqtt_send(){
  
}
