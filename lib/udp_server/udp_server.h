#include <AsyncUDP.h>

static AsyncUDP udp_server_socket;

#ifndef INTERNET_OF_BUTTON_V0_4_0_UDP_SERVER_H
#define INTERNET_OF_BUTTON_V0_4_0_UDP_SERVER_H

typedef void (*udp_server_callback_t)(AsyncUDPPacket);

void udp_server_start_listener(unsigned int port, udp_server_callback_t callback);

void udp_server_stop_listener();

#endif //INTERNET_OF_BUTTON_V0_4_0_UDP_SERVER_H
