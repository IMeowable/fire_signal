#include "config_downloader.h"


#define JSONBUFFER 1024

void printConfigAfterInitialization()
{
   Serial.println("Config Details:");
    Serial.println("Time: " + myConfig.time);
    Serial.println("WiFi SSID: " + myConfig.wifi.ssid);
    Serial.println("WiFi Password: " + myConfig.wifi.password);
    Serial.println("Digital Pin: " + String(myConfig.pins.digital));
    Serial.println("Analog Pin: " + String(myConfig.pins.analog));
    Serial.println("API Key: " + myConfig.api_key);
    Serial.println("Chait ID: " + myConfig.chat_id);
    Serial.println("Alarm: " + String(myConfig.alarm));
    Serial.println("Security Mode: " + String(myConfig.security_mode));
    Serial.println("Last ADC Value: " + String(myConfig.last_adc_value));
    Serial.println("Last Digital Sensor State: " + String(myConfig.last_digital_sensor_state));
     Serial.println("Mqtt Settings:");
    Serial.print("Broker Address: ");
    Serial.println(mqtt_settings.brokerAddress);
    Serial.print("QoS: ");
    Serial.println(mqtt_settings.qos);
    Serial.print("Username: ");
    Serial.println(mqtt_settings.username);
    Serial.print("Password: ");
    Serial.println(mqtt_settings.password);
    Serial.print("Topic: ");
    Serial.println(mqtt_settings.topic);
    Serial.print("Keepalive: ");
    Serial.println(mqtt_settings.keepalive);
}

String serializeConfig(const Config& config) {
    DynamicJsonDocument doc(JSONBUFFER);
    doc["time"] = config.time;
    doc["wifi"]["ssid"] = config.wifi.ssid;
    doc["wifi"]["password"] = config.wifi.password;
    doc["pins"]["digital"] = config.pins.digital;
    doc["pins"]["analog"] = config.pins.analog;
    doc["api_key"] = config.api_key;
    doc["chat_id"] = config.chat_id;
    doc["alarm"] = config.alarm;
    doc["security_mode"] = config.security_mode;
    doc["last_adc_value"] = config.last_adc_value;
    doc["last_digital_sensor_state"] = config.last_digital_sensor_state;
    String serializedConfig;
    serializeJson(doc, serializedConfig);
    return serializedConfig;
}
String serializeMqttSettings(const MqttSettings& mqttSettings) {
    DynamicJsonDocument doc(JSONBUFFER);
    doc["brokerAddress"] = mqttSettings.brokerAddress;
    doc["qos"] = mqttSettings.qos;
    doc["username"] = mqttSettings.username;
    doc["password"] = mqttSettings.password;
    doc["topic"] = mqttSettings.topic;
    doc["keepalive"] = mqttSettings.keepalive;
    String serializedMqttSettings;
    serializeJson(doc, serializedMqttSettings);
    return serializedMqttSettings;
}
void writeConfigToFile(const String& data, const char* filename) {
    File file = SPIFFS.open(filename, "r+");
    if (!file) {
        Serial.println("Не удалось открыть файл для чтения.");
        return;
    }

    String fileContent = file.readString();
    file.close();

    DynamicJsonDocument doc(JSONBUFFER);
    DeserializationError error = deserializeJson(doc, fileContent);
    if (error) {
        Serial.println("deserializeJson() failed: " + String(error.c_str()));
        return;
    }

    DynamicJsonDocument newData(JSONBUFFER);
    DeserializationError newError = deserializeJson(newData, data);
    if (newError) {
        Serial.println("deserializeJson() failed for newData: " + String(newError.c_str()));
        return;
    }

    JsonArray jsonArray = doc.as<JsonArray>();
    jsonArray.add(newData.as<JsonObject>());

    

    // Если текущее количество записей превышает максимальное, удаляем старые записи
    while (jsonArray.size() > MAX_LOGS_IN_FILES) {
        jsonArray.remove(0); // Удаляем первую запись
    }

    file = SPIFFS.open(filename, "w");
    if (!file) {
        Serial.println("Не удалось открыть файл для записи.");
        return;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Не удалось записать данные в файл.");
    } else {
        Serial.println("Содержимое успешно добавлено в файл.");
    }
    file.close();
}