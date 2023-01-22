#ifndef __client_h__
#define __client_h__
#include <stdint.h>
#include "transport.h"
#include "utils.h"

bool client_init(client_transport_t *transport, char* folder);
bool connect(void);
uint32_t get_file_size_from_server(char filename[]);
bool download(char filename[], char* folder);
void disconnect();
void client_deinit();


#endif