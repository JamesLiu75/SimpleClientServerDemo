#ifndef __network_h__
#define __network_h__

#include <stdbool.h>
#include "../../transport.h"

extern client_transport_t network_client_transport;
extern server_transport_t network_server_transport;

bool network_client_init(char* server, uint32_t port);
bool network_client_connect();
bool network_client_disconnect();
int network_client_send(char* buffer, uint32_t buffer_len);
int network_client_receive(char* buffer, uint32_t buffer_len);
void network_client_deinit();

bool network_server_init(uint32_t port);
bool network_server_listen(on_connect_t callback, uint32_t max_connection_num);
int network_server_send(int client_socket,char* buffer, uint32_t buffer_len);
int network_server_receive(int client_socket,char* buffer, uint32_t buffer_len); 
void network_server_exit(void);

#endif