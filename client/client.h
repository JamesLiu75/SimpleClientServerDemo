#ifndef __client_h__
#define __client_h__
#include <stdint.h>
#include "transport.h"
#include "utils.h"

bool client_init(client_transport_t *transport, char* folder);
bool client_connect(void);
uint32_t get_file_size_from_server(char filename[]);
bool client_download(char filename[], char* folder);
void client_disconnect();
void client_deinit();


#endif