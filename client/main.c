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
  if (argc < 5)
    {
      print_help (argv[0]);
      return -1;
    }

  char *server_ip = argv[1];
  int server_port = atoi (argv[2]);
  char *file = argv[3];
  char *folder = argv[4];

  uint32_t max_file_with_path_without_backup_suffix
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

  bool is_downloaded = false;
  if (network_client_init (server_ip, server_port)
      && client_init (&network_client_transport, folder) && client_connect ())
    {
      uint32_t server_file_size = get_file_size_from_server (file);
      /*check if there is old file exist and the file size is equal to the one in the server.
        Note: need to use hash to check whether they are the same files.Currently, just check
        the file size for demostration purpose.
      */

      char file_with_path[MAX_FILE_WITH_PATH];
      char backup_file_with_path[MAX_FILE_WITH_PATH];
      concatenate_strings(file_with_path, folder, file, MAX_FILE_WITH_PATH);
      FILE *fp = fopen (file_with_path, "r");
      uint32_t size = 0;
      FILE * backup_fp = NULL;
      if (fp != NULL)
        {
          size = get_file_size (fp);
        }

      if (fp && (size == server_file_size))
        {
          log_info ("the old file has the same size as the one in the server");
          fclose (fp);
          client_disconnect ();
          client_deinit ();
          network_client_deinit ();
          return 0;
        }

      if (fp != NULL)
        {
          /* Back up current old file in case the download fails*/
          concatenate_strings(backup_file_with_path, file_with_path, BACKUP_SUFFIX, MAX_FILE_WITH_PATH);
          backup_fp = fopen (backup_file_with_path, "w");
          if (backup_fp != NULL)
            {
              copy_file (fp, backup_fp);
              fclose (backup_fp);
            }
          else
            {
              log_error ("Fails to back up the old file");
              fclose (fp);
              client_disconnect ();
              client_deinit ();
              network_client_deinit ();
              return -1;
            }

      fclose (fp);
        }

      if (client_download (file, folder))
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
          if (backup_fp != NULL)
            {
              fp = fopen (file_with_path, "w");
              if (fp != NULL)
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

      client_disconnect ();
    }

      client_deinit ();
      network_client_deinit ();
      return is_downloaded ? 0 : -1;
}