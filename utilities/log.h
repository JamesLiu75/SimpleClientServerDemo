#ifndef __log_h__
#define __log_h__

#include <stdio.h>

void log_error (const char *message, ...);
void log_warn (const char *message, ...);
void log_info (const char *message, ...);
void log_debug (const char *message, ...);

#endif