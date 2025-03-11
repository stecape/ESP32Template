#include "MQTT.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include <cJSON.h>
#include "HMI.h"
#include <math.h>

#ifndef CONFIG_MQTT_LWT_RETAIN
#define CONFIG_MQTT_LWT_RETAIN 0
#endif

#ifndef CONFIG_MQTT_BIRTH_RETAIN
#define CONFIG_MQTT_BIRTH_RETAIN 0
#endif

#ifndef CONFIG_MQTT_DISABLE_CLEAN_SESSION
#define CONFIG_MQTT_DISABLE_CLEAN_SESSION 0
#endif

#ifndef CONFIG_MQTT_HOMEASSISTANT
#define CONFIG_MQTT_HOMEASSISTANT 0
#endif

#if CONFIG_MQTT_HOMEASSISTANT
#include "home_assistant/home_assistant.h"
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

static const char *TAG = "MQTT";
static char command_topic[256];
static char feedback_topic[256];

static size_t array_length = ARRAY_SIZE(id);

static esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = CONFIG_MQTT_BROKER_URL,
    .broker.address.port = CONFIG_MQTT_BROKER_PORT,
    .credentials.username = CONFIG_MQTT_USERNAME,
    .credentials.authentication.password = CONFIG_MQTT_PASSWORD,
    .session.keepalive = CONFIG_MQTT_KEEP_ALIVE,
    .session.disable_clean_session = CONFIG_MQTT_DISABLE_CLEAN_SESSION,
    .network.reconnect_timeout_ms = CONFIG_MQTT_RECONNECT_TIMEOUT * 1000,
    .session.last_will.topic = CONFIG_MQTT_LWT_TOPIC,
    .session.last_will.msg = CONFIG_MQTT_LWT_MSG,
    .session.last_will.qos = CONFIG_MQTT_LWT_QOS,
    .session.last_will.retain = CONFIG_MQTT_LWT_RETAIN,
};
static esp_mqtt_client_handle_t client;


/*
    Funzione di receive.
    Le variabili HMI sono organizzate su 3 arrays:
    id, type, pointer
    Quando ricevi un command:
    {"id":xxx, "value":yyy}
    Vai a cercare l'id nel rispettivo array. 
    La sua posizione (il cursore i della for) verrà utilizzata per andare ad incrociare negli altri due array
    il relativo tipo ed il puntatore alla variabile nella memoria dell'ESP32.
    Questo perché l'array pointer è un array di puntatori generici, e quindi devi fare il cast col tipo corretto
    della variabile che puntano prima di eseguire la scrittura.
*/
static void mqtt_receive(esp_mqtt_event_handle_t event){
  ///va a leggere gli array e scrive i le variabili
    cJSON *root = cJSON_Parse(event->data);
    if (root == NULL) {
        ESP_LOGE(TAG, "Error parsing JSON\n");
        return;
    }

    cJSON *id_json = cJSON_GetObjectItem(root, "id");
    cJSON *value_json = cJSON_GetObjectItem(root, "value");

    if (!cJSON_IsNumber(id_json) || !cJSON_IsNumber(value_json)) {
        ESP_LOGE(TAG, "Invalid JSON format\n");
        cJSON_Delete(root);
        return;
    }
    
    
    int id_value = id_json->valueint;
    double value = value_json->valuedouble;
    
    ESP_LOGI(TAG, "Received ID: %d", id_value);
    ESP_LOGI(TAG, "Received Value: %f", value);

    for (int i = 0; i < ARRAY_SIZE(id); i++) {
        if (id[i] == id_value) {
            switch (type[i]) {
                case REAL:
                    *(float *)pointer[i] = (float)value;
                    break;
                case INT:
                    *(int *)pointer[i] = (int)value;
                    break;
                case BOOL:
                    *(int *)pointer[i] = (value != 0);
                    break;
                case STRING:
                    // Gestione delle stringhe se necessario
                    break;
                case TIMESTAMP:
                    // Gestione dei timestamp se necessario
                    break;
                default:
                    ESP_LOGE(TAG, "Unknown type\n");
                    break;
            }
            break;
        }
    }

    cJSON_Delete(root);
}

//Event handler che va a gestire gli eventi legati all'MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    //Compongo il command topic: /CONFIG_MQTT_COMMAND_TOPIC/CONFIG_MQTT_CLIENT_ID
    snprintf(command_topic, sizeof(command_topic), "%s/%s", CONFIG_MQTT_COMMAND_TOPIC, CONFIG_MQTT_CLIENT_ID);
    //Compongo il feedback topic: /CONFIG_MQTT_FEEDBACK_TOPIC/CONFIG_MQTT_CLIENT_ID
    snprintf(feedback_topic, sizeof(feedback_topic), "%s/%s", CONFIG_MQTT_FEEDBACK_TOPIC, CONFIG_MQTT_CLIENT_ID);
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            // ALLA CONNESSIONE, ESEGUE LE SUBSCRIPTIONS
            msg_id = esp_mqtt_client_subscribe(client, command_topic, CONFIG_MQTT_QOS);
            msg_id = esp_mqtt_client_subscribe(client, CONFIG_MQTT_COMMAND_TOPIC_BROADCAST, CONFIG_MQTT_QOS);
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
            //AGGIORNAMENTO DEI VALORI SULLA RECEZIONE DEI COMANDI DA HMI
            mqtt_receive(event);
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

// Funzione di configurazione della sessione MQTT
void mqtt_setup(){
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
#if CONFIG_MQTT_HOMEASSISTANT
    home_assistant_setup();
#endif
}

/*
    Funzione di aggiornamento dei valori sull'HMI.
    Al momento vengono spediti messaggi in json col seguente formato:
    {"id":xxx, "value":yyyy}
    Sto pensando di passare ad una serie di coppie <id,value> in CSV, per alleggerire:
    xxx,yyy
    zzz,aaa
*/
void mqtt_updHMI(void *ptrToHMIVar, void *ptrToValue) {
    cJSON *root = cJSON_CreateObject();
    for (size_t i = 0; i < array_length; i++) {
        if (pointer[i] == ptrToHMIVar) {
            cJSON_AddNumberToObject(root, "id", id[i]);
            switch (type[i]) {
                case REAL: {
                    *(float *)ptrToHMIVar = *(float *)ptrToValue;
                    double rounded_value = round(*(float *)ptrToValue * 10000) / 10000;
                    cJSON_AddNumberToObject(root, "value", rounded_value);
                    break;
                }
                case INT: {
                    *(int *)ptrToHMIVar = *(int *)ptrToValue;
                    cJSON_AddNumberToObject(root, "value", (double)*(int *)ptrToValue);
                    break;
                }
                case BOOL: {
                    *(int *)ptrToHMIVar = (*(int *)ptrToValue != 0);
                    cJSON_AddBoolToObject(root, "value", (*(int *)ptrToValue != 0));
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
                    cJSON_Delete(root);
                    return;
            }
            char *payload = cJSON_Print(root);
            esp_mqtt_client_publish(client, feedback_topic, payload, 0, CONFIG_MQTT_BIRTH_QOS, 0);

            cJSON_Delete(root);
            free(payload);
            
#if CONFIG_MQTT_HOMEASSISTANT
            home_assistant_update(id[i], type[i], ptrToValue);
#endif
            break;
        }
    }
}