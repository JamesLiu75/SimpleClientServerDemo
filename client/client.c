#include "client.h"
#include "protocol.h"
#include "transport.h"
#include "log.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
  client_transport_t *tran;
  char folder[MAX_FILE_WITH_PATH];
} client_t;

static bool is_inited = false;
static client_t client;

static bool
save_to_file (FILE *fp, char *buffer, int bytes_to_write)
{
  log_debug("%s",__func__);

  int write_bytes = fwrite (buffer, 1, bytes_to_write, fp);
  while (write_bytes < bytes_to_write)
    {
      int bytes = fwrite (buffer+write_bytes, 1, bytes_to_write - write_bytes,fp);
      write_bytes += bytes;
    }

  return true;
}

// Internal function, relies on the caller to pass valid parameters
static bool
download_file_data_block_and_save_to_file (FILE *fp, int block_size)
{
  char *buffer = malloc (block_size);
  if (buffer == NULL)
    {
      log_error ("Fail to allocate memory");
      return false;
    }

  int received_bytes = client.tran->recv (buffer, block_size);
  if (received_bytes == 0)
    {
      log_error ("socket is broken");
      free (buffer);
      return false;
    }

  if (!save_to_file (fp, buffer, received_bytes))
    {
      log_error ("Fail to save data into file");
      free (buffer);
      return false;
    }

  while (received_bytes < block_size)
    {
      int bytes = client.tran->recv (buffer, block_size - received_bytes);
      if((bytes == 0) || !save_to_file(fp, buffer, bytes)){
        log_error ("Socket is broken or fail to save data into file");
        free (buffer);
        return false;
          }
          received_bytes += bytes;
    }

  free (buffer);
  return true;
}

static uint32_t
get_transfer_message_length ()
{
  int message_header_size = sizeof (message_header_t);
  char *buffer = malloc (message_header_size);
  if (buffer == NULL)
    {
      log_error ("Fail to allocate memory for message header");
      return 0;
    }

  int received_bytes = client.tran->recv (buffer, message_header_size);
  if (received_bytes == 0)
    {
      log_error ("socket is broken");
      free (buffer);
      return false;
    }

  while (received_bytes < message_header_size)
    {
      int bytes = client.tran->recv (buffer, message_header_size - received_bytes);
      if (bytes == 0)
        {
          log_error ("Socket is broken");
          free (buffer);
          return false;
        }
      received_bytes += bytes;
    }

  uint32_t message_id;
  memcpy ((char *)&message_id, buffer, sizeof (uint32_t));
  /*convert to host byte order*/
  message_id = transport_to_host_long (message_id);

  uint32_t message_length;
  memcpy ((char *)&message_length, buffer + sizeof (message_id),
          sizeof (message_length));
  /*convert to host byte order*/
  message_length = transport_to_host_long (message_length);
  log_info ("Got message header, message id[0x%x], message length[%d]",
            message_id, message_length);
  
  free (buffer);
  if (message_id == TRANSFER_DATA)
    {
      return message_length;
    }
  return 0;
}

bool
client_init (client_transport_t *transport, char *folder)
{
  client.tran = transport;
  strncpy (client.folder, folder, MAX_FILE_WITH_PATH);
  client.folder[MAX_FILE_WITH_PATH-1] = '\0';
  is_inited = true;

  return true;
}

bool
client_connect (void)
{
  if (is_inited)
    {
      return client.tran->con ();
    }

  log_error ("Not initialized");
  return false;
}

