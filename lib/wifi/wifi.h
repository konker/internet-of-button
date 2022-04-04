#include <WiFi.h>

#ifndef INTERNET_OF_BUTTON_V0_4_0_WIFI_H
#define INTERNET_OF_BUTTON_V0_4_0_WIFI_H

typedef void (*wifi_callback_t)(WiFiEvent_t);

void wifi_init(wifi_callback_t callback);

#endif //INTERNET_OF_BUTTON_V0_4_0_WIFI_H
