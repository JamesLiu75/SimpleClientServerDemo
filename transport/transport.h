/*! \file transport.h 
    \brief A file to define client/server operations


*/

#ifndef __transport_h__
#define __transport_h__

#include <stdbool.h>
#include <stdint.h>

/*! \def void (*on_connect_t) (int client_id)
    \brief A callback function

    It's a parameter to be passed into the listen function and 
    will be called when a client connects to the server
    \param[in] client_id The client id for the connected client
    \return nothing 
*/
typedef void (*on_connect_t) (int client_id);

/*! \def bool (*client_connect_t) (void)
    \brief A function pointer to connect from a client to a server

    \return true if the client connects to the specific server without any error.
*/
typedef bool (*client_connect_t) (void);

/*! \def int (*client_send_t) (char *buffer, uint32_t buffer_len)
    \brief A function pointer for a client to send data

    \param buffer The buffer where data need to be sent out.
    \param buffer_len The length of the data buffer.
    \return The size of data have been sent.
*/
typedef int (*client_send_t) (char *buffer, uint32_t buffer_len);

/*! \def int (*client_receive_t) (char *buffer, uint32_t buffer_len)
    \brief A function pointer for a client to receive data

    \param buffer The buffer where to store received data.
    \param buffer_len The length of the data buffer.
    \return The size of data have been received.
*/
typedef int (*client_receive_t) (char *buffer, uint32_t buffer_len);

/*! \def void (*client_disconnect_t) (void)
    \brief A function pointer for a client to disconnect

    \return true if the client disconnects the server without any error.
*/
typedef bool (*client_disconnect_t) (void);


/*! \def bool (*server_listen_t) (on_connect_t callback,uint32_t max_connection_num)
    \brief A function pointer for a server to listen on the connections
           from clients 
    
    \param callback The callback to be called when a client connects to the server.
    \param max_connection_num The maximum connection number to the server.

    \return false if it fails to listen, otherwise it will be blocked and waiting
            for clients to connect.
*/
typedef bool (*server_listen_t) (on_connect_t callback,uint32_t max_connection_num);

/*! \def int (*server_send_t) (int client_id, char *buffer, uint32_t buffer_len)
    \brief A function pointer for a server to send data to the connected client.
    
    \param client_id The id of the connected client.
    \param buffer    The buffer of the data to be sent to a client.
    \param buffer_len The buffer length.
    \return the size of data have been sent out.
*/
typedef int (*server_send_t) (int client_id, char *buffer,
                              uint32_t buffer_len);

/*! \def int (*server_receive_t) (int client_id, char *buffer, uint32_t buffer_len)
    \brief A function pointer for a server to receive data from the connected client.
    
    \param client_id The id of the connected client.
    \param buffer    The buffer to store the data which have been received from the client.
    \param buffer_len The buffer length.
    \return the size of data have been receieved.
*/
typedef int (*server_receive_t) (int client_id, char *buffer,
                                 uint32_t buffer_len);

/*! \def void (*server_exit_t) (void)
    \brief A function pointer for a server to exit listening.
    
    \return nothing.
*/                               
typedef void (*server_exit_t) (void);

/*! \def struct client_transport
    \brief define the operations for a client.

*/ 
typedef struct client_transport
{
  client_connect_t con; /**< connection function */
  client_send_t send; /**< send function */
  client_receive_t recv; /**< receive function */
  client_disconnect_t discon; /**< disconnection function */
} client_transport_t;

/*! \def struct server_transport
    \brief define the operations for a server.

*/
typedef struct server_transport
{
  server_listen_t listen; /**< listen function */
  server_send_t send; /**< send function */
  server_receive_t recv; /**< receive function */
  server_exit_t exit; /**< exit function */
} server_transport_t;

/*! \fn uint32_t host_to_transport_long (uint32_t host_long)
    \brief convert host long bytes order to transport long bytes order.

    \param host_long host long 
    \return transport long
*/
uint32_t host_to_transport_long (uint32_t host_long);

/*! \fn uint32_t host_to_transport_short (uint32_t host_short)
    \brief convert host short bytes order to transport short bytes order.

    \param host_short host short 
    \return transport short
*/
uint16_t host_to_transport_short (uint16_t host_long);

/*! \fn uint32_t transport_to_host_long (uint32_t transport_long)
    \brief convert transport long bytes order to host long bytes order.

    \param transport_long transport long 
    \return host long
*/
uint32_t transport_to_host_long (uint32_t transport_long);

/*! \fn uint32_t transport_to_host_short (uint32_t transport_short)
    \brief convert transport short bytes order to host short bytes order.

    \param transport_short transport short 
    \return host short
*/
uint16_t transport_to_host_short (uint16_t transport_short);

#endif