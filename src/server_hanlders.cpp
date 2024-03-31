#include "server_handlers.h"
#include "temperatureHandle.h"

void sendHtmlResponse(AsyncWebServerRequest *request, const String &htmlFilePath) {
    String contentType = "text/html";
    String htmlCode = readHtmlFile(htmlFilePath);
    Serial.println("Sending HTML response");
    request->send(html_OK, contentType, htmlCode);
}

void handleRoot(AsyncWebServerRequest *request) {
    sendHtmlResponse(request, "/index.html");
}

void handleWiFiHtml(AsyncWebServerRequest *request) {
    sendHtmlResponse(request, "/WiFI.html");
}

void handleConfirmSelection(AsyncWebServerRequest *request) {
    String analogPin = request->arg("analog-pin");
    String digitalPin = request->arg("digital-pin");
    setAnalogAndDigitalPins(analogPin.toInt(), digitalPin.toInt());
    request->send(html_OK, "text/plain", "Success");
}

void handleSaveWifiSettings(AsyncWebServerRequest *request) {
    String ssid = request->arg("ssid");
    String password = request->arg("password");
    setWifiSettings(ssid, password);
    request->send(html_OK, "text/plain", "Настройки Wi-Fi сохранены успешно");
}

void handleAlarmingHtml(AsyncWebServerRequest *request) {
    sendHtmlResponse(request, "/Alarming.html");
}

void handleGetAnalogData(AsyncWebServerRequest *request) {
    int analogPinValue = myConfig.last_adc_value;
    sendJsonResponse(request, "analogValue", analogPinValue);
}

void handleGetDigitalData(AsyncWebServerRequest *request) {
    int digitalPinValue = myConfig.last_digital_sensor_state;
    sendJsonResponse(request, "digitalValue", digitalPinValue);
}

void handleToggleAlarm(AsyncWebServerRequest *request) {
    String alarmStatus = request->arg("alarmStatus");
    myConfig.security_mode = (alarmStatus == "true") ? true : false;
    request->send(html_OK, "text/plain", "Success");
}

void handleGetAlarmStatus(AsyncWebServerRequest *request) {
    String responseJson;
    if (myConfig.security_mode) {
        if (myConfig.last_adc_value == HIGH or myConfig.last_adc_value >= 2000) {
            responseJson = "{\"alarmStatus\": \"Пожар или утечка газа.\"}";
        } else {
            responseJson = "{\"alarmStatus\": \"Все хорошо.\"}";
        }
        request->send(html_OK, "application/json", responseJson);
    } else {
        responseJson = "{\"alarmStatus\": \"Режим охраны отключен.\"}";
        request->send(html_OK, "application/json", responseJson);
    }
}

void handleGetSecurityStatus(AsyncWebServerRequest *request) {
    DynamicJsonDocument responseJson(json_buffer);
    responseJson["alarmStatus"] = myConfig.security_mode ? "true" : "false";
    sendJsonResponse(request, responseJson);
}

void handleSaveAlarmSettings(AsyncWebServerRequest *request) {
    myConfig.api_key = request->arg("apiKey");
    myConfig.api_key = request->arg("chatId");
    request->send(html_OK, "text/plain", "Настройки оповещения сохранены успешно");
}


void setAnalogAndDigitalPins(int analogPin, int digitalPin) {
    myConfig.pins.analog = analogPin;
    myConfig.pins.digital = digitalPin;
}

void setWifiSettings(const String &ssid, const String &password) {
    myConfig.wifi.password = password;
    myConfig.wifi.ssid = ssid;
}

void sendJsonResponse(AsyncWebServerRequest *request, const String &key, int value) {
    DynamicJsonDocument jsonDoc(json_buffer);
    jsonDoc[key] = value;
    String responseJson;
    serializeJson(jsonDoc, responseJson);
    request->send(html_OK, "application/json", responseJson);
}
void sendJsonResponse(AsyncWebServerRequest *request, DynamicJsonDocument &jsonDoc) {
    String responseJson;
    serializeJson(jsonDoc, responseJson);
    request->send(html_OK, "application/json", responseJson);
}
void handleSetDAC(AsyncWebServerRequest *request) {
  
  String temporaryvalue = request->arg("value");
  Serial.println("Check value is: " + temporaryvalue);
   dac_output_voltage(DAC_CHANNEL_2, temporaryvalue.toInt());     
}


 void getMQTTSetings(AsyncWebServerRequest *request) {
 StaticJsonDocument<200> jsonDocument;
    DeserializationError error = deserializeJson(jsonDocument, request->arg("plain"));
    
    // Проверка наличия и типов данных
    if (error) {
      Serial.println("Failed to parse JSON");
      request->send(400, "application/json", "{\"message\": \"Failed to parse JSON\"}");
      return;
    }
    mqtt_settings.brokerAddress = jsonDocument["brokerAddress"].as<String>();
    mqtt_settings.qos = jsonDocument["qos"];
    mqtt_settings.username = jsonDocument["username"].as<String>();
    mqtt_settings.password = jsonDocument["password"].as<String>();
    mqtt_settings.topic = jsonDocument["topic"].as<String>();
    mqtt_settings.keepalive = jsonDocument["keepalive"];
    Serial.println("Broker Address: " + mqtt_settings.brokerAddress);
    Serial.println("QoS: " + String(mqtt_settings.qos));
    Serial.println("Username: " + mqtt_settings.username);
    Serial.println("Password: " + mqtt_settings.password);
    Serial.println("Topic: " + mqtt_settings.topic);
    Serial.println("Keepalive: " + String(mqtt_settings.keepalive));

    request->send(200, "application/json", "{\"message\": \"Settings saved successfully\"}");  
 }
  


void setupServer() {
    
    server.on("/", HTTP_GET, handleRoot);
    
    server.on("/WiFi.html", HTTP_GET, handleWiFiHtml);
    server.on("/confirmSelection", HTTP_POST, handleConfirmSelection);
    server.on("/saveWifiSettings", HTTP_POST, handleSaveWifiSettings);
    server.on("/Alarming.html", HTTP_GET, handleAlarmingHtml);
    server.on("/getAnalogData", HTTP_GET, handleGetAnalogData);
    server.on("/getDigitalData", HTTP_GET, handleGetDigitalData);
    server.on("/toggleAlarm", HTTP_POST, handleToggleAlarm);
    server.on("/getAlarmStatus", HTTP_GET, handleGetAlarmStatus);
    server.on("/getSecurityStatus", HTTP_GET, handleGetSecurityStatus);
    server.on("/saveAlarmSettings", HTTP_POST, handleSaveAlarmSettings);
     server.on("/setDAC", HTTP_POST, handleSetDAC);
     server.on("/setMQTT", HTTP_POST, getMQTTSetings);
    server.begin();
}