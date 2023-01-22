#ifndef __transport_h__
#define __transport_h__

#include <stdbool.h>
#include <stdint.h>

#define MAX_DATA_LENGTH_IN_BYTE 1024

typedef void (*on_connect_t) (int client_id);

typedef bool (*client_connect_t) (void);
typedef int (*client_send_t) (char *buffer, uint32_t buffer_len);
typedef int (*client_receive_t) (char *buffer, uint32_t buffer_len);
typedef bool (*client_disconnect_t) (void);

typedef bool (*server_listen_t) (on_connect_t callback,uint32_t max_connection_num);
typedef int (*server_send_t) (int client_id, char *buffer,
                              uint32_t buffer_len);
typedef int (*server_receive_t) (int client_id, char *buffer,
                                 uint32_t buffer_len);
typedef void (*server_exit_t) (void);

typedef struct client_transport
{
  client_connect_t con;
  client_send_t send;
  client_receive_t recv;
  client_disconnect_t discon;
} client_transport_t;

typedef struct server_transport
{
  server_listen_t listen;
  server_send_t send;
  server_receive_t recv;
  server_exit_t exit;
} server_transport_t;

/*Convert the unsigned integer host_long from host byte order to transport byte
 * order */
uint32_t host_to_transport_long (uint32_t host_long);
uint16_t host_to_transport_short (uint16_t host_long);

uint32_t transport_to_host_long (uint32_t transport_long);
uint16_t transport_to_host_short (uint16_t transport_long);

#endif