#include "configInit.h"
#include "server_handlers.h"
#include "timers_for_func.h"
#include "config_downloader.h"
#include "readFromHtml.h"
#include "telegramwork.h"
#include "temperatureHandle.h"
#include "mqtt_work.h"

#define FIRE "Возможен пожар!"
#define GAS "Утечка газа!"
#define GASFIRE "В здании пожар!"
#define WIFI_TIMER_INTERVAL  1500
#define TEMPERATURE_TIMER_INTERVAL  700
#define GAS_TIMER_INTERVAL  150
#define ALARM_CARE_TIMER_INTERVAL  1000
#define MOSCOW_TIME_TIMER_INTERVAL  6000
#define MQTT_JSON_INTERVAL  6200

String file_name = "/logs.json";
String mqtt_file_name = "/mqtt_settings.json";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3 * 3600, 60000);

unsigned long handlerDigTimer = 0;
unsigned long handlerAnalogTimer = 0;
unsigned long handlerWifiTimer = 0;
unsigned long handlerSOFTAPTimer = 0;
unsigned long handlerMoscowTimer = 0;
unsigned long handleCareTimer = 0;
unsigned long sendToMQTTJSONTimer = 0;

bool IsConnected = false;

void toWifiPoint() {
  WiFi.softAP(myConfig.wifi.ssid.c_str(), myConfig.wifi.password.c_str());
  WiFi.softAPConfig(static_IP, gateway, subnet);
  Serial.print("AP IP address: ");
  Serial.println(static_IP);
  isPointActive = true;
}

void (*softapHandlerPtr)() = toWifiPoint;

void WiFiHandling() {
  WiFiEvent_t event;
  if (xQueueReceive(wifiEventQueue, &event, 1000) == pdPASS) {
    if (event == SYSTEM_EVENT_STA_GOT_IP) {
      Serial.println("Connected to Wi-Fi");   
      Serial.println(WiFi.localIP());
      timeClient.begin();
      IsConnected = true;
      setupServer();
    } else if (event == SYSTEM_EVENT_STA_DISCONNECTED & !isPointActive) {
      Serial.println(".");
      Serial.println("Wi-Fi connection lost");
      Serial.println(".");
      set_timer_for_function(softapHandlerPtr,15000, &handlerSOFTAPTimer); 
    }
  }
}  

void WiFiEventCallback(WiFiEvent_t event) {
  xQueueSendFromISR(wifiEventQueue, &event, NULL);
}

void gasHandle() {
  if(digitalRead(myConfig.pins.digital) == HIGH) {
    myConfig.last_digital_sensor_state = HIGH;
    Serial.println("Утечка газа");
  }
  else{
    myConfig.last_digital_sensor_state = LOW;
  }
}

void writeCurrentSettingsToFile(){
    String log_to_file = serializeConfig(myConfig);
  String mqtt_log_to_file = serializeMqttSettings(mqtt_settings);
  Serial.println(myConfig.time);
  Serial.println("Начинаем писать в файл");
  writeConfigToFile(log_to_file, file_name.c_str());
  writeConfigToFile(mqtt_log_to_file, mqtt_file_name.c_str());
}

void getMoscowTime() {
  timeClient.update();
  time_t rawTime = timeClient.getEpochTime();
  rawTime += 3 * 3600;
  struct tm *timeInfo;
  timeInfo = localtime(&rawTime);
  char formattedDate[20];  
  strftime(formattedDate, sizeof(formattedDate), "%Y-%m-%d", timeInfo);
  String formattedTime = timeClient.getFormattedTime();
  myConfig.time = String(formattedDate) + " " + formattedTime;
  writeCurrentSettingsToFile();
}

void alarm_care() {
  if(myConfig.security_mode && myConfig.alarm) { 
    String message = "Температура критическая, возможен пожар!Время тревоги " + myConfig.time;
    sendTelegramMessage(message);
    if(myConfig.last_digital_sensor_state == HIGH) {
      sendTelegramMessage("Повысилась, концентрация газа, пожар!Время тревоги" + myConfig.time);
  }
}
}

void set_dac_enable() {
  dac_output_enable(DAC_CHANNEL_2);
}

void configure_pins() {
  pinMode(myConfig.pins.analog, INPUT);
  pinMode(myConfig.pins.digital, INPUT);
   adc1_config_width(ADC_WIDTH_BIT_12); // настроили разрядность ацп
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11); // настроились на канал
  set_dac_enable();   
}

void mqtt_app_start() {
   mqtt_cfg.uri = mqtt_settings.brokerAddress.c_str();
  mqtt_cfg.keepalive = mqtt_settings.keepalive;
  mqtt_cfg.username = mqtt_settings.username.c_str();
  mqtt_cfg.password = mqtt_settings.password.c_str();
  mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_CONNECTED, mqtt_event_handler_wrapper, NULL);
  esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_DISCONNECTED, mqtt_event_handler_wrapper, NULL);
  esp_mqtt_client_start(mqtt_client); 
}
void setSettings()
{
  String init_settings =  readSettingsFromFile(file_name);
  Config initConf = deserializeLatestEntry(init_settings);
  String init_mqtt = readSettingsFromFile(mqtt_file_name);
  MqttSettings initMqtt = deserializeLatestMqttSettings(init_mqtt);
  setConfig(initConf);
  setMqttSettings(initMqtt);
}
void startWifi() {
 WiFi.begin(myConfig.wifi.ssid.c_str(), myConfig.wifi.password.c_str());
  wifiEventQueue = xQueueCreate(10, sizeof(WiFiEvent_t));
  WiFi.onEvent(WiFiEventCallback);  
}
void setupMQTT()
{
  mqtt_cfg.uri = mqtt_settings.brokerAddress.c_str();
  mqtt_app_start();
}
void setup() {
  Serial.begin(115200);
  initSPIFFS();
  setSettings();
  configure_pins();
  printConfigAfterInitialization();
  startWifi();
  setupMQTT();
}

//указатели такого же типа как функции, чтобы эти функции через указатель можно было использовать как аргумент функции
//теперь нам не надо писать миллисы для каждой функции, у нас есть одна
//это можно было сделать готовым способом, но мне было интересно сделать самому
void (*getMoscowTimeptr)() = getMoscowTime;
void (*alarmCareTimeptr)() = alarm_care;
void (*temperatureHandlerPtr)() = temperatureHandle;
void (*gasHandlerPtr)() = gasHandle;
void (*wifiHandlePtr)() = WiFiHandling;
void (*mqttSendPtr)() = send_json_to_mqtt;

void startConnectionRelaitedFunctions() {
set_timer_for_function(getMoscowTimeptr, MOSCOW_TIME_TIMER_INTERVAL, &handlerMoscowTimer);
set_timer_for_function(mqttSendPtr, MQTT_JSON_INTERVAL, &sendToMQTTJSONTimer);
set_timer_for_function(alarmCareTimeptr, ALARM_CARE_TIMER_INTERVAL, &handleCareTimer);
}
void startBasicFunctions() 
{
   set_timer_for_function(wifiHandlePtr, WIFI_TIMER_INTERVAL, &handlerWifiTimer);
   set_timer_for_function(temperatureHandlerPtr, TEMPERATURE_TIMER_INTERVAL, &handlerAnalogTimer);
  set_timer_for_function(gasHandlerPtr, GAS_TIMER_INTERVAL, &handlerDigTimer);
}
void loop() {
  startBasicFunctions();
  IsConnected?(startConnectionRelaitedFunctions()): void();           
}
