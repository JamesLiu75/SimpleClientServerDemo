# SimpleClientServerDemo
A simple Client Server Demo

#server -p port -f folder
#client -s server_ip -p port -f folder -n filename

To be improved:
 - Add crc checking to identify whether the client has the same file as the server does currently.
 - Add Windows support
 - Add other transport support like bluetooth
 - Improve the protocol for non-tcp connection
    + Add crc checking for messages
    + Define message index to re-order on receiver side
    + Define ack/nack message to confirm messages have been received



limitation:
- Server stops when files are being modified
- Client/Server only support linux at the moment
- Only allow 10 clients to connect to the server at the same time
- Only support IPV4 at the moment

