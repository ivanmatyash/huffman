binary: main.o libheap.a
	gcc -o binary main.o -L. -lheap
main.o: main.c
	gcc -c main.c
libheap.a: heap.o
	ar cr libheap.a heap.o
heap.o: heap.c heap.h
	gcc -c heap.c
clean:
	rm -rf *.o *.a binary
