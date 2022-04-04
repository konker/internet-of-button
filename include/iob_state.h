#include <FastLED.h>

#ifndef INTERNET_OF_BUTTON_V0_4_0_IOB_STATE_H
#define INTERNET_OF_BUTTON_V0_4_0_IOB_STATE_H

#define RGBLED_BUILTIN 5  // 5/D8
#define NUM_LEDS 1

typedef struct iob_state {
    // UI
    CRGB leds[NUM_LEDS];

    // Runtime state
    int builtin_button_state;
    int last_builtin_button_reading;
    unsigned long last_builtin_button_debounce_time;
    bool is_builtin_button_new_state;

    bool pwr_button_down;
    unsigned long pwr_button_down_start_time;
    unsigned long pwr_button_down_duration_ms;

    bool rst_button_down;
    unsigned long rst_button_down_start_time;
    unsigned long rst_button_down_duration_ms;

    bool is_udp_listening;
    unsigned int num_wifi_connect_failures;

    // Wifi
    bool is_wifi_connected;
    IPAddress wifi_ip_address;

    // Settings
    char *settings_wifi_ssid;
    char *settings_wifi_psk;
    char *settings_udp_auth_key;
    unsigned int settings_udp_listen_port;
} IOBState;


void iob_state_init(iob_state *state);

#endif // INTERNET_OF_BUTTON_V0_4_0_IOB_STATE_H