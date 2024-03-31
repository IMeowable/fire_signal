#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <driver/dac.h>
#include <driver/adc.h>
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"


extern volatile int lastAnalogValue;

extern esp_mqtt_client_config_t mqtt_cfg;
extern esp_mqtt_client_handle_t mqtt_client;

extern QueueHandle_t wifiEventQueue;
extern AsyncWebServer server;
extern bool isPointActive;
extern IPAddress static_IP;
extern IPAddress gateway;
extern IPAddress subnet;

struct WifiConfig {
    String ssid;
    String password;
};

struct PinsConfig {
    int digital;
    int analog;
};

struct Config {
    String time;
    WifiConfig wifi;
    PinsConfig pins;
    String api_key;
    String chat_id;
    bool alarm;
    bool security_mode;
    int last_adc_value;
    bool last_digital_sensor_state;
};
struct MqttSettings {
  String brokerAddress;
  int qos;
  String username;
  String password;
  String topic;
  int keepalive;
};

extern MqttSettings mqtt_settings;
extern Config myConfig;

#endif 
