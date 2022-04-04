#include <FastLED.h>
#include "iob_state.h"

void iob_state_init(iob_state *state) {
    state->builtin_button_state = HIGH;
    state->builtin_button_last_reading = LOW;
    state->builtin_button_last_debounce_time = 0;
    state->builtin_button_is_new_state = false;

    state->ping = false;
    state->ping_start_time = 0;
    state->ping_duration_ms = 0;

    state->pwr_button_down = false;
    state->pwr_button_down_start_time = 0;
    state->pwr_button_down_duration_ms = 0;

    state->rst_button_down = false;
    state->rst_button_down_start_time = 0;
    state->rst_button_down_duration_ms = 0;

    state->is_udp_listening = false;
    state->wifi_is_connected = false;
    state->wifi_ip_address = (new IPAddress())->fromString("0.0.0.0");

    state->settings_wifi_ssid = nullptr;
    state->settings_wifi_psk = nullptr;
    state->settings_udp_auth_key = nullptr;
    state->settings_udp_listen_port = 0;

    FastLED.addLeds<WS2812, RGBLED_BUILTIN, GRB>(state->leds, NUM_LEDS);
    state->leds[0] = CRGB::Black;
    FastLED.show();
}
