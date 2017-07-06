test: thsort.c test.c
	gcc -o test test.c thsort.c -lpthread -lm -D_REENTRANT

run:
	./ test

clean:
	rm test
