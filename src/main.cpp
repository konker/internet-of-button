#include <Arduino.h>
#include <cstring>
#include <WiFi.h>
#include <AsyncUDP.h>
#include "iob_state.h"
#include "settings.h"
#include "wifi.h"
#include "udp_server.h"

#define BUTTON_BUILTIN 27 // 27/D4
#define PIN_PWR_IN 0
#define PIN_PWR_OUT 14
#define PIN_RST_IN 25
#define PIN_RST_OUT 26

const unsigned long DEBOUNCE_DURATION_MS = 1000;
const unsigned long LED_OK_INTERVAL_MS = 8000;
const unsigned long LED_OK_DURATION_MS = 1000;
const char *UDP_SERVER_CMD_DELIMITER = ":";

const char *SETTINGS_NAME_WIFI_SSID = "settings_wifi_ssid";
const char *SETTINGS_NAME_WIFI_PSK = "settings_wifi_psk";
const char *SETTINGS_NAME_UDP_AUTH_KEY = "settings_udp_auth_key";
const char *SETTINGS_NAME_UDP_LISTEN_PORT = "settings_udp_listen_port";

const char *CMD_PING = "PING";
const char *RESP_PING = "PONG";
const char *CMD_POWER_BUTTON_LONG = "PWRL";
const char *CMD_POWER_BUTTON_SHORT = "PWR";
const char *CMD_RESET_BUTTON_SHORT = "RST";

const unsigned short IOB_PING_DURATION_MS = 1000;
const unsigned short IOB_BUTTON_DURATION_MS_SHORT = 300;
const unsigned short IOB_BUTTON_DURATION_MS_LONG = 5000;

// -------------------------------------------------------------------
iob_state state;

// -------------------------------------------------------------------
void udp_server_callback(AsyncUDPPacket packet) {
    char *packet_data = reinterpret_cast<char *>(packet.data());
    char *auth_key;
    char *cmd;

    // Try to split the data into <auth_key>:<command>
    // [XXX: should we copy the packet_data string, it is destroyed by strtok]
    auth_key = std::strtok(packet_data, UDP_SERVER_CMD_DELIMITER);
    if (auth_key != NULL) {
        cmd = std::strtok(NULL, UDP_SERVER_CMD_DELIMITER);
    } else {
        packet.printf("[internet-of-button] ERROR 401\n");
        return;
    }
    if (std::strcmp(state.settings_udp_auth_key, auth_key)) {
        packet.printf("[internet-of-button] ERROR 401\n");
        return;
    }
    if (!cmd) {
        packet.printf("[internet-of-button] ERROR 400\n");
        return;
    }

    if (!strcmp(CMD_PING, cmd)) {
        Serial.println("[internet-of-button] PING");
        state.ping_duration_ms = IOB_PING_DURATION_MS;
        state.ping_start_time = millis();
        state.ping = true;
        packet.printf("OK |%s|\n", RESP_PING);
    }
    if (!strcmp(CMD_POWER_BUTTON_LONG, cmd)) {
        Serial.println("[internet-of-button] PWR LONG");
        state.pwr_button_down_duration_ms = IOB_BUTTON_DURATION_MS_LONG;
        state.pwr_button_down_start_time = millis();
        state.pwr_button_down = true;
        packet.printf("OK |%s|\n", cmd);
    } else if (!strcmp(CMD_POWER_BUTTON_SHORT, cmd)) {
        Serial.println("[internet-of-button] PWR SHORT");
        state.pwr_button_down_duration_ms = IOB_BUTTON_DURATION_MS_SHORT;
        state.pwr_button_down_start_time = millis();
        state.pwr_button_down = true;
        packet.printf("OK |%s|\n", cmd);
    } else if (!strcmp(CMD_RESET_BUTTON_SHORT, cmd)) {
        Serial.println("[internet-of-button] RST SHORT");
        state.rst_button_down_duration_ms = IOB_BUTTON_DURATION_MS_SHORT;
        state.rst_button_down_start_time = millis();
        state.rst_button_down = true;
        packet.printf("[internet-of-button] OK |%s|\n", cmd);
    } else {
        packet.printf("[internet-of-button] ERROR 404 |%s|\n", cmd);
    }
    packet.flush();
}

