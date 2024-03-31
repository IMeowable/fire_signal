#include "telegramwork.h"
#include <HTTPClient.h>

void sendTelegramMessage(String message) {
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + myConfig.api_key + "/sendMessage?chat_id=" + myConfig.chat_id + "&text=" + message;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.println("HTTP response code: " + String(httpCode));
    String payload = http.getString();
    Serial.println("Response: " + payload);
  } else {
    Serial.println("HTTP request failed");
  }
  http.end();
}