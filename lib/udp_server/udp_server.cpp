#include <Arduino.h>
#include "udp_server.h"


void udp_server_start_listener(unsigned int port, udp_server_callback_t callback) {
    if (!udp_server_socket.connected()) {
        udp_server_socket.listen(port);
        udp_server_socket.onPacket(callback);
        Serial.printf("[udp_server] Listening on port: %u\n", port);
    }
}

void udp_server_stop_listener() {
    Serial.println("[udp_server] Closing UDP listener...");
    udp_server_socket.close();
}
