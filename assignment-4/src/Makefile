all:
	gcc -c malloc/*.h
	gcc -c malloc/*.c
	gcc -c main.c
	gcc -Imalloc -o main.exe main.o memory_allocation.o


clean:
	rm main.exe