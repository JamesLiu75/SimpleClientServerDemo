#include "protocol.h"
#include <stdbool.h>

bool
is_valid_message_id (message_id_t message_id)
{
  switch (message_id)
    {
    case REQUEST_FILE_SIZE:
    case RESP_FILE_SIZE_REQUEST:
    case REQUEST_TRANSFER_FILE:
    case TRANSFER_DATA:
      return true;
    }

  return false;
}