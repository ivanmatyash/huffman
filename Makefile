huff: main.c encode.c decode.c heap.c heap.h
	gcc -o huff  main.c encode.c decode.c heap.c 
heap.c: heap.h

main.c: encode.c encode.h

clean:
	rm -rf *.o *.a huff 
