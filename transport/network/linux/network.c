#include "network.h"
#include <log.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_SERVER_IPV4_ADDRESS_LEN 20
#define SERVER_MAX_CONNECTION_NUM 20

client_transport_t network_client_transport
    = { .con = network_client_connect,
        .send = network_client_send,
        .recv = network_client_receive,
        .discon = network_client_disconnect };

server_transport_t network_server_transport
    = { .listen = network_server_listen,
        .send = network_server_send,
        .recv = network_server_receive,
        .exit = network_server_exit };

/* local variables for server*/
static int _socket_descriptor;
static struct sockaddr_in connection_address;
static uint32_t _server_port;
static uint32_t _max_connection_num;
static bool _is_running = false;
static int accepted_connects[SERVER_MAX_CONNECTION_NUM];

/* local variables for client*/
static char _client_connect_address[MAX_SERVER_IPV4_ADDRESS_LEN];
static int _client_connect_port = 0;
static int _client_socket_descriptor = -1;

static uint32_t
find_empty_for_connection ()
{
  int i = 0;
  for (; i < SERVER_MAX_CONNECTION_NUM; i++)
    {
      if (accepted_connects[i] <= 0)
        {
          break;
        }
    }

  return i;
}

static void
close_all_accepted_connections ()
{
  for (int i = 0; i < SERVER_MAX_CONNECTION_NUM; i++)
    {
      if (accepted_connects[i] > 0)
        {
          close (accepted_connects[i]);
        }
    }
}

bool
network_server_init (uint32_t port)
{
  _server_port = port;
  int length_of_address;
  int option_value = 1;
  _socket_descriptor = socket (AF_INET, SOCK_STREAM, 0);
  if (_socket_descriptor < 0)
    {
      log_error("Socket creation failed");
      return false;
    }
  int status = setsockopt (_socket_descriptor, SOL_SOCKET, SO_REUSEADDR,
                           &option_value, sizeof (option_value));
  if (status < 0)
    {
      log_error ("Couldn't set options");
      return false;
    }

  return true;
}

bool
network_server_listen (on_connect_t callback, uint32_t max_connection_num)
{
  _max_connection_num = max_connection_num;
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons (_server_port);
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_zero[8] = '\0';
  int status = bind (_socket_descriptor, (struct sockaddr *)&server_address,
                     sizeof (struct sockaddr));
  if (status < 0)
    {
      log_error ("Couldn't bind socket");
      return false;
    }

  status = listen (_socket_descriptor, _max_connection_num);
  if (status < 0)
    {
      log_error ("Couldn't listen for connections");
      return false;
    }

  socklen_t length_of_address = sizeof (connection_address);
  _is_running = true;
  while (_is_running)
    {
      int client_socket
          = accept (_socket_descriptor, (struct sockaddr *)&connection_address,
                    &length_of_address);
      if (client_socket < 0)
        {
          log_error ("Couldn't establish connection with client");
        }

      uint32_t index = find_empty_for_connection ();
      if (index < SERVER_MAX_CONNECTION_NUM)
        {
          log_error ("No place available for the new connection");
          return false;
        }

      accepted_connects[index] = client_socket;
      /* call callback and provide the client socket*/
      callback (index);
    }
}

int
network_server_send (int client_id, char *buffer, uint32_t buffer_len)
{
  return send (accepted_connects[client_id], buffer, buffer_len, 0);
}

int
network_server_receive (int client_id, char *buffer, uint32_t buffer_len)
{
  return read (accepted_connects[client_id], buffer, buffer_len);
}

void
network_server_exit (void)
{
  _is_running = false;
  close (_socket_descriptor);
  close_all_accepted_connections ();
}

bool
network_client_init (char *server_ip, uint32_t port)
{
  strncpy (_client_connect_address, server_ip, MAX_SERVER_IPV4_ADDRESS_LEN);
  _client_connect_port = port;
  _client_socket_descriptor = socket (AF_INET, SOCK_STREAM, 0);
  if (_client_socket_descriptor < 0)
    {
      log_error ("Socket creation failed");
      return false;
    }
}
bool
network_client_connect ()
{
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons (_client_connect_port);
  server_address.sin_addr.s_addr = inet_addr (_client_connect_address);
  server_address.sin_zero[8] = '\0';
  int status
      = connect (_client_socket_descriptor, (struct sockaddr *)&server_address,
                 sizeof (server_address));
  if (status < 0)
    {
      log_error ("Couldn't connect with the server");
    }
}
bool
network_client_disconnect ()
{
  close (_client_socket_descriptor);
  return true;
}
int
network_client_send (char *buffer, uint32_t buffer_len)
{
  return send (_client_socket_descriptor, buffer, buffer_len, 0);
}

int
network_client_receive (char *buffer, uint32_t buffer_len)
{
  return read (_client_socket_descriptor, buffer, buffer_len);
}

void
network_client_deinit ()
{
}

uint32_t
host_to_transport_long (uint32_t host_long)
{
  return htonl (host_long);
}
uint16_t
host_to_transport_short (uint16_t host_short)
{
  return htons (host_short);
}

uint32_t
transport_to_host_long (uint32_t transport_long)
{
  return ntohl (transport_long);
}
uint16_t
transport_to_host_short (uint16_t transport_short)
{
  return ntohs (transport_short);
}