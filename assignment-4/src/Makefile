all:
	gcc -c malloc/*.h
	gcc -c malloc/*.c
	gcc -c main.c
	gcc -c unit_tests.c
	gcc -Imalloc -o main.exe main.o memory_allocation.o
	gcc -Imalloc -o unit_tests.exe unit_tests.o 


clean:
	rm main.exe