#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "transport.h"
#include "protocol.h"
#include "server.h"
#include "utils.h"

#define INVALID_CLIENT_ID -1
#define TRANSFER_FILE_BLOCK_SIZE 4096

static bool is_inited = false;
static pthread_mutex_t mutex;

typedef struct {
    server_transport_t* tran;
    char folder[MAX_FILE_WITH_PATH];
    uint32_t max_connections;
}server_t;

typedef struct {
    pthread_t thread;
    int       client_id;
    bool      is_valid;
}connection_t;

static connection_t* connections = NULL;
static server_t server;

static void init_connections(uint32_t max_connection_num){
    if(connections != NULL){
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < max_connection_num; i++){
            connections[i].client_id = INVALID_CLIENT_ID;
            connections[i].is_valid = false;
        }
        pthread_mutex_unlock(&mutex);
    }
}

static connection_t* allocate_connection(int client_id){
    connection_t* connection = NULL;
    if(connections != NULL){
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < server.max_connections; i++){
            if(connections[i].client_id == INVALID_CLIENT_ID) {
                connection = &connections[i];
                connection->client_id = client_id;
                connection->is_valid = true;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    return connection;
}

static void free_connection(connection_t *connection){
    if(connection != NULL){
        pthread_mutex_lock(&mutex);
        connection->client_id = INVALID_CLIENT_ID;
        connection->is_valid = false;
        pthread_mutex_unlock(&mutex);
    }
}

static error_code_t reply_file_size_request_message(int client_id,char* filename){
    if(filename == NULL){
        return NULL_POINTER;
    }

    char file_with_path[MAX_FILE_WITH_PATH];
    strncpy(file_with_path, server.folder, MAX_FILE_WITH_PATH);
    strcat(file_with_path, filename);
    FILE *fp = fopen(file_with_path, "r");
    if(fp == NULL){
        log_error("Fail to open file %s", file_with_path);
        return IO_ERROR;
    }

    uint32_t file_size = get_file_size(fp);
    log_debug("The size of File (%s) is %ld", file_with_path, file_size );
    
    // assemble the reply message
    uint32_t message_length = sizeof(message_header_t) + sizeof(file_size);
   char* message = malloc(message_length);
   if(message == NULL){
       log_error("Fail to allocate memory for message");
      return MEMORY_ERROR;
   }

    uint32_t message_id = RESP_FILE_SIZE_REQUEST;
    /*convert to transport byte order*/
    message_id = host_to_transport_long(message_id);
    uint32_t transport_message_length = host_to_transport_long(message_length);
    memcpy(message, (char *)&message_id,sizeof(message_id));
    memcpy(message + sizeof(uint32_t), (char *)&transport_message_length,sizeof(transport_message_length));
    file_size = host_to_transport_long(file_size);
    memcpy(message + sizeof(message_header_t),(char *)&file_size, sizeof(file_size));
    int bytes_sent = server.tran->send(client_id,message, message_length);
    free(message);
    if(bytes_sent != message_length){
      log_error("Fail to send message, return %d is not equal to message length(%d)",bytes_sent,message_length);
      return TRANSPORT_ERROR;
    }

    return NO_ERROR;
}
static error_code_t send_transfer_message(int client_id, char* payload, uint32_t payload_length){
     
    uint32_t message_length = sizeof(message_header_t) + payload_length;
   char* message = malloc(message_length);
   if(message == NULL){
       log_error("Fail to allocate memory for message");
      return MEMORY_ERROR;
   }

    uint32_t message_id = TRANSFER_DATA;
    /*convert to transport byte order*/
    message_id = host_to_transport_long(message_id);
    uint32_t transport_message_length = host_to_transport_long(message_length);
    memcpy(message, (char *)&message_id,sizeof(message_id));
    memcpy(message + sizeof(uint32_t), (char *)&transport_message_length,sizeof(transport_message_length));
    
    if(payload_length != 0){
        memcpy(message + sizeof(message_header_t),payload, payload_length);
    }
    int bytes_sent = server.tran->send(client_id,message, message_length);
    free(message);
    if(bytes_sent != message_length){
      log_error("Fail to send message, return %d is not equal to message length(%d)",bytes_sent,message_length);
      return TRANSPORT_ERROR;
    }

    return NO_ERROR;
}

static error_code_t reply_transfer_file_request_message(int client_id,char* filename){
    if(filename == NULL){
        return NULL_POINTER;
    }

    char file_with_path[MAX_FILE_WITH_PATH];
    strncpy(file_with_path, server.folder, MAX_FILE_WITH_PATH);
    strcat(file_with_path, filename);
    FILE *fp = fopen(file_with_path, "r");
    if(fp == NULL){
        log_error("Fail to open file %s", file_with_path);
        return IO_ERROR;
    }

    char data_block[TRANSFER_FILE_BLOCK_SIZE];
    size_t read_bytes = fread(data_block, TRANSFER_FILE_BLOCK_SIZE, 1, fp);
    while(read_bytes > 0) {
        /*Assemble message and send the data to client*/
        error_code_t error = send_transfer_message(client_id, data_block, read_bytes);
        if( error != NO_ERROR){
            fclose(fp);
            return error;
        }
        /*Read the next data block*/
        read_bytes = fread(data_block, TRANSFER_FILE_BLOCK_SIZE, 1, fp);
    }

    fclose(fp);
    // reach the end of file, send the message without payload
    return send_transfer_message(client_id, data_block, 0);
}

static error_code_t get_requested_filename(int client_id,char* buffer, uint32_t filename_len){
    int received_bytes = server.tran->recv(client_id, buffer, filename_len);
    if(received_bytes == 0){
      log_error("socket is broken");
      return TRANSPORT_ERROR;
    }

    while(received_bytes < filename_len){
        int bytes = server.tran->recv(client_id,buffer+received_bytes, filename_len-received_bytes);
        if(bytes == 0){
          log_error("socket is broken");
          return TRANSPORT_ERROR;
        }
        
        received_bytes += bytes;
    }

    return NO_ERROR;
}

static error_code_t get_message_header(int client_id,message_header_t* header, uint32_t message_header_size) {
    if(header == NULL){
        log_error("Invalid message header pointer");
        return NULL_POINTER;
    }

    char* message_header = malloc(message_header_size);
    if(message_header == NULL){
        log_error("Fail to allocate memory for message header");
        return MEMORY_ERROR;
    }

    // waiting for the message
    int received_bytes = server.tran->recv(client_id,message_header, message_header_size);
    if(received_bytes == 0){
      log_error("socket is broken");
      return TRANSPORT_ERROR;
    }

    while(received_bytes < message_header_size){
        int bytes = server.tran->recv(client_id,message_header+received_bytes, message_header_size-received_bytes);
        if(bytes == 0){
          log_error("socket is broken");
          return TRANSPORT_ERROR;
        }
        
        received_bytes += bytes;
    }

    uint32_t message_id;
    memcpy((char *)&message_id, message_header, sizeof(uint32_t));
    /*convert to host byte order*/
    message_id = transport_to_host_long(message_id);
    
    uint32_t message_length;
    memcpy((char *)&message_length, message_header+sizeof(message_id), sizeof(message_length));
    /*convert to host byte order*/
    message_length = transport_to_host_long(message_length);
    log_info("Got message header, message id[0x%x], message length[%d]",message_id, message_length);
    
    header->message_id = message_id;
    header->payload_length = message_length;

    return NO_ERROR;
}

static error_code_t wait_and_handle_message(int client_id){
    uint32_t message_header_size = sizeof(message_header_t);
    message_header_t* message_header = (message_header_t *)malloc(message_header_size);
    if(message_header == NULL){
        log_error("Fail to allocate memory for message header");
        return MEMORY_ERROR;
    }

    error_code_t error = get_message_header(client_id, message_header, message_header_size);
    if(error == NO_ERROR){
        switch(message_header->message_id){
            case REQUEST_FILE_SIZE:
            {
                char filename[MAX_FILE_WITH_PATH];
                memset(filename,0,MAX_FILE_WITH_PATH);
                uint32_t filename_length = message_header->payload_length;
                 error = get_requested_filename(client_id,filename,filename_length);
                 if(error == NO_ERROR){
                     error = reply_file_size_request_message(client_id,filename);
                 }else {
                    log_error("Fail to get the requested file name");
                 }
            }break;
            case REQUEST_TRANSFER_FILE:
            {
                char filename[MAX_FILE_WITH_PATH];
                memset(filename,0,MAX_FILE_WITH_PATH);
                uint32_t filename_length = message_header->payload_length;
                error = get_requested_filename(client_id, filename,filename_length);
                if(error == NO_ERROR){
                    error = reply_transfer_file_request_message(client_id,filename);
                }else{
                    log_error("Fail to get the requested file name");
                }
            }break;

            default:
                 log_error("Unsupported message id 0x%x", message_header->message_id);
                 error = SUPPORT_ERROR;
                 break;
        }

    }

    return error;
}

static void * start_connection(void* ptr) {
    connection_t *connection = (connection_t *)ptr;
   if(connection != NULL){
       do {
          if(wait_and_handle_message(connection->client_id) == TRANSPORT_ERROR)
          {
            log_error("Connection is broken");
            break;
          }
       }while(connection->is_valid);
       free_connection(connection);
   }else {
    log_error("Invalid parameter passed into the thread");
   }
}

static void on_connection(int client_id){
    connection_t* con = allocate_connection(client_id);
    if(con != NULL){
        pthread_t thread;
        if( pthread_create(&thread, NULL, &start_connection, con) == 0){
            con->thread = thread;
        }else{
            log_error("Fail to create a thread for the new connection");
            free_connection(con);
        }
    }else {
        log_error("Fail to allocate a new connection");
    }
}

bool server_init(server_transport_t *transport, uint32_t max_connection_num, char* folder){
    if((transport == NULL) || (max_connection_num == 0 ) || (folder == NULL)){
        log_error(" Invalid parameters");
        return false;
    }

  connections = malloc(max_connection_num*sizeof(connection_t));
  if(connections){
    init_connections(max_connection_num);
    server.tran = transport;
    strncpy(server.folder, folder,MAX_FILE_WITH_PATH); 
    server.max_connections = max_connection_num;
    is_inited = true;

    return true;
  }

  return false;
}

bool server_run (){
   return server.tran->listen(&on_connection, server.max_connections);
}

void server_exit (){
    if(connections){
        free(connections);
    }
   return;
}