uint32_t
get_file_size_from_server (char filename[])
{

  uint32_t file_size;
  if (!is_inited)
    {
      log_error ("Not initialized");
      return 0;
    }

  /*Assemble message to get the file size from the server*/
  uint32_t message_length = sizeof (message_header_t) + strlen (filename);
  char *message = malloc (message_length);
  if (message == NULL)
    {
      log_error ("Fail to allocate memory for message");
      return 0;
    }
  uint32_t message_id = REQUEST_FILE_SIZE;
  /*convert to transport byte order*/
  message_id = host_to_transport_long (message_id);
  uint32_t transport_message_length = host_to_transport_long (message_length);
  memcpy (message, (char *)&message_id, sizeof (message_id));
  memcpy (message + sizeof (uint32_t), (char *)&transport_message_length,
          sizeof (transport_message_length));
  memcpy (message + sizeof (message_header_t), filename, strlen (filename));
  int bytes_sent = client.tran->send (message, message_length);
  if ((uint32_t)bytes_sent != message_length)
    {
      log_error (
          "Fail to send message, return %d is not equal to message length(%d)",
          bytes_sent, message_length);
      free (message);
      return 0;
    }

  /*wait for the response*/
  uint32_t expected_response_message_len
      = sizeof (message_header_t) + sizeof (file_size);
  if (expected_response_message_len > message_length)
    {
      /*Re-allocate memory*/
      free (message);
      message = malloc (message_length);
      if (message == NULL)
        {
          log_error ("Fail to re-allocate memory");
          return 0;
        }
    }

  // waiting for the response message
  int received_bytes
      = client.tran->recv (message, expected_response_message_len);
  if (received_bytes == 0)
    {
      log_error ("socket is broken");
      free (message);
      return 0;
    }

  while ((uint32_t)received_bytes < expected_response_message_len)
    {
      int bytes
          = client.tran->recv (message + received_bytes,
                              expected_response_message_len - received_bytes);
      if (bytes == 0)
        {
          log_error ("socket is broken");
          free (message);
          return 0;
        }

      received_bytes += bytes;
    }

  uint32_t resp_message_id;
  memcpy ((char *)&resp_message_id, message, sizeof (uint32_t));
  resp_message_id = transport_to_host_long (resp_message_id);
  uint32_t resp_message_length;
  memcpy ((char *)&resp_message_length, message + sizeof (resp_message_id),
          sizeof (resp_message_length));
  resp_message_length = transport_to_host_long (resp_message_length);

  memcpy ((char *)&file_size, message + sizeof (message_header_t), sizeof (file_size));
  file_size = transport_to_host_long (file_size);
  free (message);

  if ((resp_message_id == RESP_FILE_SIZE_REQUEST)
      && (resp_message_length == expected_response_message_len))
    {
      return file_size;
    }
  else
    {
      log_error ("received message doesn't match, id[0x%x], length[%d], file "
                 "size[%d]",
                 resp_message_id, resp_message_length, file_size);
      return 0;
    }
}

bool
client_download (char filename[], char *folder)
{
  if (!is_inited)
    {
      log_error ("Not initialized");
      return false;
    }

  // Assemble the message
  uint32_t message_length = sizeof (message_header_t) + strlen (filename);
  char *message = malloc (message_length);
  if (message == NULL)
    {
      log_error ("Fail to allocate memory for message");
      return false;
    }

  uint32_t message_id = REQUEST_TRANSFER_FILE;
  message_id = host_to_transport_long (message_id);
  memcpy (message, (char *)&message_id, sizeof (uint32_t));

  uint32_t transport_message_length = host_to_transport_long (message_length);

  memcpy (message + sizeof (uint32_t), (char *)&transport_message_length,
          sizeof (uint32_t));
  memcpy (message + sizeof (message_header_t), filename, strlen (filename));
  int bytes_sent = client.tran->send (message, message_length);
  free (message);
  if ((uint32_t)bytes_sent != message_length)
    {
      log_error (
          "Fail to send message, return %d is not equal to message length(%d)",
          bytes_sent, message_length);
      return false;
    }

  /*Get the file data and write it to file. When receiving a tranfer message
   without payload, it means all data have been transfered.*/

  char file_to_save[MAX_FILE_WITH_PATH];
  concatenate_strings(file_to_save, folder, filename, MAX_FILE_WITH_PATH);
  FILE *fp = fopen (file_to_save, "w");
  if (fp == NULL)
    {
      log_error ("Fail to open file %s", file_to_save);
      return false;
    }

  message_length = get_transfer_message_length ();
  while (message_length > sizeof (message_header_t))
    {
      if (!download_file_data_block_and_save_to_file (
              fp, message_length - sizeof (message_header_t)))
        {
          fclose (fp);
          return false;
        }
      message_length = get_transfer_message_length ();
    }

  if (message_length == sizeof (message_header_t))
    {
      log_info ("All file data have been downloaded and saved");
      return true;
    }
  else
    {
      return false;
    }
}

void
client_disconnect ()
{
  if (is_inited)
    {
      client.tran->discon ();
    }else {

  log_error ("Not initialized");
    }
}

void
client_deinit ()
{
  client.tran = NULL;
  is_inited = false;
  return;
}