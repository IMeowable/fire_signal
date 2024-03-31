#include "mqtt_work.h"

Ticker mqtt_ticker;

void send_json_to_mqtt()
{

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "time", myConfig.time.c_str());
    cJSON_AddStringToObject(root, "ssid", myConfig.wifi.ssid.c_str());
    cJSON_AddStringToObject(root, "password", myConfig.wifi.password.c_str());
    cJSON_AddNumberToObject(root, "digitalpin", myConfig.pins.digital);
    cJSON_AddNumberToObject(root, "analogpin", myConfig.pins.analog);
    cJSON_AddStringToObject(root, "api_key", myConfig.api_key.c_str());
    cJSON_AddStringToObject(root, "chatid", myConfig.chat_id.c_str());
    cJSON_AddBoolToObject(root, "alarm", myConfig.alarm);
    cJSON_AddBoolToObject(root, "securitymode", myConfig.security_mode);
    cJSON_AddBoolToObject(root, "lastdigitalstate", myConfig.last_digital_sensor_state);
    cJSON_AddNumberToObject(root, "lastadcvalue", myConfig.last_adc_value);
    char *json_string = cJSON_Print(root);
    cJSON_Delete(root);
    esp_mqtt_client_publish(mqtt_client, mqtt_settings.topic.c_str(), json_string, 0, mqtt_settings.qos, 0);
    free(json_string);
}

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI("MQTT", "Connected to broker");
            esp_mqtt_client_publish(event->client, mqtt_settings.topic.c_str(), "Hello from ESP32", 0, mqtt_settings.qos, 0);
            Serial.printf("MQTT_EVENT_CONNECTED Start Message send\r\n");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI("MQTT", "Disconnected from broker");
            break;
        default:
            break;
    }
    return ESP_OK;
}

void mqtt_event_handler_wrapper(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    mqtt_event_handler((esp_mqtt_event_handle_t)event_data);
}

