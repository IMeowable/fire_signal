#include "Config.h"


QueueHandle_t wifiEventQueue;
AsyncWebServer server(80);
bool isPointActive;
Config myConfig;
MqttSettings mqtt_settings;

 IPAddress static_IP;
 IPAddress gateway;
 IPAddress subnet;
 
esp_mqtt_client_config_t mqtt_cfg;
esp_mqtt_client_handle_t mqtt_client;


volatile int lastAnalogValue = 0;