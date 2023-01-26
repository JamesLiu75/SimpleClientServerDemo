/*! \file client.h 
    \brief A file to provide funcationalities to the client

*/

#ifndef __client_h__
#define __client_h__
#include <stdint.h>
#include "transport.h"
#include "utils.h"

/*! \fn bool client_init(client_transport_t *transport, char* folder)
    \brief initialize a client

    \param transport Transport operation set pointer.
    \param folder File folder where the downloaded file to store.
    \return true if initialization is done without any error.
*/
bool client_init(client_transport_t *transport, char* folder);

/*! \fn bool client_connect(void)
    \brief connect to specific server which is related to the provided \param transport.

    \return true if connecting to the server successfully.
*/
bool client_connect(void);

/*! \fn uint32_t get_file_size_from_server(char* filename)
    \brief get the size of specified file by \param filename from the server.

    \param filename the file name for size requested.
    \return the size of specified file. If the server can't find the
            file, 0 will be returned.
*/
uint32_t get_file_size_from_server(char* filename);

/*! \fn bool client_download(char* filename, char* folder)
    \brief download the specified file by \param filename to 
           the specific folder \param folder

    \param filename the name of the file to be downloaded.
    \param folder the folder where the donwloaded file will be stored.
    \return true if the file has been downloaded without any error, othewise
            return false.
*/
bool client_download(char* filename, char* folder);

/*! \fn void client_disconnect()
    \brief disconnect the client from the server.

    \return nothing.
*/
void client_disconnect();

/*! \fn void client_deinit()
    \brief de-initilize the client.

    \return nothing.
*/
void client_deinit();


#endif