#include "utils.h"
#include <string.h>

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

bool concatenate_strings(char* dst, char* src1, char* src2, uint32_t max_dst_buffer){
    memset(dst, 0, max_dst_buffer);
    int src1_len = strnlen(src1, max_dst_buffer-1);
    memcpy(dst, src1, src1_len);
    memcpy(dst+src1_len, src2, strnlen(src2, max_dst_buffer-src1_len-1));

    return true;
}