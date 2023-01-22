#ifndef __protocol_h__
#define __protocol_h__

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint32_t message_id;
  uint32_t message_length;
} __attribute__ ((__packed__)) message_header_t;

typedef struct
{
  uint32_t file_size;
} file_size_resp_payload_t;

typedef enum
{
  REQUEST_FILE_SIZE = 0x1,
  RESP_FILE_SIZE_REQUEST = 0x8001,

  REQUEST_TRANSFER_FILE = 0x2,
  TRANSFER_DATA = 0x8002,

} message_id_t;

bool is_valid_message_id (message_id_t message_id);
#endif