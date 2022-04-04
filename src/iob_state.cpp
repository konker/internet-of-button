#include <FastLED.h>
#include "iob_state.h"

void iob_state_init(iob_state *state) {
    state->builtin_button_state = HIGH;
    state->last_builtin_button_reading = LOW;
    state->last_builtin_button_debounce_time = 0;
    state->is_builtin_button_new_state = false;

    state->pwr_button_down = false;
    state->pwr_button_down_start_time = 0;
    state->pwr_button_down_duration_ms = 0;
    state->rst_button_down = false;
    state->rst_button_down_start_time = 0;
    state->rst_button_down_duration_ms = 0;

    state->is_udp_listening = false;
    state->is_wifi_connected = false;
    state->wifi_ip_address = (new IPAddress())->fromString("0.0.0.0");

    state->settings_wifi_ssid = nullptr;
    state->settings_wifi_psk = nullptr;
    state->settings_udp_auth_key = nullptr;
    state->settings_udp_listen_port = 0;

    FastLED.addLeds<WS2812, RGBLED_BUILTIN, GRB>(state->leds, NUM_LEDS);
    state->leds[0] = CRGB::Black;
    FastLED.show();
}
