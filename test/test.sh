#!/bin/sh

FOLDER=test
if [ -d "$FOLDER" ];
then
    echo "$FOLDER directory exists."
else
	echo "$FOLDER directory does not exist, create it now."
    mkdir $FOLDER
fi

SRC_FOLDER=test/src
if [ -d "$SRC_FOLDER" ];
then
    echo "$SRC_FOLDER directory exists. remove it now."
    rm -rf $SRC_FOLDER
else
	echo "$SRC_FOLDER directory does not exist."
fi
mkdir $SRC_FOLDER

DST_FOLDER=test/dst
if [ -d "$DST_FOLDER" ];
then
    echo "$DST_FOLDER directory exists. remove it now."
    rm -rf $DST_FOLDER
else
	echo "$DST_FOLDER directory does not exist."
fi
mkdir $DST_FOLDER

#generate test data files
yes text 1 | head -c 101MB > $SRC_FOLDER/test1.txt
yes text 2 | head -c 102MB > $SRC_FOLDER/test2.txt
yes text 3 | head -c 103MB > $SRC_FOLDER/test3.txt
yes text 4 | head -c 104MB > $SRC_FOLDER/test4.txt
yes text 5 | head -c 105MB > $SRC_FOLDER/test5.txt
yes text 6 | head -c 106MB > $SRC_FOLDER/test6.txt
yes text 7 | head -c 107MB > $SRC_FOLDER/test7.txt

#start server
server/server 1234 $PWD/test/src/ 10 &
sleep 10s

#start multiple clients
client/client 127.0.01 1234 test1.txt $PWD/test/dst/ &
client/client 127.0.01 1234 test2.txt $PWD/test/dst/ &
client/client 127.0.01 1234 test3.txt $PWD/test/dst/ &
client/client 127.0.01 1234 test4.txt $PWD/test/dst/ &
client/client 127.0.01 1234 test5.txt $PWD/test/dst/ &
client/client 127.0.01 1234 test6.txt $PWD/test/dst/ &
client/client 127.0.01 1234 test7.txt $PWD/test/dst/ &

#waiting for all clients closed
sleep 10s

#kill server 
pkill -15 server

#compare the source files and downloaded files
diff test/src test/dst