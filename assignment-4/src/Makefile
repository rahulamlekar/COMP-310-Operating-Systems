all:
	gcc -c malloc/*.h
	gcc -c malloc/*.c
	gcc -c tests.c
	gcc -Imalloc -o tests.exe tests.o malloc.o


clean:
	rm tests.exe
	rm *.o
	rm *.gch
	rm malloc/*.gch