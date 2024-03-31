#include "readFromHtml.h"


String readHtmlFile(String html_file_name) {
  
  
  File file = SPIFFS.open(html_file_name, "r");
    String content = "";

    // Если файл успешно открыт
    if (file) {
        // Читаем содержимое файла
        while (file.available()) {
            content += (char)file.read();
        }
        // Закрываем файл
        file.close();
    } else {
        Serial.println("Failed to read HTML file");
    }

    return content;
}