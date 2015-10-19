# Boot up the server
./server.exe &

# Make sure server has booted
sleep 3

# Boot up some clients
./client.exe 1 2 3 &
./client.exe 2 4 5 &
./client.exe 3 6 7 &
./client.exe 4 8 9 &

