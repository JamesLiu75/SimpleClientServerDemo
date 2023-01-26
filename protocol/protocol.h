/*! \file server.h 
    \brief A file to provide funcationalities to the server

*/

#ifndef __protocol_h__
#define __protocol_h__

#include <stdint.h>
#include <stdbool.h>

/*! \struct message_header_t
    \brief define the message header structure.

*/
typedef struct
{
  uint32_t message_id;/**< message id,  refer to \enum message_id_t*/
  uint32_t message_length; /**< message length*/
} __attribute__ ((__packed__)) message_header_t;

/*! \enum message_id_t
    \brief define the message id for each supported message
*/
typedef enum
{
  REQUEST_FILE_SIZE = 0x1, /**< message id for file size request message*/
  RESP_FILE_SIZE_REQUEST = 0x8001, /**< message id for file size reply message*/

  REQUEST_TRANSFER_FILE = 0x2,/**< message id for file transfer request message*/
  TRANSFER_DATA = 0x8002, /**< message id for file transfer message*/

} message_id_t;

/*! \fn bool is_valid_message_id (message_id_t message_id);
    \brief check if the id is a valid message id.

    \param message_id message id to be checked.
    \return true if the message id is valid,otherwise return false.
*/
bool is_valid_message_id (message_id_t message_id);
#endif