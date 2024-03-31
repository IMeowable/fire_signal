#include "temperatureHandle.h"

const int threshold = 15;
const int alarmThreshold = 2000;
const int numValuesToTrack = 2; 

std::vector<int> lastAnalogValues; 
void readFromAdc()
{
   currentAnalogValue =  adc1_get_raw(ADC1_CHANNEL_5);

}
void temperatureHandle() 

  {
    readFromAdc();
    Serial.println(currentAnalogValue);
   
    lastAnalogValues.push_back((int)currentAnalogValue);
    while (lastAnalogValues.size() > numValuesToTrack) {
        lastAnalogValues.erase(lastAnalogValues.begin());
    }
    if (abs(currentAnalogValue - lastAnalogValue) > threshold && currentAnalogValue) {
        Serial.print("Аналоговое значение с ЦАП: ");
      
        myConfig.last_adc_value = currentAnalogValue;
         
          Serial.println(myConfig.last_adc_value);
     }
    int countHighValues = std::count_if(lastAnalogValues.begin(), lastAnalogValues.end(),
                                       [](int value) { return value >= alarmThreshold; });
    if (countHighValues >= numValuesToTrack) {
         myConfig.alarm = true;
     } else {
         myConfig.alarm = false;
     }
     lastAnalogValue = currentAnalogValue;
}