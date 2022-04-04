#include <cstring>
#include <SPIFFS.h>
#include "settings.h"

bool settings_init() {
    if (!SPIFFS.begin(true)) {
        Serial.println("[internet-of-button] An Error has occurred while mounting SPIFFS");
        return false;
    }
    delay(500);
    return true;
}

void settings_get(char **target, const char *name) {
    char file_name[strlen(SETTINGS_PREFIX) + strlen(name) + strlen(SETTINGS_POSTFIX)];
    strcpy(file_name, SETTINGS_PREFIX);
    strcat(file_name, name);
    strcat(file_name, SETTINGS_POSTFIX);
    Serial.printf("[internet-of-button] Reading setting: |%s|\n", file_name);

    File file = SPIFFS.open(file_name);
    if (!file) {
        Serial.printf("[internet-of-button] Failed to open file for reading: |%s|\n", file_name);
        return;
    }

    String file_contents = file.readString();
    file.close();

    if (*target) {
        free(*target);
    }
    *target = static_cast<char *>(malloc(sizeof(char) * file_contents.length()));
    std::strcpy(*target, file_contents.c_str());
}

void settings_get_int(unsigned int *target, const char *name) {
    char file_name[strlen(SETTINGS_PREFIX) + strlen(name) + strlen(SETTINGS_POSTFIX)];
    strcpy(file_name, SETTINGS_PREFIX);
    strcat(file_name, name);
    strcat(file_name, SETTINGS_POSTFIX);
    Serial.printf("[internet-of-button] Rading int setting: |%s|\n", file_name);

    File file = SPIFFS.open(file_name);
    if (!file) {
        Serial.printf("[internet-of-button] Failed to open file for reading: |%s|\n", file_name);
        return;
    }

    String file_contents = file.readString();
    file.close();

    *target = std::atoi(file_contents.c_str());
}