// -------------------------------------------------------------------
void wifi_callback(WiFiEvent_t event) {
    switch (event) {
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.printf("[internet-of-button] WIFI connected\n");
            Serial.printf("[internet-of-button] RSSI: %d ...\n", WiFi.RSSI());
            udp_server_start_listener(state.settings_udp_listen_port, udp_server_callback);
            state.wifi_num_connect_failures = 0;
            state.wifi_is_connected = true;
            state.is_udp_listening = true;
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.printf("[internet-of-button] WIFI disconnected\n");
            udp_server_stop_listener();
            state.wifi_num_connect_failures++;
            state.wifi_is_connected = false;
            state.is_udp_listening = false;

            // Attempt to reconnect
            delay(1000 * state.wifi_num_connect_failures);
            WiFi.mode(WIFI_STA);
            WiFi.begin(state.settings_wifi_ssid, state.settings_wifi_psk);
            Serial.printf("[internet-of-button] Connecting to SSID: |%s| ...\n", state.settings_wifi_ssid);
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
        case SYSTEM_EVENT_GOT_IP6:
            Serial.printf("[internet-of-button] WIFI got IP address\n");
            state.wifi_ip_address = WiFi.localIP();
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            Serial.printf("[internet-of-button] WIFI lost IP address\n");
            state.wifi_ip_address = (new IPAddress())->fromString("0.0.0.0");
            break;
        default:
            break;
    }

}

void setup() {
    // -------------------------------------------------------------------
    pinMode(RGBLED_BUILTIN, OUTPUT);
    pinMode(BUTTON_BUILTIN, INPUT_PULLUP);

    pinMode(PIN_PWR_IN, INPUT_PULLUP);
    pinMode(PIN_PWR_OUT, OUTPUT);
    pinMode(PIN_RST_IN, INPUT_PULLUP);
    pinMode(PIN_RST_OUT, OUTPUT);

    digitalWrite(PIN_PWR_OUT, HIGH);
    digitalWrite(PIN_RST_OUT, HIGH);

    Serial.begin(9600);
    Serial.setDebugOutput(true);

    iob_state_init(&state);

    if (!settings_init()) {
        Serial.println("[internet-of-button] FATAL ERROR: could not init settings");
        return;
    }

    // -------------------------------------------------------------------
    settings_get(&state.settings_wifi_ssid, SETTINGS_NAME_WIFI_SSID);
    if (!std::strlen(state.settings_wifi_ssid)) {
        Serial.printf("[internet-of-button] FATAL ERROR: could not get setting: |%s|\n", SETTINGS_NAME_WIFI_SSID);
        return;
    }

    settings_get(&state.settings_wifi_psk, SETTINGS_NAME_WIFI_PSK);
    if (!std::strlen(state.settings_wifi_psk)) {
        Serial.printf("[internet-of-button] FATAL ERROR: could not get setting: %s\n", SETTINGS_NAME_WIFI_PSK);
        return;
    }

    settings_get(&state.settings_udp_auth_key, SETTINGS_NAME_UDP_AUTH_KEY);
    if (!std::strlen(state.settings_udp_auth_key)) {
        Serial.printf("[internet-of-button] FATAL ERROR: could not get setting: %s\n", SETTINGS_NAME_UDP_AUTH_KEY);
        return;
    }
    Serial.println(state.settings_udp_auth_key);

    settings_get_int(&state.settings_udp_listen_port, SETTINGS_NAME_UDP_LISTEN_PORT);
    if (!state.settings_udp_listen_port) {
        Serial.printf("[internet-of-button] FATAL ERROR: could not get setting: %s\n", SETTINGS_NAME_UDP_LISTEN_PORT);
        return;
    }

    // -------------------------------------------------------------------
    wifi_init(wifi_callback);
    WiFi.mode(WIFI_STA);
    WiFi.begin(state.settings_wifi_ssid, state.settings_wifi_psk);
    Serial.printf("[internet-of-button] RSSI: %d ...\n", WiFi.RSSI());
    Serial.printf("[internet-of-button] Connecting to SSID: |%s| ...\n", state.settings_wifi_ssid);
}


