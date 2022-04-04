#include "wifi.h"

/*
* WiFi Events
0  SYSTEM_EVENT_WIFI_READY               < ESP32 WiFi ready
1  SYSTEM_EVENT_SCAN_DONE                < ESP32 finish scanning AP
2  SYSTEM_EVENT_STA_START                < ESP32 station start
3  SYSTEM_EVENT_STA_STOP                 < ESP32 station stop
4  SYSTEM_EVENT_STA_CONNECTED            < ESP32 station connected to AP
5  SYSTEM_EVENT_STA_DISCONNECTED         < ESP32 station disconnected from AP
6  SYSTEM_EVENT_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
7  SYSTEM_EVENT_STA_GOT_IP               < ESP32 station got IP from connected AP
8  SYSTEM_EVENT_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
9  SYSTEM_EVENT_STA_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
10 SYSTEM_EVENT_STA_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
11 SYSTEM_EVENT_STA_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
12 SYSTEM_EVENT_STA_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
13 SYSTEM_EVENT_AP_START                 < ESP32 soft-AP start
14 SYSTEM_EVENT_AP_STOP                  < ESP32 soft-AP stop
15 SYSTEM_EVENT_AP_STACONNECTED          < a station connected to ESP32 soft-AP
16 SYSTEM_EVENT_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
17 SYSTEM_EVENT_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
18 SYSTEM_EVENT_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
19 SYSTEM_EVENT_GOT_IP6                  < ESP32 station or ap or ethernet interface v6IP addr is preferred
20 SYSTEM_EVENT_ETH_START                < ESP32 ethernet start
21 SYSTEM_EVENT_ETH_STOP                 < ESP32 ethernet stop
22 SYSTEM_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
23 SYSTEM_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
24 SYSTEM_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
25 SYSTEM_EVENT_MAX
*/


WiFiEventFuncCb wifi_event_handler(wifi_callback_t callback) {
    return [callback](WiFiEvent_t event, system_event_info_t _) {
        Serial.printf("[wifi] event: %d\n", event);

        switch (event) {
            case SYSTEM_EVENT_WIFI_READY:
            case SYSTEM_EVENT_SCAN_DONE:
            case SYSTEM_EVENT_STA_START:
            case SYSTEM_EVENT_STA_STOP:
            case SYSTEM_EVENT_STA_CONNECTED:
            case SYSTEM_EVENT_STA_DISCONNECTED:
            case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            case SYSTEM_EVENT_STA_GOT_IP:
            case SYSTEM_EVENT_STA_LOST_IP:
            case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            case SYSTEM_EVENT_STA_WPS_ER_PIN:
            case SYSTEM_EVENT_AP_START:
            case SYSTEM_EVENT_AP_STOP:
            case SYSTEM_EVENT_AP_STACONNECTED:
            case SYSTEM_EVENT_AP_STADISCONNECTED:
            case SYSTEM_EVENT_AP_STAIPASSIGNED:
            case SYSTEM_EVENT_AP_PROBEREQRECVED:
            case SYSTEM_EVENT_GOT_IP6:
            case SYSTEM_EVENT_ETH_START:
            case SYSTEM_EVENT_ETH_STOP:
            case SYSTEM_EVENT_ETH_CONNECTED:
            case SYSTEM_EVENT_ETH_DISCONNECTED:
            case SYSTEM_EVENT_ETH_GOT_IP:
                callback(event);
            default:
                break;
        }
    };
}

void wifi_sta_got_ip_event_handler(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.printf("[wifi] Got IP address: %s\n", IPAddress(info.got_ip.ip_info.ip.addr).toString().c_str());
}

/*
    WIFI_REASON_UNSPECIFIED              = 1,
    WIFI_REASON_AUTH_EXPIRE              = 2,
    WIFI_REASON_AUTH_LEAVE               = 3,
    WIFI_REASON_ASSOC_EXPIRE             = 4,
    WIFI_REASON_ASSOC_TOOMANY            = 5,
    WIFI_REASON_NOT_AUTHED               = 6,
    WIFI_REASON_NOT_ASSOCED              = 7,
    WIFI_REASON_ASSOC_LEAVE              = 8,
    WIFI_REASON_ASSOC_NOT_AUTHED         = 9,
    WIFI_REASON_DISASSOC_PWRCAP_BAD      = 10,
    WIFI_REASON_DISASSOC_SUPCHAN_BAD     = 11,
    WIFI_REASON_IE_INVALID               = 13,
    WIFI_REASON_MIC_FAILURE              = 14,
    WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,
    WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
    WIFI_REASON_IE_IN_4WAY_DIFFERS       = 17,
    WIFI_REASON_GROUP_CIPHER_INVALID     = 18,
    WIFI_REASON_PAIRWISE_CIPHER_INVALID  = 19,
    WIFI_REASON_AKMP_INVALID             = 20,
    WIFI_REASON_UNSUPP_RSN_IE_VERSION    = 21,
    WIFI_REASON_INVALID_RSN_IE_CAP       = 22,
    WIFI_REASON_802_1X_AUTH_FAILED       = 23,
    WIFI_REASON_CIPHER_SUITE_REJECTED    = 24,

    WIFI_REASON_BEACON_TIMEOUT           = 200,
    WIFI_REASON_NO_AP_FOUND              = 201,
    WIFI_REASON_AUTH_FAIL                = 202,
    WIFI_REASON_ASSOC_FAIL               = 203,
    WIFI_REASON_HANDSHAKE_TIMEOUT        = 204,
 */
void wifi_sta_disconnected_event_handler(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.printf("[wifi] WiFi lost connection. Reason: %u\n", info.disconnected.reason);
}

void wifi_init(wifi_callback_t callback) {
    WiFi.onEvent(wifi_event_handler(callback));
    WiFi.onEvent(wifi_sta_got_ip_event_handler, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(wifi_sta_disconnected_event_handler, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.disconnect(true);
}

