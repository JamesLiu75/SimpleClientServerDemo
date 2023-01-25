/*! \file server.h 
    \brief A file to provide funcationalities to the server

*/

#ifndef __server_h__
#define __server_h__

#include "transport.h"

/*! \fn bool server_init(server_transport_t *transport, uint32_t max_connection_num, char* folder)
    \brief initialize a server

    \param transport Transport operation set pointer.
    \param max_connection_num maximum connection number for the server.
    \param folder File folder where the server looks for the request file from clients.
    \return true if initialization is done without any error.
*/
bool server_init(server_transport_t *transport, uint32_t max_connection_num, char* folder);

/*! \fn bool server_run ()
    \brief run the server

    \return false if there is any error happening. Otherwise it will be blocked
            and waiting for clients to connect. 
*/
bool server_run ();

/*! \fn void server_exit ()
    \brief server exits

    \return nothing 
*/
void server_exit ();

#endif