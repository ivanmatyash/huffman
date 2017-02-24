huff: main.c encode.c heap.c
	gcc -o huff  main.c encode.c heap.c
heap.c: heap.h

main.c: encode.c encode.h

clean:
	rm -rf *.o *.a huff 
