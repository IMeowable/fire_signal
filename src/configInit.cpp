#include "configinit.h"



volatile int currentAnalogValue = 0;


const int JSON_BUFFER_SIZE = 1024;

void initSPIFFS() {
  if (!SPIFFS.begin(false)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    delay(10000);
    ESP.restart();
  }
  Serial.println("SPIFFS mounted successfully");

}
 String readSettingsFromFile(String FILE_NAME)
{

    File file = SPIFFS.open(FILE_NAME, "r");
    String content = "";
    if (file) {
        while (file.available()) {
           content += (char)file.read();
        }
        file.close();
    } else {
        Serial.println("Failed to read  file");
    }
    return content;
}
MqttSettings deserializeLatestMqttSettings(const String &fileData) {
    if (fileData.isEmpty()) {
        Serial.println("Входные данные пусты.");
        return MqttSettings(); 
    }
    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    DeserializationError error = deserializeJson(doc, fileData);

    if (error) {
        Serial.print("Ошибка deserializeJson(): ");
        Serial.println(error.c_str());
        return MqttSettings(); 
    }
    
    if (!doc.is<JsonArray>()) {
        Serial.println("Это не JSON-массив.");
        return MqttSettings(); 
    }
    JsonArray jsonArray = doc.as<JsonArray>();
    if (jsonArray.isNull() || jsonArray.size() == 0) {
        Serial.println("JSON-массив пуст.");
        return MqttSettings();
    }
    JsonObject latestEntry = jsonArray[jsonArray.size() - 1];
    MqttSettings mqttSettings;
    mqttSettings.brokerAddress = latestEntry["brokerAddress"].as<String>();
mqttSettings.qos = latestEntry["qos"].as<int>();
mqttSettings.username = latestEntry["username"].as<String>();
mqttSettings.password = latestEntry["password"].as<String>();
mqttSettings.topic = latestEntry["topic"].as<String>();
mqttSettings.keepalive = latestEntry["keepalive"].as<int>();
    Serial.println("Возвращаемые значения при парсинге:");

Serial.print("Broker Address: ");
Serial.println(mqttSettings.brokerAddress);

Serial.print("QoS: ");
Serial.println(mqttSettings.qos);

Serial.print("Username: ");
Serial.println(mqttSettings.username);

Serial.print("Password: ");
Serial.println(mqttSettings.password);

Serial.print("Topic: ");
Serial.println(mqttSettings.topic);

Serial.print("Keepalive: ");
Serial.println(mqttSettings.keepalive);
    return mqttSettings;
}
Config deserializeLatestEntry(const String &fileData) {
    if (fileData.isEmpty()) {
        Serial.println("Входные данные пусты.");
        return Config();
    }
    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    DeserializationError error = deserializeJson(doc, fileData);
    if (error) {
        Serial.print("Ошибка deserializeJson(): ");
        Serial.println(error.c_str());
        return Config();
    }
    Serial.println("Десериализованный JSON:");
    serializeJsonPretty(doc, Serial);
    Serial.println();
    Config latestConfig;
    if (doc.is<JsonArray>()) {
        Serial.println("Это JSON-массив.");
        JsonArray jsonArray = doc.as<JsonArray>();
        if (!jsonArray.isNull()) {
            JsonObject latestEntry = jsonArray[jsonArray.size() - 1];
            if (latestEntry.containsKey("time") &&
                latestEntry["wifi"].containsKey("ssid") &&
                latestEntry["wifi"].containsKey("password") &&
                latestEntry["pins"].containsKey("digital") &&
                latestEntry["pins"].containsKey("analog") &&
                latestEntry.containsKey("api_key") &&
                latestEntry.containsKey("chat_id") &&
                latestEntry.containsKey("alarm") &&
                latestEntry.containsKey("security_mode") &&
                latestEntry.containsKey("last_adc_value") &&
                latestEntry.containsKey("last_digital_sensor_state")) {
                latestConfig.time = latestEntry["time"].as<String>();
                latestConfig.wifi.ssid = latestEntry["wifi"]["ssid"].as<String>();
                latestConfig.wifi.password = latestEntry["wifi"]["password"].as<String>();
                latestConfig.pins.digital = latestEntry["pins"]["digital"].as<int>();
                latestConfig.pins.analog = latestEntry["pins"]["analog"].as<int>();
                latestConfig.api_key = latestEntry["api_key"].as<String>();
                latestConfig.chat_id = latestEntry["chat_id"].as<String>();
                latestConfig.alarm = latestEntry["alarm"].as<bool>();
                latestConfig.security_mode = latestEntry["security_mode"].as<bool>();
                latestConfig.last_adc_value = latestEntry["last_adc_value"].as<int>();
                latestConfig.last_digital_sensor_state = latestEntry["last_digital_sensor_state"].as<bool>();
            } else {
                Serial.println("Некоторые обязательные ключи отсутствуют в последней записи массива.");
            }
        } else {
            Serial.println("JSON-массив пуст.");
        }
    } else {
        Serial.println("Это не JSON-массив.");
    }
    return latestConfig;
}
void setConfig(Config configData) {
    myConfig.alarm = (configData.alarm != NULL) ? configData.alarm : false;
    myConfig.security_mode = configData.security_mode;
    myConfig.wifi.ssid = (configData.wifi.ssid != "") ? configData.wifi.ssid : DEFAULT_SSID;
    myConfig.wifi.password = (configData.wifi.password != "") ? configData.wifi.password : DEFAULT_PASSWORD_WIFI;
    myConfig.pins.analog = (configData.pins.analog != 0) ? configData.pins.analog : DEFAULT_APIN;
    myConfig.chat_id = (configData.chat_id != "") ? configData.chat_id : DEFAULT_CHATID;
    myConfig.pins.digital = (configData.pins.digital != 0) ? configData.pins.digital : DEFUALT_DPIN;
    myConfig.api_key = (configData.api_key != "") ? configData.api_key : DEFAULT_API;
    myConfig.last_adc_value = (configData.last_adc_value != 0) ? configData.last_adc_value : DEFAULT_APIN_STATE;
    myConfig.last_digital_sensor_state = DEFAULT_DPIN_STATE;
}

void setMqttSettings(MqttSettings mqttSettingsData) {
    mqtt_settings.brokerAddress = (mqttSettingsData.brokerAddress != "0") ? mqttSettingsData.brokerAddress : DEFAULT_BROKER_ADDRESS;
    mqtt_settings.qos = mqttSettingsData.qos;
    mqtt_settings.username = (mqttSettingsData.username != "0") ? mqttSettingsData.username : DEFAULT_USERNAME;
    mqtt_settings.password = (mqttSettingsData.password != "0") ? mqttSettingsData.password : DEFAULT_PASSWORD;
    mqtt_settings.topic = (mqttSettingsData.topic != "0") ? mqttSettingsData.topic : DEFAULT_TOPIC;
    mqtt_settings.keepalive = (mqttSettingsData.keepalive != 0) ? mqttSettingsData.keepalive : DEFAULT_KEEPALIVE;
}