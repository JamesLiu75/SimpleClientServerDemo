#ifndef __server_h__
#define __server_h__

#include "transport.h"

bool server_init(server_transport_t *transport, uint32_t max_connection_num, char* fold);
bool server_run ();
void server_exit ();

#endif