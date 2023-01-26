# SimpleClientServerDemo
A simple Client Server Demo

A server waits for multiple clients to connect and request to transfer a file.

A client requests the server to transfer the specified file and save it into local folder. 
-The client connects to the server.
-Then the client sents a message to the server to get the size of the specified file.
-The server will send a reply message with the file size inside.
-If the file size is 0, it means the file is empty or the server can't find the file. The client will exit.
-Then the client will check if there is old one in the destination folder and whether it's the same as the one from the server.
-If it's not same, then the client rename the old one with .org as back up and then send a message to the server to transfer the file.
-The server will divide the file into serveral blocks and tansfer them one by one by sending a transfer message with the data as payload to the client.
-If all blocks have been transfered, the server will send the last transfer message without payload to the client which means there is no more data to transfer.
-If the client receives the new file susccessfully, it will delete the backup file. Otherwise it will restore the backup file.


A flexible architecture is defined to allow the communciation between the client and the server through different ways like network, bluetooth or even uart/i2c.
Network is implemented only currently.

Client/Server are supposed to run on linux/window or even embedded platform. Only linux is supported at the moment.

Several messages are defined in order to transfer files from server to client. But it's open for more messages to add more functionalities in future.

How to build:(have cmake installed on linux system)
#cmake -S <path-to-source> -B <path-to-build>
#cmake --build <path-to-build>

How to run:
#server <port> <folder> <max-connection-number>
#client <server-ip> <server-port> <file> <dst-folder>


To be improved:
 - Use crc/hash to check whether the client has the same file as the server does.
 - Add unit test.

limitation:
- Only support IPV4 at the moment

