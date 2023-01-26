/*! \file log.h 
    \brief A file for log messages for different level 

*/

#ifndef __log_h__
#define __log_h__

#include <stdio.h>

/*! \fn void log_error (const char *message, ...)
    \brief log error message

    \param message Error message.
    \param ... More params.
    \return nothing. 
*/
void log_error (const char *message, ...);

/*! \fn void log_warn (const char *message, ...)
    \brief log warning message

    \param message Warning message.
    \param ... More params.
    \return nothing. 
*/
void log_warn (const char *message, ...);

/*! \fn void log_info (const char *message, ...)
    \brief log info message

    \param message Inform message.
    \param ... More params.
    \return nothing. 
*/
void log_info (const char *message, ...);

/*! \fn void log_debug (const char *message, ...)
    \brief log debug message

    \param message Debug message.
    \param ... More params.
    \return nothing. 
*/
void log_debug (const char *message, ...);

#endif