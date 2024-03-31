#ifndef MQTT_WORK_H
#define MQTT_WORK_H

#include "config.h"
#include "configInit.h"
#include <WiFi.h>


#include <Ticker.h>


extern Ticker mqtt_ticker;

#define MQTT_BROKER_URL 
#define MQTT_TOPIC 

#endif // MQTT_WORK_H
void send_json_to_mqtt();

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);

 void mqtt_event_handler_wrapper(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
