#include "utils.h"
#include "log.h"
#include <string.h>

void
copy_file (FILE *src, FILE *dst)
{
  if((src != NULL) && (dst != NULL)){
    char c = fgetc (src);
    while (c != EOF)
      {
        fputc (c, dst);
        c = fgetc (src);
      }
  }else {
    log_error("invalid file description");
  }
}

uint32_t
get_file_size (FILE *fp)
{ 
  if(fp == NULL){
    log_error("invalid file description");
    return 0;
  }

  int prev = ftell (fp);
  fseek (fp, 0L, SEEK_END);
  int file_size = ftell (fp);
  fseek (fp, prev, SEEK_SET);
  return file_size;
}

bool concatenate_strings(char* dst, char* src1, char* src2, uint32_t max_dst_buffer){
    if((dst == NULL) || (src1 == NULL) || (src2 == NULL)){
      log_error("invalid file description");
      return false;
    }

    memset(dst, 0, max_dst_buffer);
    int src1_len = strnlen(src1, max_dst_buffer-1);
    memcpy(dst, src1, src1_len);
    memcpy(dst+src1_len, src2, strnlen(src2, max_dst_buffer-src1_len-1));

    return true;
}