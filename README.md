# SimpleClientServerDemo
A simple Client Server Demo

A server waits for multiple clients to connect and request to transfer a file.

A client requests the server to transfer the specified file and save it into local folder. 


A flexible architecture is defined to allow the communciation between the client and the server through different ways like network, bluetooth or even uart/i2c.
Network is implemented only currently.

Client/Server are supposed to run on linux/window or even embedded platform. Only linux is supported at the moment.

Several messages are defined in order to transfer files from server to client. But it's open for more messages to add more functionalities in future.


To be improved:
 - Use crc/hash to identify whether the client has the same file as the server does currently.

limitation:
- Only support IPV4 at the moment

