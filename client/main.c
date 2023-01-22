#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "log.h"
#include "network.h"
#include "utils.h"

#define BACKUP_SUFFIX ".org"

void
print_help (char *program)
{
  printf ("Please use the format: %s <server ip> <server port> <file> <dest "
          "folder>", program);
}

int
main (int argc, char **argv)
{
  // TODO : use the short/long arguments
  if (argc < 6)
    {
      print_help (argv[0]);
      return -1;
    }

  char *server_ip = argv[1];
  int server_port = atoi (argv[2]);
  char *file = argv[3];
  char *folder = argv[4];

  int max_file_with_path_without_backup_suffix
      = (MAX_FILE_WITH_PATH - sizeof (BACKUP_SUFFIX));
  if((strlen (file) + strlen (folder))
   > max_file_with_path_without_backup_suffix)
  {
    log_error ("file name with folder is larger than %d",
               max_file_with_path_without_backup_suffix);
    return -1;
  }

  if (server_port <= 0)
    {
      print_help (argv[0]);
      return -1;
    }

  if (network_client_init (server_ip, server_port)
      || client_init (&network_client_transport, folder) || connect ())
    {
      uint32_t server_file_size = get_file_size_from_server (file);
      /*check if there is old file exist and the file size if it's*/

      char file_with_path[MAX_FILE_WITH_PATH];
      char backup_file_with_path[MAX_FILE_WITH_PATH];
      strncpy (file_with_path, folder, MAX_FILE_WITH_PATH);
      strcat (file_with_path, file);
      FILE *fp = fopen (file_with_path, "r");
      uint32_t size = 0;
      FILE * backup_fp = NULL;
      if (fp != NULL)
        {
          size = get_file_size (fp);
        }

      if (fp && size == server_file_size)
        {
          log_info ("the old file has the same size as the one in the server");
          fclose (fp);
          return -1;
        }

      if (fp != NULL)
        {
          /* Back up current old file in case the download fails*/
          strncpy (backup_file_with_path, file_with_path, MAX_FILE_WITH_PATH);
          strcat (backup_file_with_path, ".org");
          backup_fp = fopen (backup_file_with_path, "w");
          if (backup_fp > 0)
            {
              copy_file (fp, backup_fp);
              fclose (backup_fp);
            }
          else
            {
              log_error ("Fails to back up the old file");
              fclose (fp);
              return -1;
            }
        }

      fclose (fp);
      bool is_downloaded = false;
      if (download (file, folder))
        {
          log_info ("File %s has been downloaded successfully", file);
          remove (backup_file_with_path);
          is_downloaded = true;
        }
      else
        {
          /*copy the backup file back*/
          bool result = false;
          backup_fp = fopen (backup_file_with_path, "r");
          if (backup_fp > 0)
            {
              fp = fopen (file_with_path, "w");
              if (fp > 0)
                {
                  copy_file (backup_fp, fp);
                  fclose (backup_fp);
                  fclose (fp);
                  remove (backup_file_with_path);
                  result = true;
                }
              else
                {
                  fclose (fp);
                }
            }

          if (!result)
            {
              log_error ("Fail to restore the old file, it's here: %s",
                         backup_file_with_path);
            }
        }

      disconnect ();
      client_deinit ();
      network_client_deinit ();

      return is_downloaded ? 0 : -1;
    }

  return -1;
}