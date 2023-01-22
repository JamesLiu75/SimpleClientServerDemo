#include "log.h"
#include <stdio.h>
#include <stdarg.h>

void
log_format (const char *tag, const char *message, va_list args)
{
  printf ("[%s] ", tag);
  vprintf (message, args);
  printf ("\n");
}

void
log_error (const char *message, ...)
{
  va_list args;
  va_start (args, message);
  log_format ("error", message, args);
  va_end (args);
}

void
log_warn (const char *message, ...)
{
  va_list args;
  va_start (args, message);
  log_format ("warn", message, args);
  va_end (args);
}

void
log_info (const char *message, ...)
{
  va_list args;
  va_start (args, message);
  log_format ("info", message, args);
  va_end (args);
}

void
log_debug (const char *message, ...)
{
  va_list args;
  va_start (args, message);
  log_format ("debug", message, args);
  va_end (args);
}