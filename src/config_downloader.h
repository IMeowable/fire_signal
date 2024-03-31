#include "config.h"
#include "configInit.h"

#define MAX_LOGS_IN_FILES 100

void printConfigAfterInitialization();
String serializeConfig(const Config& config);
void writeConfigToFile(const String& data, const char* filename);
String serializeMqttSettings(const MqttSettings& mqttSettings);

