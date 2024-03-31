#pragma once

#include "Arduino.h"
#include "ArduinoJson.h"
#include "SPIFFS.h"
#include "config.h"

#define DEFAULT_APIN 32
#define DEFUALT_DPIN 27
#define DEFAULT_SSID "Checkwifi"
#define DEFAULT_PASSWORD "11111111"
#define DEFAULT_API ""
#define DEFAULT_CHATID ""
#define DEFAULT_APIN_STATE 512
#define DEFAULT_DPIN_STATE 0
#define DEFAULT_SECURITY_MOD 1
#define DEFAULT_ALARM_STATUS 0
#define DEFAULT_BROKER_ADDRESS "" 
#define DEFAULT_QOS 0
#define DEFAULT_USERNAME ""
#define DEFAULT_PASSWORD_WIFI "11111111"
#define DEFAULT_PASSWORD "pass_0600c969"
#define DEFAULT_TOPIC ""
#define DEFAULT_KEEPALIVE 60



extern volatile int currentAnalogValue;

 void initSPIFFS();
 String readSettingsFromFile(String FILE_NAME);
void setConfig(Config configData);
Config deserializeLatestEntry(const String& fileData);
MqttSettings deserializeLatestMqttSettings(const String &fileData);
void setMqttSettings(MqttSettings mqttSettingsData);