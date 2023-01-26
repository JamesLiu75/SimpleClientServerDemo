/*! \file utils.h 
    \brief A file with utilities 

    This is the file to hold help functions
*/

#ifndef __utils_h__
#define __utils_h__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/*! \def MAX_FILE_WITH_PATH
    \brief A macro to define the maximum string for the file with path

     Note:4096bytes for linux, 260 bytes for windows
*/
#define MAX_FILE_WITH_PATH 260

/*! \enum error_code_t
    \brief Define error codes for different errors
*/
typedef enum
{
  NO_ERROR = 0, /**< emum value NO_ERROR. */
  GENERAL_ERROR = -1, /**< emum value GENERAL_ERROR. */
  MEMORY_ERROR = -2, /**< emum value MEMORY_ERROR. */
  TRANSPORT_ERROR = -3, /**< emum value TRANSPORT_ERROR. */
  NULL_POINTER = -4, /**< emum value NULL_POINTER. */
  IO_ERROR = -5, /**< emum value IO_ERROR. */
  SUPPORT_ERROR = -6 /**< emum value SUPPORT_ERROR. */
} error_code_t;

/*! \fn void copy_file (FILE *src, FILE *dst)
    \brief copy a file

    \param src The source file pointer.
    \param dst The destinate file pointer. 
*/
void copy_file (FILE *src, FILE *dst);

/*! \fn uint32_t get_file_size (FILE *fp)
    \brief get the size of a file

    \param fp The file pointer.
    \return the size of the file.
*/
uint32_t get_file_size (FILE *fp);


/*! \fn bool concatenate_strings(char* dst, char* src1, char* src2, uint32_t max_dst_buffer)
    \brief concatenate two strings and save it into another string.

    \param dst The destination string to store the cancatenated string
    \param src1 The first source string.
    \param src2 The second source string.
    \param max_dst_buffer The maximum size of the destination string.

    \return true if operation is done without any errors, otherwise return false.
*/
bool concatenate_strings(char* dst, char* src1, char* src2, uint32_t max_dst_buffer);
#endif