#include <stdlib.h>
#include <stdio.h>
#include "server.h"
#include "network.h"
#include "transport.h"

#define MAX_CONNECTION_NUM 10

void
print_help (char *program)
{
  printf (
      "Please use the format: %s <server port> <folder> [max_connection_num]",program);
}

int
main (int argc, char **argv)
{
  // TODO : use the short/long arguments
  if (argc < 4)
    {
      print_help (argv[0]);
      return -1;
    }

  int server_port = atoi (argv[1]);
  char *folder = argv[2];

  int max_connection_num = MAX_CONNECTION_NUM;
  if (argc >= 4)
    {
      max_connection_num = atoi (argv[3]);
    }

  if (max_connection_num <= 0)
    {
      print_help (argv[0]);
      return -1;
    }

  if (network_server_init (server_port)
      && server_init (&network_server_transport, max_connection_num, folder)
      && server_run ())
    {
      server_exit ();
      return 0;
    }

  server_exit ();
  return -1;
}