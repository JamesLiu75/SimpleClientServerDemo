/*! \file network.h 
    \brief A file to implement client/server operations

    It implements the client/server operations defined 
    in \file transport.h based on TCP stack.
*/

#ifndef __network_h__
#define __network_h__

#include <stdbool.h>
#include "../../transport.h"

/*! \var  extern client_transport_t network_client_transport
    \brief client transport operation based on network.
*/
extern client_transport_t network_client_transport;

/*! \var extern server_transport_t network_server_transport
    \brief client transport operation based on network.
*/
extern server_transport_t network_server_transport;

/*! \fn bool network_client_init(char* server, uint32_t port)
    \brief initialize a network client

    \param server server ip string.
    \param port   server port.
    \return true if the initialization is done successfully.
*/
bool network_client_init(char* server, uint32_t port);

/*! \fn network_client_connect (void)
    \brief A function to connect from a network client to a server.

    \return true if the client connects to the specific server without any error.
*/
bool network_client_connect(void);

/*! \def bool (*client_disconnect_t) (void)
    \brief A function for a network client to disconnect

    \return true if the client disconnects the server without any error.
*/
bool network_client_disconnect(void);

/*! \fn int network_client_send (char *buffer, uint32_t buffer_len)
    \brief A function for a network client to send data.

    \param buffer The buffer where data need to be sent out.
    \param buffer_len The length of the data buffer.
    \return The size of data have been sent.
*/
int network_client_send(char* buffer, uint32_t buffer_len);

/*! \fn int network_client_receive (char *buffer, uint32_t buffer_len)
    \brief A function for a network client to receive data

    \param buffer The buffer where to store received data.
    \param buffer_len The length of the data buffer.
    \return The size of data have been received.
*/
int network_client_receive(char* buffer, uint32_t buffer_len);

/*! \fn void network_client_deinit()
    \brief A function for a network client to disconnect.

    \return nothing.
*/
void network_client_deinit();

/*! \fn bool network_server_init(uint32_t port)
    \brief initialize a network server

    \param port  port to listen.
    \return true if the initialization is done successfully.
*/
bool network_server_init(uint32_t port);

/*! \fn bool network_server_listen (on_connect_t callback,uint32_t max_connection_num)
    \brief A function for a network server to listen on the connections
           from network clients 
    
    \param callback The callback to be called when a client connects to the server.
    \param max_connection_num The maximum connection number to the server.

    \return false if it fails to listen, otherwise it will be blocked and waiting
            for clients to connect.
*/
bool network_server_listen(on_connect_t callback, uint32_t max_connection_num);

/*! \def int network_server_send(int client_id, char *buffer, uint32_t buffer_len)
    \brief A function for a network server to send data to the connected client.
    
    \param client_id The id of the connected client.
    \param buffer    The buffer of the data to be sent to a client.
    \param buffer_len The buffer length.
    \return the size of data have been sent out.
*/
int network_server_send(int client_socket,char* buffer, uint32_t buffer_len);

/*! \def int network_server_receive(int client_id, char *buffer, uint32_t buffer_len)
    \brief A function for a network server to receive data from the connected client.
    
    \param client_id The id of the connected client.
    \param buffer    The buffer to store the data which have been received from the client.
    \param buffer_len The buffer length.
    \return the size of data have been receieved.
*/
int network_server_receive(int client_socket,char* buffer, uint32_t buffer_len); 

/*! \def void network_server_exit(void)
    \brief A function for a network server to exit listening.
    
    \return nothing.
*/
void network_server_exit(void);

#endif