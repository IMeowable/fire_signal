#ifndef SERVER_HANDLERS_H
#define SERVER_HANDLERS_H

#include "config.h"
#include "configInit.h"
#include "readFromHtml.h"
#include <ESPAsyncWebServer.h>

const int html_OK = 200;
const int json_buffer = 256;

void setupServer();
void handleRoot(AsyncWebServerRequest *request);
void handleWiFiHtml(AsyncWebServerRequest *request);
void handleConfirmSelection(AsyncWebServerRequest *request);
void handleSaveWifiSettings(AsyncWebServerRequest *request);
void handleAlarmingHtml(AsyncWebServerRequest *request);
void handleGetAnalogData(AsyncWebServerRequest *request);
void handleGetDigitalData(AsyncWebServerRequest *request);
void handleToggleAlarm(AsyncWebServerRequest *request);
void handleGetAlarmStatus(AsyncWebServerRequest *request);
void handleGetSecurityStatus(AsyncWebServerRequest *request);
void handleSaveAlarmSettings(AsyncWebServerRequest *request);
void sendHtmlResponse(AsyncWebServerRequest *request, const String &htmlFilePath);
void setAnalogAndDigitalPins(int analogPin, int digitalPin);
void setWifiSettings(const String &ssid, const String &password);
void sendJsonResponse(AsyncWebServerRequest *request, const String &key, int value);
void sendJsonResponse(AsyncWebServerRequest *request, DynamicJsonDocument &jsonDoc);
void handleSetDAC(AsyncWebServerRequest *request);

#endif 
