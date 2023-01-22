#include "utils.h"

void
copy_file (FILE *src, FILE *dst)
{
  char c = fgetc (src);
  while (c != EOF)
    {
      fputc (c, dst);
      c = fgetc (src);
    }
}

uint32_t
get_file_size (FILE *fp)
{
  int prev = ftell (fp);
  fseek (fp, 0L, SEEK_END);
  int file_size = ftell (fp);
  fseek (fp, prev, SEEK_SET);
  return file_size;
}