void loop() {
    // -------------------------------------------------------------------
    const int pwr_reading = digitalRead(PIN_PWR_IN);
    if (pwr_reading == LOW) {
        // Forward the physical power button press to the motherboard
        digitalWrite(PIN_PWR_OUT, LOW);
    } else if (state.pwr_button_down) {
        if (state.pwr_button_down_start_time + state.pwr_button_down_duration_ms > millis()) {
            // Simulate power button press to the motherboard
            Serial.println("[internet-of-button] PWR DOWN");
            digitalWrite(PIN_PWR_OUT, LOW);
        } else {
            Serial.println("[internet-of-button] PWR UP");
            state.pwr_button_down = false;
            digitalWrite(PIN_PWR_OUT, HIGH);
        }
    } else {
        digitalWrite(PIN_PWR_OUT, HIGH);
    }

    // -------------------------------------------------------------------
    const int rst_reading = digitalRead(PIN_RST_IN);
    if (rst_reading == LOW) {
        // Forward the physical reset button press to the motherboard
        digitalWrite(PIN_RST_OUT, LOW);
    } else if (state.rst_button_down) {
        if (state.rst_button_down_start_time + state.rst_button_down_duration_ms > millis()) {
            // Simulate reset button press to the motherboard
            Serial.println("[internet-of-button] RST DOWN");
            digitalWrite(PIN_RST_OUT, LOW);
        } else {
            Serial.println("[internet-of-button] RST UP");
            state.rst_button_down = false;
            digitalWrite(PIN_RST_OUT, HIGH);
        }
    } else {
        digitalWrite(PIN_RST_OUT, HIGH);
    }

    // -------------------------------------------------------------------
    int builtin_button_reading = digitalRead(BUTTON_BUILTIN);
    if (builtin_button_reading != state.builtin_button_last_reading) {
        state.builtin_button_last_debounce_time = millis();
        state.builtin_button_last_reading = builtin_button_reading;
    }
    if (builtin_button_reading == HIGH || (millis() - state.builtin_button_last_debounce_time) > DEBOUNCE_DURATION_MS) {
        if (builtin_button_reading != state.builtin_button_state) {
            state.builtin_button_state = builtin_button_reading;
            state.builtin_button_last_reading = builtin_button_reading;
            state.builtin_button_is_new_state = true;
        }
    }

    // Handle button press, if any
    if (state.builtin_button_state == LOW && state.builtin_button_is_new_state) {
        state.builtin_button_is_new_state = false;
        // [TODO? switch wifi on/off?]
        // [TODO? wake from deep sleep?]
    }

    // -------------------------------------------------------------------
    if (state.ping) {
        if (millis() > state.ping_start_time + state.ping_duration_ms) {
            state.ping = false;
            state.ping_start_time = 0;
        }
    }

    // -------------------------------------------------------------------
    // Handle LED display
    if (rst_reading == LOW || state.rst_button_down) {
        state.leds[0] = CRGB::DeepPink;
        FastLED.show();
    } else if (pwr_reading == LOW || state.pwr_button_down) {
        state.leds[0] = CRGB::DeepSkyBlue;
        FastLED.show();
    } else if (state.ping) {
        state.leds[0] = CRGB::LemonChiffon;
        FastLED.show();
    } else if (state.wifi_is_connected && millis() % LED_OK_INTERVAL_MS < LED_OK_DURATION_MS) {
        state.leds[0] = CRGB::Green;
        FastLED.show();
    } else if (!state.wifi_is_connected) {
        state.leds[0] = CRGB::Red;
        FastLED.show();
    } else {
        state.leds[0] = CRGB::Black;
        FastLED.show();
    }
}
