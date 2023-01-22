#ifndef __utils_h__
#define __utils_h__

#include <stdint.h>
#include <stdio.h>

/*4096bytes for linux, 260 bytes for windows*/
#define MAX_FILE_WITH_PATH 260

typedef enum
{
  NO_ERROR = 0,
  GENERAL_ERROR = -1,
  MEMORY_ERROR = -2,
  TRANSPORT_ERROR = -3,
  NULL_POINTER = -4,
  IO_ERROR = -5,
  SUPPORT_ERROR = -6
} error_code_t;

void copy_file (FILE *src, FILE *dst);

uint32_t get_file_size (FILE *fp);
#endif