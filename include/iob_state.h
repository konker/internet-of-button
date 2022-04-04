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
    int builtin_button_last_reading;
    unsigned long builtin_button_last_debounce_time;
    bool builtin_button_is_new_state;

    bool ping;
    unsigned long ping_start_time;
    unsigned long ping_duration_ms;

    bool pwr_button_down;
    unsigned long pwr_button_down_start_time;
    unsigned long pwr_button_down_duration_ms;

    bool rst_button_down;
    unsigned long rst_button_down_start_time;
    unsigned long rst_button_down_duration_ms;

    bool is_udp_listening;

    // Wifi
    bool wifi_is_connected;
    unsigned int wifi_num_connect_failures;
    IPAddress wifi_ip_address;

    // Settings
    char *settings_wifi_ssid;
    char *settings_wifi_psk;
    char *settings_udp_auth_key;
    unsigned int settings_udp_listen_port;
} IOBState;


void iob_state_init(iob_state *state);

#endif // INTERNET_OF_BUTTON_V0_4_0_IOB_STATE_H