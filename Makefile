test: thsort.c test.c
	gcc -o test test.c thsort.c -lpthread -lm -D_REENTRANT

clean:
	rm test
