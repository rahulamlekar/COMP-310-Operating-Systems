# Boot up the servers
./server.exe 1 &
./server.exe 2 &

# Make sure server has booted
sleep 3

# Boot up some clients
./client.exe 1 1 2 3 &
./client.exe 2 1 4 5 &
./client.exe 3 1 6 7 &
./client.exe 4 1 8 9 &
./client.exe 5 1 2 3 &
./client.exe 6 1 4 5 &
./client.exe 7 1 6 7 &
./client.exe 8 1 8 9 &

# Some printer 2 clients
./client.exe 11 2 2 3 &
./client.exe 12 2 4 5 &
./client.exe 13 2 6 7 &
./client.exe 14 2 8 9 &
./client.exe 15 2 2 3 &
./client.exe 16 2 4 5 &
./client.exe 17 2 6 7 &
./client.exe 18 2 8 9 &



